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
