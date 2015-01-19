// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// implemenation of a Gauss Seidel solver for a system of equations
// given by A * u = rhs

#include "solver.hpp"

#include "matrix.hpp"
#include <hpx/components/vector/vector.hpp>


inline double operator(std::vector<doubles> const & matrixRow,
                double const & matrixDiagonal,
                std::vector<std::size_t> const & matrixIndices,
                std::size_t const nonZeros,
                std::vector<double> const & u)
{
    double sum = rhs;

    for (std::size_t j=0; j<nonZeros; ++j){
        sum -= matrixRow[j]*u[ matrixIndices[j] ];
    }
    sum += matrixDiagonal; // equalize by removing diagone element again

    return sum/matrixDiagonal;
}


steper(SparseMatrix const & matrix, hpx::vector<double> const & rhs,
        hpx::vector<double>& u)
{
    //chunc iterator
        // list of chunks from GID
    

}
