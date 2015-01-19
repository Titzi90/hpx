// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// intervace of an interative solver for a system of equations
// given by A * u = rhs

#include "matrix.hpp"
#include <hpx/components/vector/vector.hpp>

steper(SparseMatrix const & matrix, hpx::vector<double> const & rhs,
        hpx::vector<double>& u);

solve(SparseMatrix const & matrix,
      hpx::vector<double> const & rhs,
      hpx::vector<double>& u,
      std::size_t const maxTimeSteps,
      double const epsilon,
      std::size_t epsilonCheckrate)
    //TODO output
{
    for (size_t timeStep=0; timeStep<maxTimeSteps; ++timeStep){
        // overlaping loop without checking for error bound
        for (size_t timeToEpsilon=0; timeToEpsilon<epsilonCheckrate;
                ++timeToEpsilon, ++timeStep){
            // solve solution for next time step
            steper(matrix, rhs, u);
        }
        // stop loop if error correction is smaler then the given error bound
        if (epsilon >= errorCorrection) break;
    }

}
