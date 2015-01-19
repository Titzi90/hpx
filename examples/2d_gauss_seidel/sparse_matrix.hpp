// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// simple implemenation of a sparse matrix

#include "matrix.hpp"

#include <vector>

class SparseMatrix
{
public:
    SparseMatrix(std::size_t const rows)
    :values(),diagonalValue(),indices(),nonZeros(rows,0)
    {
        values.reserve(rows);
        diagonalValue.reserve(rows);
        indices.reserve(rows);
        nonZeros.reserve(rows);
    }

    double getValue    (std::size_t const row, std::size_t const index) const
    {
        return values.at(row).at(index);
    }

    std::vector<double> const & getValue    (std::size_t const row) const
    {
        return values.at(row);
    }

    std::vector<double> & getValue    (std::size_t const row)
    {
        return values.at(row);
    }

    std::size_t getIndices  (std::size_t const row, std::size_t const index) const
    {
        return indices.at(row).at(index);
    }

    std::vector<std::size_t> const & getIndices  (std::size_t const row) const
    {
        return indices.at(row);
    }

    std::vector<std::size_t> & getIndices  (std::size_t const row)
    {
        return indices.at(row);
    }

    double getDiagVal  (std::size_t const row) const
    {
        return diagonalValue.at(row);
    }

    std::size_t getNonZeros (std::size_t const row) const
    {
        return nonZeros.at(row);
    }

    friend SparseMatrix generateProblem(std::size_t const,
                                        std::size_t const,
                                        hpx::vector&, hpx::vector&);

private:
    std::vector< std::vector<double> >      values;
    std::vector< double & >                 diagonalValue;
    std::vector< std::vector<std::size_t> > indices;
    std::vector< std::size_t >              nonZeros;

}
