// Copyright (c) 2014 Christopher Bross
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// TODO

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/components/vector/vector.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "matrix.hpp"
#include "generate_problem.hpp"

struct vector_wrapper
{
    vector_wrapper (std::size_t size, std::size_t numOfPatritions):
        values (size_, hpx::block(numOfPartitions_, hpx::find_all_localities) ),
        partitions (numOfPartitions_, ),
        size(size_),
        numOfPatritions(numOfPatritions_)
    {}


    // get a vector of values from different partitions
    // and make sure this partitions are ready
    // index vector 'pos' contains global ids
    hpx::future< std::vector<double> >
    getValues(std::vector<std::size_t> pos)
    {
        using hpx::lcos::local::dataflow

        //CODE FROM HPX VECTOR + Partition check
        // check if position vector is empty
        // the follwoing code needs at least one element.
        if (0 == pos_vec.size()){
            return make_ready_future( std::vector<T>() );
        }

        // current partition index of the block
        size_type part_cur = values.get_partition( pos_vec[0] );
        // iterator to the begin of current block
        std::vector<size_type>::const_iterator part_begin = pos_vec.begin();
        // vector holding futures of the values for all blocks
        std::vector< future< std::vector<T> > > part_values_future;
        for (std::vector<size_type>::const_iterator it = pos_vec.begin();
                it != pos_vec.end(); ++it){
            // get the partition of the current position
            size_type part = values.get_partition(*it);

            // if the partition of the current position is the same
            // as the rest of the current block go to next position
            if ( part == part_cur) continue;
            // if the partition of the current position is NOT the same
            // as the positions before the block ends here
            else
            {
                // this is the end of a block containing indexes ('pos')
                // of the same partition ('part').
                // get asyncorn values for this block
                // if partition is ready
                part_values_future.push_back( dataflow(
                            // Lamda that calls the get_value function with
                            // the right arguments
                            [part_cur, &values, &part_begin, &it]
                            (hpx::shared_future<void>)
                            {
                                values.get_values( part_cur,
                                        values.get_local_indicesi(
                                        std::vector<std::size_t>(part_begin, it) )
                                                 )
                            },
                            partitions[part_cur]
                            ) );

                // reset block varibles to start a new one from here
                part_cur = part;
                part_begin = it;
            }
        }

        // the end of the vector is also an end of a block
        // get asyncorn values for this block
        part_values_future.push_back( dataflow(
                // Lamda that calls the get_value function with
                // the right arguments
                [part_cur, &values, &part_begin]
                (hpx::shared_future<void>)
                {
                    values.get_values( part_cur,
                            values.get_local_indicesi(
                            std::vector<std::size_t>(part_begin, pos_vec.end()) )
                                        )
                },
                partitions[part_cur]
                ) );

        // This helper function unwraps the vectors from each partition
        // and merge them to one vector
        auto merge_func =
                [&pos_vec]
                (std::vector< future< std::vector<T> > > part_values_f)
                -> std::vector<T>y)
                {
                    std::vector<T> return_values;
                    return_values.reserve( pos_vec.size() );

                    for (future< std::vector<T> >& part_f : part_values_f){
                        std::vector<T> part_values = part_f.get();
                        std::move( part_values.begin(), part_values.end(),
                                   std::back_inserter(return_values) );
                    }
                    return return_values;
                };

        // when all values are here merge them to one vector
        // and return a future to this vector
        using lcos::local::dataflow;
        return dataflow(launch::async, merge_func,
                        std::move( part_values_future)
                       );
    }

    // set values to local indices given by pos to the 
    future<void>
    setValues(std::size_t const part, std::vector<std::size_t>const& pos,
              std::vector<double> const& cal)
    {
        using hpx::lcos::local::dataflow

        return dataflow(set_values)
    }

    hpx::vector<double> values;
    std::vector< hpx::shared_future<void> > partitions;
    std::size_t size;
    std__size_t numOfPatritions;
}


int hpx::main(boost::program_options::variables_map& vm)
{
    // get command line arguments
    std::size_t nx = vm["nx"].as<std::size_t>();    // Nuber of points in x dir.
    std::size_t ny = vm["ny"].as<std::size_t>();    // Nuber of points in y dir.
    std::size_t npx = vm["npx"].as<std::size_t>();  // Number of partition in x dir.
    std::size_t npy = vm["npy"].as<std::size_t>();  // Number of partition in y dir.
    std::size_t nt = vm["nt"].as<std::size_t>();    // Nuber of time steps
    double epsilon = vm["e"].as<double>();          // Error bound
    std::size_t epsilonCheckrate =vm["nte"].as<std::size_t>();
    bool verbose = vm.count("v");                   // Verbose mode
    std::size_t deltaOut = vm["o"].as<std::size_t>(); // Output frequenc
    std::string outputFile = vm["out-file"].as<std::string>(); // Prefix for output file

    //vector for right hand site(rhs) and solution (u)
    vector_wrapper u   (nx*ny, npx*npy);
    vector_wrapper rhs (nx*ny, npx*npy);

    // generate Problem
    SparseMatrix matrix = generateProblem(nx, ny,
                                          u.values, rhs.values,
                                          u.partitions, rhs.partitions);




}

int main(int argc, const char *argv[])
{
    using namespace boost::program_options;

    options_description cmdline;
    cmdlina.add_options()
        ("nx", value<std::size_t>()->default_value(1024),
         "Global number of points in x dimension")
        ("ny", value<std::size_t>()->default_value(1024),
         "Global number of points in y dimension")
        ("npx", value<std::size_t>()->default_value(64),
         "Number of partitions in x dimension")
        ("npy", value<std::size_t>()->default_value(64),
         "Number of partitions in y dimension")
        ("nt", value<std::size_t>()->default_value(std::size_t(-1)),
         "Number of time steps")
        ("e", value<double>()->default_value(1.e-9),
         "Error bound")
        ("nte", value<std::size_t>()-default_value(100),
         "how offent to check for epsilon")
        ("v",
         "verbose mode")
        ("o", value<std:size_t>()->default_value(std::site_t(-1)),
         "Output intervall, '-1' just print finale solution, '0' no output,
         'n' print output every n iteration")
        ("output-file", value<std::string>()->default_value("output"),
         "prefix for output file")
    ;

    //run HPX_main on all localities
    std::vector<std::string> cfg;
    cfg.push_back("hpx.run_hpx_main!=1");

    return hpx::init(cmdline, argc, argv, cfg);

    return 0;
}
