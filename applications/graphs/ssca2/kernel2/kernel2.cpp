//  Copyright (c) 2009-2010 Dylan Stark
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <applications/graphs/ssca2/kernel2/server/kernel2.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

///////////////////////////////////////////////////////////////////////////////
HPX_REGISTER_COMPONENT_MODULE();
