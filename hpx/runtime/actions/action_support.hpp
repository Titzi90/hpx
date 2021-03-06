//  Copyright (c) 2007-2014 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//  Copyright (c)      2011 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file action_support.hpp

#if !defined(HPX_RUNTIME_ACTIONS_ACTION_SUPPORT_NOV_14_2008_0711PM)
#define HPX_RUNTIME_ACTIONS_ACTION_SUPPORT_NOV_14_2008_0711PM

#include <hpx/hpx_fwd.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/runtime/actions/continuation.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/runtime/threads/thread_init_data.hpp>
#include <hpx/util/move.hpp>
#include <hpx/util/polymorphic_factory.hpp>
#include <hpx/util/tuple.hpp>
#include <hpx/util/detail/count_num_args.hpp>
#include <hpx/util/detail/serialization_registration.hpp>

#include <boost/cstdint.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <hpx/config/warnings_prefix.hpp>

/// \cond NOINTERNAL
namespace hpx { namespace actions { namespace detail
{
    struct action_serialization_data
    {
        boost::uint64_t parent_id_;
        boost::uint64_t parent_phase_;
        boost::uint32_t parent_locality_;
        boost::uint16_t priority_;
        boost::uint16_t stacksize_;
    };
}}}

namespace boost { namespace serialization
{
    template <>
    struct is_bitwise_serializable<
            hpx::actions::detail::action_serialization_data>
       : boost::mpl::true_
    {};
}}

