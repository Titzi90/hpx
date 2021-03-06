// Copyright (c) 2007-2013 Hartmut Kaiser
// Copyright (c) 2012-2013 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This file has been automatically generated using the Boost.Wave tool.
// Do not edit manually.


namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
           
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
           
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                   
                )
            );
        }
        template <
            typename Action
          , typename Futures
           
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
           
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                   
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
           
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
           
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                   
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
           
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
           
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                   
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
           
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
           
          , std::size_t)
        {
            hpx::apply(
                act
              , id
               
            );
        }
        template <
            typename Action
           
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
           
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
               
              , global_idx
            );
        }
        
        template <
            typename Action
           
        >
        
        void
        broadcast_impl0(
            Action const & act
          , std::vector<hpx::id_type> const & ids
           
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                   
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                           
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
           
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl0(
            Action const & act
          , std::vector<hpx::id_type> const & ids
           
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                   
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                           
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
           
        >
        void
        broadcast_apply_impl0(
            Action const & act
          , std::vector<hpx::id_type> const & ids
           
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                   
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                       
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
           
          , typename IsVoid
        >
        struct broadcast_invoker0
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
               
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl0(
                        act
                      , ids
                       
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 0>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker0<
                        Action
                      
                        
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
           
        >
        struct broadcast_apply_invoker0
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
               
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl0(
                        act
                      , ids
                       
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 0>
        {
            typedef broadcast_apply_invoker0<
                        Action
                      
                        
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
       
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
       )
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
               
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
       
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
       )
    {
        return broadcast<Derived>(
                ids
               
            );
    }
    
    template <
        typename Action
       
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
       )
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
               
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
       
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
       )
    {
        broadcast_apply<Derived>(
            ids
           
        );
    }
    
    template <
        typename Action
       
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
       )
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
               
            );
    }
    template <
        typename Component, typename Signature, typename Derived
       
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
       )
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
               
            );
    }
    
    template <
        typename Action
       
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
       )
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
           
        );
    }
    template <
        typename Component, typename Signature, typename Derived
       
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
       )
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
           
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0
            );
        }
        template <
            typename Action
          , typename A0
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0
        >
        
        void
        broadcast_impl1(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl1(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0
        >
        void
        broadcast_apply_impl1(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0
          , typename IsVoid
        >
        struct broadcast_invoker1
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl1(
                        act
                      , ids
                      , a0
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 1>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker1<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0
        >
        struct broadcast_apply_invoker1
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl1(
                        act
                      , ids
                      , a0
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 1>
        {
            typedef broadcast_apply_invoker1<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        return broadcast<Derived>(
                ids
              , a0
            );
    }
    
    template <
        typename Action
      , typename A0
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        broadcast_apply<Derived>(
            ids
          , a0
        );
    }
    
    template <
        typename Action
      , typename A0
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0
            );
    }
    
    template <
        typename Action
      , typename A0
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1
            );
        }
        template <
            typename Action
          , typename A0 , typename A1
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1
        >
        
        void
        broadcast_impl2(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl2(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1
        >
        void
        broadcast_apply_impl2(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1
          , typename IsVoid
        >
        struct broadcast_invoker2
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl2(
                        act
                      , ids
                      , a0 , a1
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 2>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker2<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1
        >
        struct broadcast_apply_invoker2
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl2(
                        act
                      , ids
                      , a0 , a1
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 2>
        {
            typedef broadcast_apply_invoker2<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        
        void
        broadcast_impl3(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl3(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        void
        broadcast_apply_impl3(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
          , typename IsVoid
        >
        struct broadcast_invoker3
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl3(
                        act
                      , ids
                      , a0 , a1 , a2
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 3>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker3<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2
        >
        struct broadcast_apply_invoker3
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl3(
                        act
                      , ids
                      , a0 , a1 , a2
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 3>
        {
            typedef broadcast_apply_invoker3<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        
        void
        broadcast_impl4(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl4(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        void
        broadcast_apply_impl4(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
          , typename IsVoid
        >
        struct broadcast_invoker4
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl4(
                        act
                      , ids
                      , a0 , a1 , a2 , a3
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 4>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker4<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3
        >
        struct broadcast_apply_invoker4
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl4(
                        act
                      , ids
                      , a0 , a1 , a2 , a3
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 4>
        {
            typedef broadcast_apply_invoker4<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        
        void
        broadcast_impl5(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl5(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        void
        broadcast_apply_impl5(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
          , typename IsVoid
        >
        struct broadcast_invoker5
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl5(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 5>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker5<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
        >
        struct broadcast_apply_invoker5
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl5(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 5>
        {
            typedef broadcast_apply_invoker5<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        
        void
        broadcast_impl6(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl6(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        void
        broadcast_apply_impl6(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
          , typename IsVoid
        >
        struct broadcast_invoker6
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl6(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 6>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker6<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
        >
        struct broadcast_apply_invoker6
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl6(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 6>
        {
            typedef broadcast_apply_invoker6<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        
        void
        broadcast_impl7(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl7(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        void
        broadcast_apply_impl7(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
          , typename IsVoid
        >
        struct broadcast_invoker7
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl7(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 7>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker7<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
        >
        struct broadcast_apply_invoker7
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl7(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 7>
        {
            typedef broadcast_apply_invoker7<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        
        void
        broadcast_impl8(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl8(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        void
        broadcast_apply_impl8(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
          , typename IsVoid
        >
        struct broadcast_invoker8
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl8(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 8>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker8<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
        >
        struct broadcast_apply_invoker8
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl8(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 8>
        {
            typedef broadcast_apply_invoker8<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        
        void
        broadcast_impl9(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl9(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        void
        broadcast_apply_impl9(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
          , typename IsVoid
        >
        struct broadcast_invoker9
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl9(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 9>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker9<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
        >
        struct broadcast_apply_invoker9
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl9(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 9>
        {
            typedef broadcast_apply_invoker9<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        
        void
        broadcast_impl10(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl10(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        void
        broadcast_apply_impl10(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
          , typename IsVoid
        >
        struct broadcast_invoker10
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl10(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 10>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker10<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
        >
        struct broadcast_apply_invoker10
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl10(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 10>
        {
            typedef broadcast_apply_invoker10<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        
        void
        broadcast_impl11(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl11(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        void
        broadcast_apply_impl11(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
          , typename IsVoid
        >
        struct broadcast_invoker11
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl11(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 11>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker11<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
        >
        struct broadcast_apply_invoker11
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl11(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 11>
        {
            typedef broadcast_apply_invoker11<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        
        void
        broadcast_impl12(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl12(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        void
        broadcast_apply_impl12(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
          , typename IsVoid
        >
        struct broadcast_invoker12
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl12(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 12>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker12<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
        >
        struct broadcast_apply_invoker12
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl12(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 12>
        {
            typedef broadcast_apply_invoker12<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        
        void
        broadcast_impl13(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl13(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        void
        broadcast_apply_impl13(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
          , typename IsVoid
        >
        struct broadcast_invoker13
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl13(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 13>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker13<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
        >
        struct broadcast_apply_invoker13
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl13(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 13>
        {
            typedef broadcast_apply_invoker13<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        
        void
        broadcast_impl14(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl14(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        void
        broadcast_apply_impl14(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
          , typename IsVoid
        >
        struct broadcast_invoker14
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl14(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 14>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker14<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
        >
        struct broadcast_apply_invoker14
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl14(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 14>
        {
            typedef broadcast_apply_invoker14<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        
        void
        broadcast_impl15(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl15(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        void
        broadcast_apply_impl15(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
          , typename IsVoid
        >
        struct broadcast_invoker15
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl15(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 15>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker15<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
        >
        struct broadcast_apply_invoker15
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl15(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 15>
        {
            typedef broadcast_apply_invoker15<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        
        void
        broadcast_impl16(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl16(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        void
        broadcast_apply_impl16(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
          , typename IsVoid
        >
        struct broadcast_invoker16
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl16(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 16>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker16<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
        >
        struct broadcast_apply_invoker16
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl16(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 16>
        {
            typedef broadcast_apply_invoker16<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        
        void
        broadcast_impl17(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl17(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        void
        broadcast_apply_impl17(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
          , typename IsVoid
        >
        struct broadcast_invoker17
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl17(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 17>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker17<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
        >
        struct broadcast_apply_invoker17
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl17(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 17>
        {
            typedef broadcast_apply_invoker17<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        
        void
        broadcast_impl18(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl18(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        void
        broadcast_apply_impl18(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
          , typename IsVoid
        >
        struct broadcast_invoker18
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl18(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 18>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker18<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
        >
        struct broadcast_apply_invoker18
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl18(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 18>
        {
            typedef broadcast_apply_invoker18<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        
        void
        broadcast_impl19(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl19(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        void
        broadcast_apply_impl19(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
          , typename IsVoid
        >
        struct broadcast_invoker19
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl19(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 19>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker19<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 18 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
        >
        struct broadcast_apply_invoker19
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl19(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 19>
        {
            typedef broadcast_apply_invoker19<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 18 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18
        );
    }
}}
namespace hpx { namespace lcos
{
    namespace detail
    {
        
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke(Action act, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures, hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                  , global_idx
                )
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke(Action act, Futures& futures, Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t)
        {
            futures.push_back(
                hpx::async(
                    act
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                ).then(std::forward<Cont>(cont))
            );
        }
        template <
            typename Action
          , typename Futures
          , typename Cont
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke(broadcast_with_index<Action>, Futures& futures
          , Cont && cont
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx)
        {
            futures.push_back(
                hpx::async(
                    Action()
                  , id
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                  , global_idx
                ).then(std::forward<Cont>(cont))
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke_apply(Action act
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t)
        {
            hpx::apply(
                act
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
            );
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_invoke_apply(broadcast_with_index<Action>
          , hpx::id_type const& id
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx)
        {
            hpx::apply(
                Action()
              , id
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
              , global_idx
            );
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        
        void
        broadcast_impl20(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx
          , boost::mpl::true_
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<void> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                          , global_idx + applied
                          , boost::integral_constant<bool, true>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            
            hpx::when_all(broadcast_futures).then(&return_void).get();
        }
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        
        typename broadcast_result<Action>::type
        broadcast_impl20(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx
          , boost::mpl::false_
        )
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef
                typename broadcast_result<Action>::type
                result_type;
            
            if(ids.empty()) return result_type();
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
            std::vector<hpx::future<result_type> > broadcast_futures;
            broadcast_futures.reserve(local_size + (ids.size()/fanout) + 1);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke(
                    act
                  , broadcast_futures
                  , &wrap_into_vector<action_result>
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_action<
                        Action
                    >::type
                    broadcast_impl_action;
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    broadcast_futures.push_back(
                        hpx::async_colocated<broadcast_impl_action>(
                            id
                          , act
                          , std::move(ids_next)
                          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                          , global_idx + applied
                          , boost::integral_constant<bool, false>::type()
                        )
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
            return hpx::when_all(broadcast_futures).
                then(&return_result_type<action_result>).get();
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        void
        broadcast_apply_impl20(
            Action const & act
          , std::vector<hpx::id_type> const & ids
          , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
          , std::size_t global_idx
        )
        {
            if(ids.empty()) return;
            std::size_t const local_fanout = HPX_BROADCAST_FANOUT;
            std::size_t local_size = (std::min)(ids.size(), local_fanout);
            for(std::size_t i = 0; i != local_size; ++i)
            {
                broadcast_invoke_apply(
                    act
                  , ids[i]
                  , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                  , global_idx + i
                );
            }
            if(ids.size() > local_fanout)
            {
                std::size_t applied = local_fanout;
                std::vector<hpx::id_type>::const_iterator it =
                    ids.begin() + local_fanout;
                typedef
                    typename detail::make_broadcast_apply_action<
                        Action
                    >::type
                    broadcast_impl_action;
                std::size_t fanout = util::calculate_fanout(ids.size(), local_fanout);
                while(it != ids.end())
                {
                    HPX_ASSERT(ids.size() >= applied);
                    std::size_t next_fan = (std::min)(fanout, ids.size() - applied);
                    std::vector<hpx::id_type> ids_next(it, it + next_fan);
                    hpx::id_type id(ids_next[0]);
                    hpx::apply_colocated<broadcast_impl_action>(
                        id
                      , act
                      , std::move(ids_next)
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                      , global_idx + applied
                    );
                    applied += next_fan;
                    it += next_fan;
                }
            }
        }
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
          , typename IsVoid
        >
        struct broadcast_invoker20
        {
            
            static typename broadcast_result<Action>::type
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
              , std::size_t global_idx
              , IsVoid
            )
            {
                return
                    broadcast_impl20(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                      , global_idx
                      , IsVoid()
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_action_impl<Action, 20>
        {
            typedef
                typename broadcast_result<Action>::action_result
                action_result;
            typedef broadcast_invoker20<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 18 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 19 >::type
                      , typename boost::is_same<void, action_result>::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
        
        template <
            typename Action
          , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
        >
        struct broadcast_apply_invoker20
        {
            static void
            call(
                Action const & act
              , std::vector<hpx::id_type> const & ids
              , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19
              , std::size_t global_idx
            )
            {
                return
                    broadcast_apply_impl20(
                        act
                      , ids
                      , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
                      , global_idx
                    );
            }
        };
        template <typename Action>
        struct make_broadcast_apply_action_impl<Action, 20>
        {
            typedef broadcast_apply_invoker20<
                        Action
                      ,
                        typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 0 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 1 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 2 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 3 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 4 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 5 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 6 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 7 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 8 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 9 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 10 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 11 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 12 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 13 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 14 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 15 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 16 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 17 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 18 >::type , typename boost::fusion::result_of::value_at_c< typename Action::arguments_type, 19 >::type
                    >
                    broadcast_invoker_type;
            typedef
                typename HPX_MAKE_ACTION(broadcast_invoker_type::call)::type
                type;
        };
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        typedef
            typename detail::make_broadcast_action<Action>::type
            broadcast_impl_action;
        typedef
            typename detail::broadcast_result<Action>::action_result
            action_result;
        return
            hpx::async_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
              , 0
              , typename boost::is_same<void, action_result>::type()
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        return broadcast<Derived>(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    void
    broadcast_apply(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        typedef
            typename detail::make_broadcast_apply_action<Action>::type
            broadcast_impl_action;
        hpx::apply_colocated<broadcast_impl_action>(
                ids[0]
              , Action()
              , ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
              , 0
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    void
    broadcast_apply(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        broadcast_apply<Derived>(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
        );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    hpx::future<
        typename detail::broadcast_result<Action>::type
    >
    broadcast_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        return broadcast<detail::broadcast_with_index<Action> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
            );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    hpx::future<
        typename detail::broadcast_result<Derived>::type
    >
    broadcast_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        return broadcast<detail::broadcast_with_index<Derived> >(
                ids
              , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
            );
    }
    
    template <
        typename Action
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    void
    broadcast_apply_with_index(
        std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        broadcast_apply<detail::broadcast_with_index<Action> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
        );
    }
    template <
        typename Component, typename Signature, typename Derived
      , typename A0 , typename A1 , typename A2 , typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 , typename A10 , typename A11 , typename A12 , typename A13 , typename A14 , typename A15 , typename A16 , typename A17 , typename A18 , typename A19
    >
    void
    broadcast_apply_with_index(
        hpx::actions::basic_action<Component, Signature, Derived> 
      , std::vector<hpx::id_type> const & ids
      , A0 const & a0 , A1 const & a1 , A2 const & a2 , A3 const & a3 , A4 const & a4 , A5 const & a5 , A6 const & a6 , A7 const & a7 , A8 const & a8 , A9 const & a9 , A10 const & a10 , A11 const & a11 , A12 const & a12 , A13 const & a13 , A14 const & a14 , A15 const & a15 , A16 const & a16 , A17 const & a17 , A18 const & a18 , A19 const & a19)
    {
        broadcast_apply<detail::broadcast_with_index<Derived> >(
            ids
          , a0 , a1 , a2 , a3 , a4 , a5 , a6 , a7 , a8 , a9 , a10 , a11 , a12 , a13 , a14 , a15 , a16 , a17 , a18 , a19
        );
    }
}}
