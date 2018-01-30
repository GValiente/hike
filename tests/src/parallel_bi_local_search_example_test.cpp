#include <array>
#include <catch.hpp>
#include "hike_ts_cached_loss_function.h"
#include "hike_parallel_bi_local_search.h"
#include "hike_vns.h"

#include "hike_thread_pool.h"

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
            std::size_t result = 0;

            for(int param : solution)
            {
                // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x:
                result ^= std::hash<int>()(param) + 0x9e3779b9 + (result << 6) + (result >> 2);
            }

            return result;
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
