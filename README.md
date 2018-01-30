hike
====

hike is a small C++11 [mathematical optimization](https://en.wikipedia.org/wiki/Mathematical_optimization) library.  

It currently only provides a [variable neighborhood search (VNS)](https://en.wikipedia.org/wiki/Variable_neighborhood_search) with first improvement (first descent) and best improvement (highest descent) local search.

## Features

- Header-only library.
- Solutions can be of any type and size.
- Loss functions can return any type.
- Calculated losses can be cached to speedup the optimization process.
- Best improvement local search can be parallelized across all CPU threads.
- Optimization process can be debugged through callbacks.
- Low overhead, no heap usage (besides loss caching and parallel local search).
- Without dependencies (besided [catch](https://github.com/catchorg/Catch2) for testing).
- Doxygen documentation provided for API reference.
- Licensed under [zlib license](LICENSE.txt).

## Tested compilers

- GCC 5.3.
- MSVC 15.0 (Visual Studio 2017).

## Usage

Although hike is a header-only library, a CMakeLists.txt file is provided to facilitate being added as a dependency.  

Doxygen documentation can be generated with `doxygen Doxyfile`.

## Example

In this [catch](https://github.com/catchorg/Catch2) test, a 3D integer vector is optimized using VNS with first improvement local search:

```C++
#include <array>
#include <catch.hpp>
#include "hikefilocalsearch.h"
#include "hikevns.h"

TEST_CASE("VNS example")
{
    // Solution is a 3D integer vector. It can be of any type and size:
    using Solution = std::array<int, 3>;

    // Loss function return an integer scalar. It can be of any type:
    struct LossFunction
    {
        Solution targetSolution;

        int operator()(const Solution& solution) const
        {
            int loss = 0;

            for(std::size_t i = 0; i < solution.size(); ++i)
            {
                loss += std::abs(solution[i] - targetSolution[i]);
            }

            return loss;
        }
    };

    // Loss function returns the Manhattan distance between the target solution and the given one:
    Solution targetSolution{{ 2, 5, -10 }};
    LossFunction lossFunction = { targetSolution };

    // VNS uses first improvement (first descent) local search.
    // Best improvement (highest descent) local search can be used too:
    using LocalSearch = hike::FILocalSearch<Solution, LossFunction>;

    // Candidate solutions are generated adding and subtracting the parameters of this solution to the given one:
    Solution stepSolution{{ 1, 1, 1 }};

    // Declare local search object:
    LocalSearch localSearch(lossFunction, stepSolution);

    // Declare VNS object with a maximum neighborhood (kmax) of 5:
    hike::VNS<Solution, LocalSearch> vns(localSearch, 5);

    // Optimize a solution:
    Solution solution{{ 15, -7, 22 }};
    bool optimized;
    Solution optimizedSolution = vns.optimize(solution, optimized);

    // The optimized solution should be equals to the target one:
    REQUIRE(optimized);
    REQUIRE(optimizedSolution == targetSolution);
}
```

## Using loss caching

With `hike::CachedLossFunction` class, calculated losses for the same solutions are cached to avoid to recalculate them in following iterations:

```C++
#include <array>
#include <catch.hpp>
#include "hikecachedlossfunction.h"
#include "hikefilocalsearch.h"
#include "hikevns.h"

// Solution is a 3D integer vector. It can be of any type and size:
using Solution = std::array<int, 3>;

// CachedLossFunction uses std::hash to identify a solution, so it must be overloaded for the solution type:
namespace std
{
    template<>
    struct hash<Solution>
    {
        std::size_t operator()(const Solution& solution) const
        {
            std::size_t hash = 0;

            for(int param : solution)
            {
                // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x:
                hash ^= std::hash<int>()(param) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }

            return hash;
        }
    };
}

TEST_CASE("CachedLossFunction example")
{
    // Loss function return an integer scalar. It can be of any type:
    struct LossFunction
    {
        Solution targetSolution;

        int operator()(const Solution& solution) const
        {
            int loss = 0;

            for(std::size_t i = 0; i < solution.size(); ++i)
            {
                loss += std::abs(solution[i] - targetSolution[i]);
            }

            return loss;
        }
    };

    // Loss function returns the Manhattan distance between the target solution and the given one:
    Solution targetSolution{{ 2, 5, -10 }};
    LossFunction lossFunction{targetSolution};

    // Declare cached loss function object using lossFunction as implementation:
    using CachedLossFunction = hike::CachedLossFunction<Solution, LossFunction>;
    CachedLossFunction cachedLossFunction(lossFunction);

    // VNS uses first improvement (first descent) local search.
    // Best improvement (highest descent) local search can be used too:
    using LocalSearch = hike::FILocalSearch<Solution, CachedLossFunction>;

    // Candidate solutions are generated adding and subtracting the parameters of this solution to the given one:
    Solution stepSolution{{ 1, 1, 1 }};

    // Declare local search object:
    LocalSearch localSearch(cachedLossFunction, stepSolution);

    // Declare VNS object with a maximum neighborhood (kmax) of 5:
    hike::VNS<Solution, LocalSearch> vns(localSearch, 5);

    // Optimize a solution:
    Solution solution{{ 15, -7, 22 }};
    bool optimized;
    Solution optimizedSolution = vns.optimize(solution, optimized);

    // The optimized solution should be equals to the target one:
    REQUIRE(optimized);
    REQUIRE(optimizedSolution == targetSolution);
}
```

## Parallel best improvement local search

`hike::ParallelBILocalSearch` class provides multithread best improvement (highest descent) local search.  

To use it, the provided loss function must be thread safe. `hike::TSCachedLossFunction` class provides thread safe loss caching:

```C++
#include <array>
#include <catch.hpp>
#include "hike_ts_cached_loss_function.h"
#include "hike_parallel_bi_local_search.h"
#include "hike_vns.h"

// Solution is a 3D integer vector. It can be of any type and size:
using Solution = std::array<int, 3>;

// CachedLossFunction uses std::hash to identify a solution, so it must be overloaded for the solution type:
namespace std
{
    template<>
    struct hash<Solution>
    {
        std::size_t operator()(const Solution& solution) const
	{
	    std::size_t hash = 0;

            for(int param : solution)
	    {
	        // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x:
		hash ^= std::hash<int>()(param) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	    }

            return hash;
	}
    };
}

TEST_CASE("ParallelBILocalSearch example")
{
    // Loss function returns an integer scalar. It can be of any type, but it must be thread safe:
    struct LossFunction
    {
        Solution targetSolution;

        int operator()(const Solution& solution) const
	{
	    int loss = 0;

            for(std::size_t i = 0; i < solution.size(); ++i)
	    {
	        loss += std::abs(solution[i] - targetSolution[i]);
	    }

            return loss;
	}
    };

    // Loss function returns the Manhattan distance between the target solution and the given one:
    Solution targetSolution{{ 2, 5, -10 }};
    LossFunction lossFunction{targetSolution};

    // Declare thread safe cached loss function object using lossFunction as implementation:
    using CachedLossFunction = hike::TSCachedLossFunction<Solution, LossFunction>;
    CachedLossFunction cachedLossFunction(lossFunction);

    // VNS uses parallel best improvement (highest descent) local search:
    using LocalSearch = hike::ParallelBILocalSearch<Solution, CachedLossFunction>;

    // Candidate solutions are generated adding and subtracting the parameters of this solution to the given one:
    Solution stepSolution{{ 1, 1, 1 }};

    // Declare local search object:
    LocalSearch localSearch(std::move(cachedLossFunction), stepSolution);

    // Declare VNS object with a maximum neighborhood (kmax) of 5:
    hike::VNS<Solution, LocalSearch> vns(std::move(localSearch), 5);

    // Optimize a solution:
    Solution solution{{ 15, -7, 22 }};
    bool optimized;
    Solution optimizedSolution = vns.optimize(solution, optimized);

    // The optimized solution should be equals to the target one:
    REQUIRE(optimized);
    REQUIRE(optimizedSolution == targetSolution);
}
```

## Debugging the optimization process

Optimization algorithms can receive a callback as parameter. This callback is called when the given solution is improved, allowing to trace the optimization process.  

In this example, `hike::VNS` class receives a callback which prints in console the input solution and the improved one. Please note that local search classes can receive it too:

```C++
#include <array>
#include <iostream>
#include <catch.hpp>
#include "hike_fi_local_search.h"
#include "hike_vns.h"

TEST_CASE("OnImprovedSolution example")
{
    // Solution is a 2D integer vector. It can be of any type and size:
    using Solution = std::array<int, 2>;

    // Loss function returns an integer scalar. It can be of any type:
    struct LossFunction
    {
        Solution targetSolution;

        int operator()(const Solution& solution) const
        {
            int loss = 0;

            for(std::size_t i = 0; i < solution.size(); ++i)
            {
                loss += std::abs(solution[i] - targetSolution[i]);
            }

            return loss;
        }
    };

    // Callback called when a given solution has been improved in an optimization algorithm:
    struct OnImprovedSolution
    {
        void operator()(const Solution& inputSolution, int inputLoss, const Solution& improvedSolution,
                        int improvedLoss, int k) const noexcept
        {
            std::cout << "Solution improved (k=" << k << ")! ";
            std::cout << "From (" << inputSolution[0] << ", " << inputSolution[1] << ") ";
            std::cout << "(loss=" << inputLoss << ") ";
            std::cout << "to (" << improvedSolution[0] << ", " << improvedSolution[1] << ") ";
            std::cout << "(loss=" << improvedLoss << ") " << std::endl;
        }
    };

    // Loss function returns the Manhattan distance between the target solution and the given one:
    Solution targetSolution{{ 2, 5 }};
    LossFunction lossFunction = { targetSolution };

    // VNS uses first improvement (first descent) local search.
    // Best improvement (highest descent) local search can be used too:
    using LocalSearch = hike::FILocalSearch<Solution, LossFunction>;

    // Candidate solutions are generated adding and subtracting the parameters of this solution to the given one:
    Solution stepSolution{{ 1, 1 }};

    // Declare local search object:
    LocalSearch localSearch(lossFunction, stepSolution);

    // Declare VNS object with a maximum neighborhood (kmax) of 5 and the solution improved callback class:
    hike::VNS<Solution, LocalSearch, OnImprovedSolution> vns(localSearch, 5, OnImprovedSolution());

    // Optimize a solution:
    Solution solution{{ 15, -7 }};
    bool optimized;
    Solution optimizedSolution = vns.optimize(solution, optimized);

    // The optimized solution should be equals to the target one:
    REQUIRE(optimized);
    REQUIRE(optimizedSolution == targetSolution);
}
```
