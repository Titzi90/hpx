//  Copyright (c) 2008-2009 Chirag Dekate, Hartmut Kaiser, Anshul Tandon
//  Copyright (c)      2011 Bryce Lelbach
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <hpx/exception.hpp>
#include <hpx/runtime/threads/thread_affinity.hpp>      // must be first header!
#include <hpx/runtime/threads/threadmanager.hpp>
#include <hpx/runtime/threads/thread.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/include/performance_counters.hpp>
#include <hpx/util/unlock_lock.hpp>
#include <hpx/util/logging.hpp>
#include <hpx/util/block_profiler.hpp>
#include <hpx/util/time_logger.hpp>
#include <hpx/util/itt_notify.hpp>
#include <hpx/util/stringstream.hpp>

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <numeric>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace threads
{
    ///////////////////////////////////////////////////////////////////////////
    namespace strings
    {
        char const* const thread_state_names[] = 
        {
            "unknown",
            "active",
            "pending",
            "suspended",
            "depleted",
            "terminated"
        };
    }

    char const* get_thread_state_name(thread_state_enum state)
    {
        if (state < unknown || state > terminated)
            return "unknown";
        return strings::thread_state_names[state];
    }

    ///////////////////////////////////////////////////////////////////////////
    namespace strings
    {
        char const* const thread_priority_names[] = 
        {
            "default",
            "low",
            "normal",
            "critical"
        };
    }

    char const* get_thread_priority_name(thread_priority priority)
    {
        if (priority < thread_priority_low || priority > thread_priority_critical)
            return "unknown";
        return strings::thread_priority_names[priority];
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    threadmanager_impl<SchedulingPolicy, NotificationPolicy>::threadmanager_impl(
            util::io_service_pool& timer_pool, 
            scheduling_policy_type& scheduler,
            notification_policy_type& notifier)
      : startup_(NULL),
        thread_count_(0),
        state_(starting),
        timer_pool_(timer_pool), 
        thread_logger_("threadmanager_impl::register_thread"),
        work_logger_("threadmanager_impl::register_work"),
        set_state_logger_("threadmanager_impl::set_state"),
        scheduler_(scheduler),
        notifier_(notifier)
    {
        //LTM_(debug) << "threadmanager_impl ctor";
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    threadmanager_impl<SchedulingPolicy, NotificationPolicy>::~threadmanager_impl() 
    {
        //LTM_(debug) << "~threadmanager_impl";
        if (!threads_.empty()) {
            if (state_.load() == running) 
                stop();
            threads_.clear();
        }
        delete startup_;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    boost::uint64_t threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_thread_count(thread_state_enum state) const
    {
        mutex_type::scoped_lock lk(mtx_);
        return scheduler_.get_thread_count(state);
    }

    ///////////////////////////////////////////////////////////////////////////
    // \brief Abort all threads which are in suspended state. This will set 
    //        the state of all suspended threads to \a pending while 
    //        supplying the wait_abort extended state flag
    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        abort_all_suspended_threads()
    {
        mutex_type::scoped_lock lk(mtx_);
        scheduler_.abort_all_suspended_threads();
    }

    ///////////////////////////////////////////////////////////////////////////
    // \brief Clean up terminated threads. This deletes all threads which 
    //        have been terminated but which are still held in the queue 
    //        of terminated threads. Some schedulers might not do anything
    //        here.
    template <typename SchedulingPolicy, typename NotificationPolicy>
    bool threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        cleanup_terminated()
    {
        mutex_type::scoped_lock lk(mtx_);
        return scheduler_.cleanup_terminated();
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_id_type threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        register_thread(thread_init_data& data, thread_state_enum initial_state, 
            bool run_now, error_code& ec)
    {
        util::block_profiler_wrapper<register_thread_tag> bp(thread_logger_);

        // verify state
        if (thread_count_ == 0 && !(state_.load() == running)) 
        {
            // threadmanager is not currently running
            HPX_THROWS_IF(ec, invalid_status,
                "threadmanager_impl::register_thread",
                "invalid state: thread manager is not running");
            return invalid_thread_id;
        }

        // verify parameters
        switch (initial_state) {
        case pending:
        case suspended:
            break;

        default:
            {
                hpx::util::osstream strm;
                strm << "invalid initial state: " 
                     << get_thread_state_name(initial_state);
                HPX_THROWS_IF(ec, bad_parameter,
                    "threadmanager_impl::register_thread",
                    hpx::util::osstream_get_string(strm));
                return invalid_thread_id;
            }
        }

        if (0 == data.description)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "threadmanager_impl::register_thread", "description is NULL");
            return invalid_thread_id;
        }

        if (0 == data.parent_id) {
            thread_self* self = get_self_ptr();
            if (self)
            {
                data.parent_id = self->get_thread_id();
                data.parent_phase = self->get_thread_phase();
            }
        }
        if (0 == data.parent_prefix) 
            data.parent_prefix = applier::get_prefix_id();

        // NOTE: This code overrides a request to schedule a thread on a scheduler
        // selected queue. The schedulers are written to select a queue to put
        // a thread in if the os thread number is -1. Not only does overriding this
        // prevent extensibility by forcing a certain queueing behavior, but it also
        // schedules unfairly. A px thread is always put into the queue of the
        // os thread that it's producer is currently running on. In a single
        // producer environment, this can lead to unexpected inbalances and
        // work only gets distributed by work stealing.
        //if (std::size_t(-1)  == data.num_os_thread) 
        //    data.num_os_thread = get_thread_num();

        // create the new thread
        thread_id_type newid = scheduler_.create_thread(
            data, initial_state, run_now, ec, data.num_os_thread);

        LTM_(info) << "register_thread(" << newid << "): initial_state(" 
                   << get_thread_state_name(initial_state) << "), "
                   << "run_now(" << (run_now ? "true" : "false") << "), "
                   << "description(" << data.description << ")";

        if (&ec != &throws)
            ec = make_success_code();

        return newid;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::register_work(
        thread_init_data& data, thread_state_enum initial_state, error_code& ec)
    {
        util::block_profiler_wrapper<register_work_tag> bp(work_logger_);

        // verify state
        if (thread_count_ == 0 && !(state_.load() == running)) 
        {
            // threadmanager is not currently running
            HPX_THROWS_IF(ec, invalid_status,
                "threadmanager_impl::register_work",
                "invalid state: thread manager is not running");
            return;
        }

        // verify parameters
        switch (initial_state) {
        case pending:
        case suspended:
            break;

        default:
            {
                hpx::util::osstream strm;
                strm << "invalid initial state: " 
                     << get_thread_state_name(initial_state);
                HPX_THROWS_IF(ec, bad_parameter,
                    "threadmanager_impl::register_work",
                    hpx::util::osstream_get_string(strm));
                return;
            }
        }

        if (0 == data.description)
        {
            HPX_THROWS_IF(ec, bad_parameter,
                "threadmanager_impl::register_work", "description is NULL");
            return;
        }

        LTM_(info) << "register_work: initial_state(" 
                   << get_thread_state_name(initial_state) << "), thread_priority("
                   << get_thread_priority_name(data.priority) << "), "
                   << "description(" << data.description << ")";

        if (0 == data.parent_id) {
            thread_self* self = get_self_ptr();
            if (self)
            {
                data.parent_id = self->get_thread_id();
                data.parent_phase = self->get_thread_phase();
            }
        }
        if (0 == data.parent_prefix) 
            data.parent_prefix = applier::get_prefix_id();

        // NOTE: This code overrides a request to schedule a thread on a scheduler
        // selected queue. The schedulers are written to select a queue to put
        // a thread in if the os thread number is -1. Not only does overriding this
        // prevent extensibility by forcing a certain queueing behavior, but it also
        // schedules unfairly. A px thread is always put into the queue of the
        // os thread that it's producer is currently running on. In a single
        // producer environment, this can lead to unexpected inbalances and
        // work only gets distributed by work stealing.
        //if (std::size_t(-1) == data.num_os_thread) 
        //    data.num_os_thread = get_thread_num();

        // create the new thread
        scheduler_.create_thread(data, initial_state, false, ec, data.num_os_thread);

        if (&ec != &throws)
            ec = make_success_code();
    }

    ///////////////////////////////////////////////////////////////////////////
    // thread function registered for set_state if thread is currently active
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_state threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_active_state(thread_id_type id, 
            thread_state_enum newstate, thread_state_ex_enum newstate_ex,
            thread_priority priority)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::set_active_state",
              "NULL thread id encountered");  

        // just retry, set_state will create new thread if target is still active
        error_code ec;      // do not throw
        set_state(id, newstate, newstate_ex, priority, ec);
        return thread_state(terminated);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// The set_state function is part of the thread related API and allows
    /// to change the state of one of the threads managed by this threadmanager_impl
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_state threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_state(thread_id_type id, thread_state_enum new_state, 
            thread_state_ex_enum new_state_ex, thread_priority priority,
            error_code& ec)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROWS_IF(ec, null_thread_id,
              "threadmanager_impl::set_state", "NULL thread id encountered");  

        util::block_profiler_wrapper<set_state_tag> bp(set_state_logger_);

        // set_state can't be used to force a thread into active state
        if (new_state == active) {
            hpx::util::osstream strm;
            strm << "invalid new state: " << get_thread_state_name(new_state);
            HPX_THROWS_IF(ec, bad_parameter, 
                "threadmanager_impl::set_state", hpx::util::osstream_get_string(strm));
            return thread_state(unknown);
        }

        if (&ec != &throws)
            ec = make_success_code();

        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        if (NULL == thrd->get())
            return thread_state(terminated);     // this thread has already been terminated 

        // action depends on the current state
        thread_state previous_state = thrd->get_state();
        thread_state_enum previous_state_val = previous_state;

        // nothing to do here if the state doesn't change
        if (new_state == previous_state_val)
            return thread_state(new_state);

        // the thread to set the state for is currently running, so we 
        // schedule another thread to execute the pending set_state
        if (previous_state_val == active) 
        {
            // schedule a new thread to set the state
            LTM_(warning) << "set_state: " << "thread(" << id << "), "
                          << "is currently active, scheduling new thread...";

            thread_init_data data(
                boost::bind(&threadmanager_impl::set_active_state, this, 
                    id, new_state, new_state_ex, priority), 
                "set state for active thread", 0, priority);
            register_work(data);

            return previous_state;     // done
        }
        else if (previous_state_val == terminated) {
            // If the thread has been terminated while this set_state was 
            // pending nothing has to be done anymore.
            return previous_state;
        }

        // If the previous state was pending we are supposed to remove the
        // thread from the queue. But in order to avoid linearly looking 
        // through the queue we defer this to the thread function, which 
        // at some point will ignore this thread by simply skipping it 
        // (if it's not pending anymore). 

        LTM_(info) << "set_state: " << "thread(" << id << "), "
                   << "description(" << thrd->get_description() << "), "
                   << "new state(" << get_thread_state_name(new_state) << ")";

        // So all what we do here is to set the new state.
        thrd->set_state(new_state);
        thrd->set_state_ex(new_state_ex);

        if (new_state == pending) {
            scheduler_.schedule_thread(thrd, get_thread_num(), priority);
            do_some_work();
        }

        return previous_state;
    }

    /// The get_state function is part of the thread related API. It
    /// queries the state of one of the threads known to the threadmanager_impl
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_state threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_state(thread_id_type id) 
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_state() : thread_state(terminated);
    }
    
    /// The get_phase function is part of the thread related API. It
    /// queries the phase of one of the threads known to the threadmanager_impl
    template <typename SchedulingPolicy, typename NotificationPolicy>
    std::size_t threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_phase(thread_id_type id) 
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_thread_phase() : std::size_t(~0);
    }

    /// The get_description function is part of the thread related API and 
    /// allows to query the description of one of the threads known to the 
    /// threadmanager_impl
    template <typename SchedulingPolicy, typename NotificationPolicy>
    std::string threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_description(thread_id_type id) const
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_description() : "<unknown>";
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_description(thread_id_type id, char const* desc)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::set_description",
              "NULL thread id encountered");  

        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        if (thrd->get()) 
            thrd->set_description(desc);
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    std::string threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_lco_description(thread_id_type id) const
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_lco_description() : "<unknown>";
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_lco_description(thread_id_type id, char const* desc)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::set_lco_description",
              "NULL thread id encountered");  

        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        if (thrd->get()) 
            thrd->set_lco_description(desc);
    }

    /// This thread function is used by the at_timer thread below to trigger
    /// the required action.
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_state_enum threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        wake_timer_thread(thread_id_type id, 
            thread_state_enum newstate, thread_state_ex_enum newstate_ex, 
            thread_priority priority, thread_id_type timer_id, 
            boost::shared_ptr<boost::atomic<bool> > triggered) 
    {
        if (HPX_UNLIKELY(!id))
        {
            HPX_THROW_EXCEPTION(null_thread_id,
                "threadmanager_impl::ake_timer_thread",
                "NULL thread id encountered (id)");  
        }

        else if (HPX_UNLIKELY(!timer_id))
        {
            HPX_THROW_EXCEPTION(null_thread_id,
                "threadmanager_impl::ake_timer_thread",
                "NULL thread id encountered (timer_id)");
        }

        bool oldvalue = false;
        if (triggered->compare_exchange_strong(oldvalue, true))
        {
            // timer has not been canceled yet, trigger the requested set_state 
            set_state(id, newstate, newstate_ex, priority);
        }

        // then re-activate the thread holding the deadline_timer
        error_code ec;    // do not throw
        set_state(timer_id, pending, wait_timeout, thread_priority_normal, ec);
        return terminated;
    }

    /// This thread function initiates the required set_state action (on 
    /// behalf of one of the threadmanager_impl#set_state functions).
    template <typename SchedulingPolicy, typename NotificationPolicy>
    template <typename TimeType>
    thread_state_enum threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        at_timer(TimeType const& expire, thread_id_type id, 
            thread_state_enum newstate, thread_state_ex_enum newstate_ex, 
            thread_priority priority)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::at_timer",
              "NULL thread id encountered");  

        // create a new thread in suspended state, which will execute the 
        // requested set_state when timer fires and will re-awaken this thread, 
        // allowing the deadline_timer to go out of scope gracefully
        thread_self& self = get_self();
        thread_id_type self_id = self.get_thread_id();

        boost::shared_ptr<boost::atomic<bool> > triggered(
            new boost::atomic<bool>(false));

        thread_init_data data(
            boost::bind(&threadmanager_impl::wake_timer_thread, this, id, 
                newstate, newstate_ex, priority, self_id, triggered), 
            "wake_timer", 0, priority);
        thread_id_type wake_id = register_thread(data, suspended);

        // create timer firing in correspondence with given time
        boost::asio::deadline_timer t (timer_pool_.get_io_service(), expire);

        // let the timer invoke the set_state on the new (suspended) thread
        t.async_wait(boost::bind(&threadmanager_impl::set_state, this, wake_id, 
            thread_state(pending), thread_state_ex(wait_timeout), priority,
            boost::ref(throws)));

        // this waits for the thread to be reactivated when the timer fired
        // if it returns 'signalled' the timer has been canceled, otherwise 
        // the tiler fired and the wake_timer_thread above has been executed
        bool oldvalue = false;
        thread_state_ex_enum statex = self.yield(suspended);

        if (wait_timeout != statex && 
            triggered->compare_exchange_strong(oldvalue, true))
        {
            // wake_timer_thread has not been executed yet, cancel timer
            t.cancel();
        }

        return terminated;
    }

    /// Set a timer to set the state of the given \a thread to the given 
    /// new value after it expired (at the given time)
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_id_type threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_state(time_type const& expire_at, thread_id_type id, 
            thread_state_enum newstate, thread_state_ex_enum newstate_ex,
            thread_priority priority, error_code& ec)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::set_state",
              "NULL thread id encountered");  

        // this creates a new thread which creates the timer and handles the
        // requested actions
        thread_state_enum (threadmanager_impl::*f)(time_type const&, 
                thread_id_type, thread_state_enum, thread_state_ex_enum,
                thread_priority)
            = &threadmanager_impl::template at_timer<time_type>;

        thread_init_data data(
            boost::bind(f, this, expire_at, id, newstate, newstate_ex, priority),
            "at_timer (expire at)", 0, priority);
        return register_thread(data, pending, true, ec);
    }

    /// Set a timer to set the state of the given \a thread to the given
    /// new value after it expired (after the given duration)
    template <typename SchedulingPolicy, typename NotificationPolicy>
    thread_id_type threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        set_state(duration_type const& from_now, thread_id_type id, 
            thread_state_enum newstate, thread_state_ex_enum newstate_ex,
            thread_priority priority, error_code& ec)
    {
        if (HPX_UNLIKELY(!id))
           HPX_THROW_EXCEPTION(null_thread_id,
              "threadmanager_impl::set_state",
              "NULL thread id encountered");  

        // this creates a new thread which creates the timer and handles the
        // requested actions
        thread_state_enum (threadmanager_impl::*f)(duration_type const&, 
                thread_id_type, thread_state_enum, thread_state_ex_enum, 
                thread_priority)
            = &threadmanager_impl::template at_timer<duration_type>;

        thread_init_data data(
            boost::bind(f, this, from_now, id, newstate, newstate_ex, priority),
            "at_timer (from now)", 0, priority);
        return register_thread(data, pending, true, ec);
    }

    /// Retrieve the global id of the given thread
    template <typename SchedulingPolicy, typename NotificationPolicy>
    naming::id_type const& 
    threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_thread_gid(thread_id_type id) 
    {
        // we know that the id is actually the pointer to the thread
        thread* thrd = reinterpret_cast<thread*>(id);
        return thrd->get() ? thrd->get_gid() : naming::invalid_id;
    }

    // helper class for switching thread state in and out during execution
    class switch_status
    {
    public:
        switch_status (thread* t, thread_state prev_state)
          : thread_(t), prev_state_(prev_state),
            need_restore_state_(t->set_state_tagged(active, prev_state_, orig_state_))
        {}

        ~switch_status ()
        {
            if (need_restore_state_)
                store_state(prev_state_);
        }

        bool is_valid() const { return need_restore_state_; }

        // allow to change the state the thread will be switched to after 
        // execution
        thread_state operator=(thread_state_enum new_state)
        {
            return prev_state_ = thread_state(new_state);
        }

        // Get the state this thread was in before execution (usually pending),
        // this helps making sure no other OS-thread is started to execute this
        // PX-thread in the meantime.
        thread_state get_previous() const
        {
            return prev_state_;
        }

        // This restores the previous state, while making sure that the 
        // original state has not been changed since we started executing this
        // thread. The function returns true if the state has been set, false
        // otherwise.
        bool store_state(thread_state& newstate)
        {
            disable_restore();
            if (thread_->restore_state(prev_state_, orig_state_)) {
                newstate = prev_state_;
                return true;
            }
            return false;
        }

        // disable default handling in destructor
        void disable_restore() { need_restore_state_ = false; }

    private:
        thread* thread_;
        thread_state prev_state_;
        thread_state orig_state_;
        bool need_restore_state_;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct manage_counter_type
    {
        manage_counter_type()
          : status_(performance_counters::status_invalid_data)
        {
        }
        ~manage_counter_type()
        {
            if (performance_counters::status_invalid_data != status_) {
                error_code ec;
                performance_counters::remove_counter_type(info_, ec);   // ignore errors
            }
        }

        performance_counters::counter_status install(
            std::string const& name, performance_counters::counter_type type, 
            error_code& ec = throws)
        {
            BOOST_ASSERT(performance_counters::status_invalid_data == status_);
            info_.fullname_ = name;
            info_.type_ = type;
            status_ = performance_counters::add_counter_type(info_, ec);
            return status_;
        }

        performance_counters::counter_status status_;
        performance_counters::counter_info info_;
    };

    ///////////////////////////////////////////////////////////////////////////
    // main function executed by all OS threads managed by this threadmanager_impl
    template <typename SP, typename NP>
    struct init_tss_helper
    {
        typedef threadmanager_impl<SP, NP> threadmanager_type;

        init_tss_helper(threadmanager_type& tm, std::size_t thread_num, 
                bool numa_sensitive)
          : tm_(tm)
        {
            tm_.init_tss(thread_num, numa_sensitive);
        }
        ~init_tss_helper()
        {
            tm_.deinit_tss();
        }

        threadmanager_type& tm_;
    };

    struct manage_active_thread_count
    {
        manage_active_thread_count(boost::atomic<long>& counter)
          : has_exited_(false), counter_(counter)
        {
            ++counter_;
        }
        ~manage_active_thread_count()
        {
            if (!has_exited_)
                --counter_;
        }

        void exit()
        {
            has_exited_ = true;
            --counter_;
        }

        bool has_exited_;
        boost::atomic<long>& counter_;
    };

    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        tfunc(std::size_t num_thread, std::size_t& num_px_threads)
    {
        // wait for all threads to start up before before starting px work
        startup_->wait();   

        // manage the number of this thread in its TSS
        init_tss_helper<SchedulingPolicy, NotificationPolicy> 
            tss_helper(*this, num_thread, scheduler_.numa_sensitive());

        // needs to be done as the first thing, otherwise logging won't work
        notifier_.on_start_thread(num_thread);       // notify runtime system of started thread
        scheduler_.on_start_thread(num_thread);

        {
#if HPX_AGAS_VERSION <= 0x10
            manage_counter_type counter_type;
            if (0 == num_thread) {
                // register counter types
                error_code ec;
                counter_type.install("/queue/length", 
                    performance_counters::counter_raw, ec);   // doesn't throw
                if (ec) {
                    LTM_(info) << "tfunc(" << num_thread << "): failed to install "
                        "counter type '/queue/length': " << ec.get_message();
                }
            }
#endif

            LTM_(info) << "tfunc(" << num_thread << "): start";
            try {
                num_px_threads = tfunc_impl(num_thread);
            }
            catch (hpx::exception const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught hpx::exception: " 
                        << e.what() << ", aborted thread execution";
                report_error(num_thread, boost::current_exception());
                return;
            }
            catch (boost::system::system_error const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught boost::system::system_error: " 
                        << e.what() << ", aborted thread execution";
                report_error(num_thread, boost::current_exception());
                return;
            }
            catch (std::exception const& e) {
                LFATAL_ << "tfunc(" << num_thread 
                        << "): caught std::exception: " 
                        << e.what() << ", aborted thread execution";
                report_error(num_thread, boost::current_exception());
                return;
            }
            catch (...) {
                LFATAL_ << "tfunc(" << num_thread << "): caught unexpected "
                    "exception, aborted thread execution";
                report_error(num_thread, boost::current_exception());
                return;
            }

            LTM_(fatal) << "tfunc(" << num_thread << "): end, executed " 
                       << num_px_threads << " HPX threads";
        }

        notifier_.on_stop_thread(num_thread);
        scheduler_.on_stop_thread(num_thread);
    }

    ///////////////////////////////////////////////////////////////////////////
    struct manage_counter
    {
        ~manage_counter()
        {
            uninstall();
        }

        performance_counters::counter_status install(
            std::string const& name, boost::function<boost::int64_t()> f, 
            error_code& ec = throws)
        {
            BOOST_ASSERT(!counter_);
            info_.fullname_ = name;
            return performance_counters::add_counter(info_, f, counter_, ec);
        }

        void uninstall()
        {
            if (counter_) {
                error_code ec;
                performance_counters::remove_counter(info_, counter_, ec);
                counter_ = naming::invalid_id;
            }
        }

        performance_counters::counter_info info_;
        naming::id_type counter_;
    };

    ///////////////////////////////////////////////////////////////////////////
    inline void write_old_state_log(std::size_t num_thread, thread* thrd, 
        thread_state_enum state)
    {
        LTM_(debug) << "tfunc(" << num_thread << "): "
                   << "thread(" << thrd->get_thread_id() << "), " 
                   << "description(" << thrd->get_description() << "), "
                   << "old state(" << get_thread_state_name(state) << ")";
    }

    inline void write_new_state_log_debug(std::size_t num_thread, thread* thrd, 
        thread_state_enum state, char const* info)
    {
        LTM_(debug) << "tfunc(" << num_thread << "): "
                   << "thread(" << thrd->get_thread_id() << "), "
                   << "description(" << thrd->get_description() << "), "
                   << "new state(" << get_thread_state_name(state) << "), "
                   << info;
    }
    inline void write_new_state_log_warning(std::size_t num_thread, thread* thrd, 
        thread_state_enum state, char const* info)
    {
        LTM_(debug) << "tfunc(" << num_thread << "): "
                   << "thread(" << thrd->get_thread_id() << "), "
                   << "description(" << thrd->get_description() << "), "
                   << "new state(" << get_thread_state_name(state) << "), "
                   << info;
    }

#if HPX_USE_ITT != 0
    ///////////////////////////////////////////////////////////////////////////
    struct itt_caller_context
    {
        itt_caller_context() 
          : itt_context_(0)
        {
            HPX_ITT_STACK_CREATE(itt_context_);
        }
        ~itt_caller_context()
        {
            HPX_ITT_STACK_DESTROY(itt_context_);
        }

        struct ___itt_caller* itt_context_;
    };

    struct caller_context
    {
        caller_context(itt_caller_context& ctx) 
          : ctx_(ctx) 
        {
            HPX_ITT_STACK_CALLEE_ENTER(ctx_.itt_context_);
        }
        ~caller_context()
        {
            HPX_ITT_STACK_CALLEE_LEAVE(ctx_.itt_context_);
        }

        itt_caller_context& ctx_;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct itt_frame_context
    {
        itt_frame_context(char const* name) 
          : itt_frame_(0)
        {
            HPX_ITT_FRAME_CREATE(itt_frame_, name);
            HPX_ITT_FRAME_BEGIN(itt_frame_);
        }
        ~itt_frame_context()
        {
            HPX_ITT_FRAME_END(itt_frame_);
            HPX_ITT_FRAME_DESTROY(itt_frame_);
        }

        struct __itt_frame_t* itt_frame_;
    };

    struct undo_frame_context
    {
        undo_frame_context(itt_frame_context& frame) 
          : frame_(frame) 
        {
            HPX_ITT_FRAME_END(frame_.itt_frame_);
        }
        ~undo_frame_context() 
        {
            HPX_ITT_FRAME_BEGIN(frame_.itt_frame_);
        }

        itt_frame_context& frame_;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct itt_mark_context
    {
        itt_mark_context(char const* name) 
          : itt_mark_(0), name_(name)
        {
            HPX_ITT_MARK_CREATE(itt_mark_, name);
        }
        ~itt_mark_context()
        {
            HPX_ITT_MARK_OFF(itt_mark_);
        }

        int itt_mark_;
        char const* name_;
    };

    struct undo_mark_context
    {
        undo_mark_context(itt_mark_context& mark) 
          : mark_(mark) 
        {
            HPX_ITT_MARK_OFF(mark_.itt_mark_);
        }
        ~undo_mark_context() 
        {
            HPX_ITT_MARK_CREATE(mark_.itt_mark_, mark_.name_);
        }

        itt_mark_context& mark_;
    };
#else
    ///////////////////////////////////////////////////////////////////////////
    struct itt_caller_context
    {
        itt_caller_context() {}
        ~itt_caller_context() {}
    };

    struct caller_context
    {
        caller_context(itt_caller_context&) {}
        ~caller_context() {}
    };

    ///////////////////////////////////////////////////////////////////////////
    struct itt_frame_context
    {
        itt_frame_context(char const* name) {}
        ~itt_frame_context() {}
    };

    struct undo_frame_context
    {
        undo_frame_context(itt_frame_context&) {}
        ~undo_frame_context() {}
    };

    ///////////////////////////////////////////////////////////////////////////
    struct itt_mark_context
    {
        itt_mark_context(char const* name) {}
        ~itt_mark_context() {}
    };

    struct undo_mark_context
    {
        undo_mark_context(itt_mark_context&) {}
        ~undo_mark_context() {}
    };
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    std::size_t threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        tfunc_impl(std::size_t num_thread)
    {
        itt_caller_context ctx_;        // helper for itt support
        itt_mark_context mark_ ("threadmanager");

        manage_active_thread_count count(thread_count_);

        std::size_t num_px_threads = 0;
        util::time_logger tl1("tfunc", num_thread);
        util::time_logger tl2("tfunc2", num_thread);

        // the thread with number zero is the master
#if HPX_AGAS_VERSION <= 0x10
        bool is_master_thread = (0 == num_thread) ? true : false;
#endif
        set_affinity(num_thread, scheduler_.numa_sensitive());     // set affinity on Linux systems

#if HPX_AGAS_VERSION <= 0x10
        // register performance counters
        manage_counter queue_length_counter; 
        if (is_master_thread) {
            std::string name("/queue(threadmanager)/length");
            queue_length_counter.install(name, 
                boost::bind(&scheduling_policy_type::get_queue_lengths, 
                    &scheduler_, num_thread));
        }
#endif

        std::size_t idle_loop_count = 0;
        
        // run the work queue
        boost::coroutines::prepare_main_thread main_thread;
        while (true) {
            // Get the next PX thread from the queue
            thread* thrd = NULL;
            if (scheduler_.get_next_thread(num_thread, state_.load() == running, idle_loop_count, &thrd)) {
                idle_loop_count = 0;
                tl1.tick();

                // Only pending PX threads will be executed.
                // Any non-pending PX threads are leftovers from a set_state() 
                // call for a previously pending PX thread (see comments above).
                thread_state state = thrd->get_state();
                thread_state_enum state_val = state;

                write_old_state_log(num_thread, thrd, state_val);

                if (pending == state_val) {
                    // switch the state of the thread to active and back to 
                    // what the thread reports as its return value

                    {
                        // tries to set state to active (only if state is still 
                        // the same as 'state')
                        switch_status thrd_stat (thrd, state);
                        if (thrd_stat.is_valid() && thrd_stat.get_previous() == pending) {
                            // thread returns new required state
                            // store the returned state in the thread
                            tl2.tick();
                            {
                                undo_mark_context mark (mark_);  // itt support
                                caller_context ctx (ctx_);

                                thrd_stat = (*thrd)();
                            }
                            tl2.tock();
                            ++num_px_threads;
                        }
                        else {
                            // some other OS-thread got in between and started 
                            // executing this PX-thread, we just continue with 
                            // the next one
                            thrd_stat.disable_restore();
                            write_new_state_log_warning(
                                num_thread, thrd, state_val, "no execution");
                            tl1.tock();
                            continue;
                        }

                        // store and retrieve the new state in the thread 
                        if (!thrd_stat.store_state(state)) {
                            // some other OS-thread got in between and changed 
                            // the state of this thread, we just continue with 
                            // the next one
                            write_new_state_log_warning(
                                num_thread, thrd, state_val, "no state change");
                            tl1.tock();
                            continue;
                        }
                        state_val = state;

                        // any exception thrown from the thread will reset its 
                        // state at this point
                    }

                    write_new_state_log_debug(num_thread, thrd, state_val, "normal");

                    // Re-add this work item to our list of work items if the PX
                    // thread should be re-scheduled. If the PX thread is suspended 
                    // now we just keep it in the map of threads.
                    if (state_val == pending) {
                        scheduler_.schedule_thread(thrd, num_thread);
                        do_some_work(num_thread);
                    }
                }
                else if (active == state_val) {
                    // re-schedule thread, if it is still marked as active
                    // this might happen, if some thread has been added to the
                    // scheduler queue already but the state has not been reset 
                    // yet
                    scheduler_.schedule_thread(thrd, num_thread);
                }

                // Remove the mapping from thread_map_ if PX thread is depleted 
                // or terminated, this will delete the PX thread as all 
                // references go out of scope.
                // FIXME: what has to be done with depleted PX threads?
                if (state_val == depleted || state_val == terminated) 
                    scheduler_.destroy_thread(thrd);

                tl1.tock();
            }

            // if nothing else has to be done either wait or terminate
            else if (scheduler_.wait_or_add_new(num_thread, state_.load() == running, idle_loop_count))
            {
                // if we need to terminate, unregister the counter first
#if HPX_AGAS_VERSION <= 0x10
                queue_length_counter.uninstall();
#endif
                count.exit();
                break;
            }
        }

#if HPX_DEBUG != 0
        // the last OS thread is allowed to exit only if no more PX threads exist
        BOOST_ASSERT(!scheduler_.get_thread_count(num_thread));
#endif
        return num_px_threads;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename SchedulingPolicy, typename NotificationPolicy>
    bool threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        run(std::size_t num_threads) 
    {
        LTM_(info) << "run: creating " << num_threads << " OS thread(s)";

        if (0 == num_threads) {
            HPX_THROW_EXCEPTION(bad_parameter, 
                "threadmanager_impl::run", "number of threads is zero");
        }

        mutex_type::scoped_lock lk(mtx_);
        if (!threads_.empty() || (state_.load() == running)) 
            return true;    // do nothing if already running

        LTM_(info) << "run: running timer pool"; 
        timer_pool_.run(false);

        executed_threads_.reserve(num_threads);

        try {
            // run threads and wait for initialization to complete
            BOOST_ASSERT (NULL == startup_);
            startup_ = new boost::barrier(num_threads+1);

            state_.store(running); 

            std::size_t thread_num = num_threads;
            while (thread_num-- != 0) {
                LTM_(info) << "run: create OS thread: " << thread_num;

                // create a new thread
                executed_threads_.push_back(0);
                threads_.push_back(new boost::thread(boost::bind(
                    &threadmanager_impl::tfunc, this, thread_num, 
                    boost::ref(executed_threads_.back()))));

                // set the new threads affinity (on Windows systems)
                set_affinity(threads_.back(), thread_num, scheduler_.numa_sensitive());
            }

            // start timer pool as well
            timer_pool_.run(false);

            // the main thread needs to have a unique thread_num
            init_tss(thread_num, scheduler_.numa_sensitive());
            startup_->wait();
        }
        catch (std::exception const& e) {
            LTM_(fatal) << "run: failed with:" << e.what(); 

            // trigger the barrier
            while (num_threads-- != 0 && !startup_->wait())
                ;

            stop();
            threads_.clear();

            return false;
        }

        LTM_(info) << "run: running"; 
        return true;
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    void threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        stop (bool blocking)
    {
        LTM_(info) << "stop: blocking(" << std::boolalpha << blocking << ")"; 

        deinit_tss();

        mutex_type::scoped_lock l(mtx_);
        if (!threads_.empty()) {
            if (state_.load() == running) {
                state_.store(stopping);
                do_some_work();         // make sure we're not waiting
            }

            if (blocking) {
                for (std::size_t i = 0; i < threads_.size(); ++i) 
                {
                    // make sure no OS thread is waiting
                    LTM_(info) << "stop: notify_all"; 
                    do_some_work();

                    LTM_(info) << "stop(" << i << "): join"; 

                    // unlock the lock while joining
                    util::unlock_the_lock<mutex_type::scoped_lock> ul(l);
                    threads_[i].join();
                }
                threads_.clear();

                LTM_(info) << "stop: stopping timer pool"; 
                timer_pool_.stop();             // stop timer pool as well
                if (blocking) {
                    timer_pool_.join();
                    timer_pool_.clear();
                }
            }
        }
        delete startup_;
        startup_ = NULL;
    }

    template <typename SchedulingPolicy, typename NotificationPolicy>
    std::size_t threadmanager_impl<SchedulingPolicy, NotificationPolicy>::
        get_executed_threads(std::size_t num) const
    {
        if (num != std::size_t(-1)) 
            return executed_threads_[num];

        return std::accumulate(executed_threads_.begin(), executed_threads_.end(), 0);
    }

    ///////////////////////////////////////////////////////////////////////////
    boost::thread_specific_ptr<std::size_t> threadmanager_base::thread_num_;

    void threadmanager_base::init_tss(std::size_t thread_num, bool numa_sensitive)
    {
        BOOST_ASSERT(NULL == threadmanager_base::thread_num_.get());    // shouldn't be initialized yet
        threadmanager_base::thread_num_.reset(
            new std::size_t(thread_num | (std::size_t(0x1) << 31)));
    }

    void threadmanager_base::deinit_tss()
    {
        threadmanager_base::thread_num_.reset();
    }

    std::size_t threadmanager_base::get_thread_num(bool* numa_sensitive)
    {
        if (NULL != threadmanager_base::thread_num_.get())
        {
            std::size_t result = *threadmanager_base::thread_num_;
            if (std::size_t(-1) != result) 
            {
                if (numa_sensitive)
                    *numa_sensitive = (result & (std::size_t(0x1) << 31)) ? true : false;
                return result & ~(std::size_t(0x1) << 31);
            }
        }

        // some OS threads are not managed by the threadmanager
        if (numa_sensitive)
            *numa_sensitive = false;
        return std::size_t(-1);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Return the number of the NUMA node the current thread is running on
    int get_numa_node_number()
    {
        bool numa_sensitive = false;
        std::size_t thread_num = threadmanager_base::get_thread_num(&numa_sensitive);
        return get_numa_node(thread_num, numa_sensitive);
    }
}}

///////////////////////////////////////////////////////////////////////////////
/// explicit template instantiation for the thread manager of our choice
#include <hpx/runtime/threads/policies/global_queue_scheduler.hpp>
#include <hpx/runtime/threads/policies/local_queue_scheduler.hpp>
#include <hpx/runtime/threads/policies/local_priority_queue_scheduler.hpp>
#include <hpx/runtime/threads/policies/abp_queue_scheduler.hpp>
#include <hpx/runtime/threads/policies/callback_notifier.hpp>

template HPX_EXPORT class hpx::threads::threadmanager_impl<
    hpx::threads::policies::global_queue_scheduler, 
    hpx::threads::policies::callback_notifier>;

template HPX_EXPORT class hpx::threads::threadmanager_impl<
    hpx::threads::policies::local_queue_scheduler, 
    hpx::threads::policies::callback_notifier>;

template HPX_EXPORT class hpx::threads::threadmanager_impl<
    hpx::threads::policies::local_priority_queue_scheduler, 
    hpx::threads::policies::callback_notifier>;

template HPX_EXPORT class hpx::threads::threadmanager_impl<
    hpx::threads::policies::abp_queue_scheduler, 
    hpx::threads::policies::callback_notifier>;

