hike
====

hike is a small C++11 [mathematical optimization](https://en.wikipedia.org/wiki/Mathematical_optimization) library.  

It currently only provides a [variable neighborhood search (VNS)](https://en.wikipedia.org/wiki/Variable_neighborhood_search) with first improvement (first descent) and best improvement (highest descent) local search.

## Features

- Header-only library.
- Solutions can be of any type and size.
- Loss functions can return any type.
- Calculated losses can be cached to speedup the optimization process.
- Low overhead, no heap usage (besides loss caching).
- Without dependencies (besided [catch](https://github.com/catchorg/Catch2) for testing).
- [Doxygen documentation](https://gvaliente.github.io/hike/) provided for API reference.
- Licensed under [zlib license](LICENSE.txt).

## Tested compilers

- GCC 5.4.
- MSVC 15.0 (Visual Studio 2017).

## Usage

Although hike is a header-only library, a CMakeLists.txt file is provided to facilitate being added as a dependency.

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

With [hike::CachedLossFunction](https://gvaliente.github.io/hike/classhike_1_1CachedLossFunction.html) class, calculated losses for the same solutions are cached to avoid to recalculate them in following iterations:

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
    using CachedLossFunction = hike::CachedLossFunction<Solution, LossFunction, int>;
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

