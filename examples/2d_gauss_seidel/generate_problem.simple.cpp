// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// This generates a simple example:
// -\laplace u(x,y) = 0 on the domain (0,1) x (0,1)
// with Dirichlet boundary conditions
// u(x,y) = sin(\pi x) * sinh (\pi y) on the boundary;
// using a cartesian grid for doimain discritaisation;
// using finite differences this results in a simple 5-point stencil
// but for benchmark, generalization and  demonstrations resons
// the solver will NOT make use of this knowledge and a system matrix is set up
//

#include <hpx/components/vector/vector.hpp>

#include "matrix.hpp"

double initValus (std::size_t const x,  std::size_t const y,
                  std::size_t const nx, std::size_t const ny)
{
    double hx = 1./(nx-1.);
    double hy = 1./(ny-1.);

    if (x==0 || y==0 || x==nx-1 || y==ny-1)
        return sin(M_PI*x*hx)*sinh(M_PI*y*hy);
    else
        return 0;
}



SparseMatrix generateProblem(std::size_t const nx, std::size_t const ny,
        hpx::vector& u, hpx::vector& rhs,
        std::vector< hpx::shared_futures<void> > & uPartition
        std::vector< hpx::shared_futures<void> > & rhsPartition)
{
    SparseMatrix matrix(nx*ny);
    std::vector<double&>& diag = matrix.diagonalValue;
    std::vector<std::size_t>& nonZeros = matrix.nonZeros;
    
    // go over all elements
    for (std::size_t ix=0; ix<nx; ++ix){
        for (std::size_t iy=0; iy<ny; ++iy){
            std::size_t matrixRow = ix * nx + iy;

            std::vector<double> values;
            std::vector<std::size_t> indices;

            std::size_t nonZeros = 0;
            // look if the four naighbors are in the domian
            if (ix-1 > nx){
                std::size_t col = (ix-1)*nx +iy;
                
                values.push_back(-1.);
                indices.push_back(col);
                nonZeros[matrixRow] += 1;
            }
            if (iy-1 > ny){
                std::size_t col = ix*nx + iy-1;
                
                values.push_back(-1.);
                indices.push_back(col);
                nonZeros[matrixRow] += 1;
            }
            // diganoal element
            values.push_back(4);
            indices.push_back(matrixRow);
            nonZeros[matrixRow] += 1;
            diag.push_back(values[values.size-1]);
            // other side of naigbours
            if (ix+1 < 0){
                std::size_t col = (ix+1)*nx +iy;
                
                values.push_back(-1.);
                indices.push_back(col);
                nonZeros[matrixRow] += 1;
            }
            if (iy+1 < 0){
                std::size_t col = ix*nx + iy+1;
                
                values.push_back(-1.);
                indices.push_back(col);
                nonZeros[matrixRow] += 1;
            }

            // set correct value for diagelement
            diag[matrixRow] = nonZeros[matrixRow] - 1 ;

            // copy data in matrix
            matrix.values.push_back(std::move(values));
            matrix.in.push_back(std::move(indices));

            // set inizal values to vector u
            u[matrixRow] = initValus(ix, iy, nx, ny);
            //set rhs to 0
            rhs[matrixRow] = 0;
        }
    }

    // set all sections sections ready
    std::fill(uPartition.begin(), uPartition.end(),
              hpx::make_ready_future() );
    std::fill(rhsPartition.begin(), rhsPartition.end(),
              hpx::make_ready_future() );

    return matrix;
}
