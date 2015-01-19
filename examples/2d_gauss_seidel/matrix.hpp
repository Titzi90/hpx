// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Intervace of a matrix implemantation

#include "generateProblem.hpp"

class SparseMatrix
{
public:
    SparseMatrix(std::size_t rows);

    double getValue (std::size_t const row, std::size_t const index) const;
    std::vector<double> const & getValue (std::size_t const row) const;
    std::vector<double>& getValue (std::size_t const row);
    std::size_t getIndices (std::size_t const row, std::size_t const index) const;
    std::vector<std::size_t> const & getIndices  (std::size_t const row) const;
    std::vector<std::size_t>& getIndices  (std::size_t const row);
    double getDiagVal  (std::size_t const row) const;
    std::size_t getNonZeros (std::size_t const row) const;

    friend SparseMatrix generateProblem(std::size_t const,
                                        std::size_t const,
                                        hpx::vector&, hpx::vector&);

private:

}
