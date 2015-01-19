// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// TODO

#include <hpx/components/vector/vector.hpp>

#include "matrix.hpp"

SparseMatrix generateProblem(std::size_t const nx, std::size_t const ny,
                             hpx::vector& u, hpx::vector& rhs,
                             std::vector< hpx::shared_futures<void> > uPartition,
                             std::vector< hpx::shared_futures<void> > rhsPartition);