/// \endcond

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace actions
{
    /// \cond NOINTERNAL

    struct base_action;

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename Action>
        char const* get_action_name()
#ifdef HPX_DISABLE_AUTOMATIC_SERIALIZATION_REGISTRATION
        ;
#else
        {
            /// If you encounter this assert while compiling code, that means that
            /// you have a HPX_REGISTER_ACTION macro somewhere in a source file,
            /// but the header in which the action is defined misses a
            /// HPX_REGISTER_ACTION_DECLARATION
            BOOST_MPL_ASSERT_MSG(
                traits::needs_automatic_registration<Action>::value
              , HPX_REGISTER_ACTION_DECLARATION_MISSING
              , (Action)
            );
            return util::type_id<Action>::typeid_.type_id();
        }
#endif

        ///////////////////////////////////////////////////////////////////////
        // If an action returns a future, we need to do special things
        template <typename Result>
        struct remote_action_result
        {
            typedef Result type;
        };

        template <>
        struct remote_action_result<void>
        {
            typedef util::unused_type type;
        };

        template <typename Result>
        struct remote_action_result<lcos::future<Result> >
        {
            typedef Result type;
        };

        template <>
        struct remote_action_result<lcos::future<void> >
        {
            typedef hpx::util::unused_type type;
        };

        template <typename Result>
        struct remote_action_result<lcos::shared_future<Result> >
        {
            typedef Result type;
        };

        template <>
        struct remote_action_result<lcos::shared_future<void> >
        {
            typedef hpx::util::unused_type type;
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename Action>
        struct action_registration
        {
            static boost::shared_ptr<base_action> create()
            {
                return boost::shared_ptr<base_action>(new Action());
            }

            action_registration()
            {
                util::polymorphic_factory<base_action>::get_instance().
                    add_factory_function(
                        detail::get_action_name<typename Action::derived_type>()
                      , &action_registration::create
                    );
            }
        };

        template <typename Action, typename Enable =
            typename traits::needs_automatic_registration<Action>::type>
        struct automatic_action_registration
        {
            automatic_action_registration()
            {
                action_registration<Action> auto_register;
            }

            automatic_action_registration & register_action()
            {
                return *this;
            }
        };

        template <typename Action>
        struct automatic_action_registration<Action, boost::mpl::false_>
        {
            automatic_action_registration()
            {
            }

            automatic_action_registration & register_action()
            {
                return *this;
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    /// The \a base_action class is an abstract class used as the base class
    /// for all action types. It's main purpose is to allow polymorphic
    /// serialization of action instances through a shared_ptr.
    struct base_action
    {
        /// The type of an action defines whether this action will be executed
        /// directly or by a HPX-threads
        enum action_type
        {
            plain_action = 0,   ///< The action will be executed by a newly created thread
            direct_action = 1   ///< The action needs to be executed directly
        };

        /// Destructor
        virtual ~base_action() {}

        /// The function \a get_component_type returns the \a component_type
        /// of the component this action belongs to.
        virtual int get_component_type() const = 0;

        /// The function \a get_action_name returns the name of this action
        /// (mainly used for debugging and logging purposes).
        virtual char const* get_action_name() const = 0;

        /// The function \a get_action_type returns whether this action needs
        /// to be executed in a new thread or directly.
        virtual action_type get_action_type() const = 0;

        /// The \a get_thread_function constructs a proper thread function for
        /// a \a thread, encapsulating the functionality and the arguments
        /// of the action it is called for.
        ///
        /// \param lva    [in] This is the local virtual address of the
        ///               component the action has to be invoked on.
        ///
        /// \returns      This function returns a proper thread function usable
        ///               for a \a thread.
        ///
        /// \note This \a get_thread_function will be invoked to retrieve the
        ///       thread function for an action which has to be invoked without
        ///       continuations.
        virtual threads::thread_function_type
            get_thread_function(naming::address::address_type lva) = 0;

        /// The \a get_thread_function constructs a proper thread function for
        /// a \a thread, encapsulating the functionality, the arguments, and
        /// the continuations of the action it is called for.
        ///
        /// \param cont   [in] This is the list of continuations to be
        ///               triggered after the execution of the action
        /// \param lva    [in] This is the local virtual address of the
        ///               component the action has to be invoked on.
        ///
        /// \returns      This function returns a proper thread function usable
        ///               for a \a thread.
        ///
        /// \note This \a get_thread_function will be invoked to retrieve the
        ///       thread function for an action which has to be invoked with
        ///       continuations.
        virtual threads::thread_function_type
            get_thread_function(continuation_type& cont,
                naming::address::address_type lva) = 0;

        /// return the id of the locality of the parent thread
        virtual boost::uint32_t get_parent_locality_id() const = 0;

        /// Return the thread id of the parent thread
        virtual threads::thread_id_repr_type get_parent_thread_id() const = 0;

        /// Return the thread phase of the parent thread
        virtual boost::uint64_t get_parent_thread_phase() const = 0;

        /// Return the thread priority this action has to be executed with
        virtual threads::thread_priority get_thread_priority() const = 0;

        /// Return the thread stacksize this action has to be executed with
        virtual threads::thread_stacksize get_thread_stacksize() const = 0;

        /// Return the size of action arguments in bytes
        virtual std::size_t get_type_size() const = 0;

        /// Return whether the embedded action may require id-splitting
        virtual bool may_require_id_splitting() const = 0;

        /// Return whether the embedded action is part of termination detection
        virtual bool does_termination_detection() const = 0;

        virtual void load(hpx::util::portable_binary_iarchive & ar) = 0;
        virtual void save(hpx::util::portable_binary_oarchive & ar) const = 0;

        /// Wait for embedded futures to become ready
        virtual void wait_for_futures() = 0;

//         /// Return all data needed for thread initialization
//         virtual threads::thread_init_data&
//         get_thread_init_data(naming::id_type const& target,
//             naming::address::address_type lva, threads::thread_init_data& data) = 0;
//
//         virtual threads::thread_init_data&
//         get_thread_init_data(continuation_type& cont,
//             naming::id_type const& target, naming::address::address_type lva,
//             threads::thread_init_data& data) = 0;

        /// Return all data needed for thread initialization
        virtual void schedule_thread(naming::id_type const& target,
            naming::address::address_type lva,
            threads::thread_state_enum initial_state) = 0;

        virtual void schedule_thread(continuation_type& cont,
            naming::id_type const& target, naming::address::address_type lva,
            threads::thread_state_enum initial_state) = 0;

        /// Return a pointer to the filter to be used while serializing an
        /// instance of this action type.
        virtual util::binary_filter* get_serialization_filter(
            parcelset::parcel const& p) const = 0;

        /// Return a pointer to the message handler to be used for this action.
        virtual parcelset::policies::message_handler* get_message_handler(
            parcelset::parcelhandler* ph, parcelset::locality const& loc,
            parcelset::parcel const& p) const = 0;

#if defined(HPX_HAVE_SECURITY)
        /// Return the set of capabilities required to invoke this action
        virtual components::security::capability get_required_capabilities(
            naming::address::address_type lva) const = 0;
#endif
    };

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        // Figure out what priority the action has to be be associated with
        // A dynamically specified default priority results in using the static
        // Priority.
        template <threads::thread_priority Priority>
        struct thread_priority
        {
            static threads::thread_priority
            call(threads::thread_priority priority)
            {
                if (priority == threads::thread_priority_default)
                    return Priority;
                return priority;
            }
        };

        // If the static Priority is default, a dynamically specified default
        // priority results in using the normal priority.
        template <>
        struct thread_priority<threads::thread_priority_default>
        {
            static threads::thread_priority
            call(threads::thread_priority priority)
            {
                if (priority == threads::thread_priority_default)
                    return threads::thread_priority_normal;
                return priority;
            }
        };

        ///////////////////////////////////////////////////////////////////////
        // Figure out what stacksize the action has to be be associated with
        // A dynamically specified default stacksize results in using the static
        // Stacksize.
        template <threads::thread_stacksize Stacksize>
        struct thread_stacksize
        {
            static threads::thread_stacksize
            call(threads::thread_stacksize stacksize)
            {
                if (stacksize == threads::thread_stacksize_default)
                    return Stacksize;
                return stacksize;
            }
        };

        // If the static Stacksize is default, a dynamically specified default
        // stacksize results in using the normal stacksize.
        template <>
        struct thread_stacksize<threads::thread_stacksize_default>
        {
            static threads::thread_stacksize
            call(threads::thread_stacksize stacksize)
            {
                if (stacksize == threads::thread_stacksize_default)
                    return threads::thread_stacksize_minimal;
                return stacksize;
            }
        };
    }

    template <typename Action>
    struct init_registration;

    /// \endcond
}}

#endif
