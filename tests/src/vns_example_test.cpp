#include <array>
#include <catch.hpp>
#include "hike_fi_local_search.h"
#include "hike_vns.h"

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
