#include <array>
#include <catch.hpp>
#include "hike_cached_loss_function.h"
#include "hike_fi_local_search.h"
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
