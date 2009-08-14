//  Copyright (c) 2009-2010 Dylan Stark
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_STUBS_DISTRIBUTED_LIST_AUG_14_2009_1131AM)
#define HPX_COMPONENTS_STUBS_DISTRIBUTED_LIST_AUG_14_2009_1131AM

#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/runtime/components/stubs/runtime_support.hpp>
#include <hpx/runtime/components/component_type.hpp>
#include <hpx/runtime/components/stubs/stub_base.hpp>
#include <hpx/lcos/eager_future.hpp>

#include "../server/distributed_list.hpp"

namespace hpx { namespace components { namespace stubs
{    
    ///////////////////////////////////////////////////////////////////////////
    /// The \a stubs#distributed_list class is the client side representation of
    /// all \a server#distributed_list components
    template <typename List>
    struct distributed_list
      : components::stubs::stub_base<server::distributed_list<List> >
    {
        ///////////////////////////////////////////////////////////////////////
        // exposed functionality of this component

        /// Initialize the server#distributed_list instance
        /// with the given \a gid
        static naming::id_type append(naming::id_type gid, naming::id_type locale)
        {
            typedef typename server::distributed_list<List>::append_action
                action_type;
            return lcos::eager_future<action_type>(gid, locale).get();
        }
    };

}}}

#endif
