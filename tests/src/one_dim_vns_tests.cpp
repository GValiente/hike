#include <array>
#include <catch.hpp>
#include "hike_fi_local_search.h"
#include "hike_bi_local_search.h"
#include "hike_vns.h"

namespace
{
    using Solution = std::array<int, 1>;

    class LossFunction
    {

    public:
        explicit LossFunction(const Solution& targetSolution) noexcept :
            _targetSolution(targetSolution)
        {
        }

        int operator()(const Solution& solution) const noexcept
        {
            int loss = 0;

            for(std::size_t i = 0, l = solution.size(); i < l; ++i)
            {
                loss += std::abs(solution[i] - _targetSolution[i]);
            }

            return loss;
        }

    protected:
        Solution _targetSolution;
    };

    template<class LocalSearch>
    void testVNS(const LocalSearch& localSearch, const Solution& targetSolution)
    {
        for(int k = 1; k < 5; ++k)
        {
            hike::VNS<Solution, LocalSearch> vns(localSearch, k);

            for(int p = -50; p <= 50; ++p)
            {
                Solution solution{{ p }};
                bool optimized;
                Solution optimizedSolution = vns.optimize(solution, optimized);
                REQUIRE(optimized == (solution != targetSolution));
                REQUIRE(optimizedSolution == targetSolution);
            }
        }
    }
}

TEST_CASE("1D FILocalSearch VNS")
{
    Solution targetSolution{{ 2 }};
    Solution stepSolution{{ 1 }};
    LossFunction lossFunction(targetSolution);
    using LocalSearch = hike::FILocalSearch<Solution, LossFunction>;
    LocalSearch localSearch(lossFunction, stepSolution);
    testVNS(localSearch, targetSolution);
}

TEST_CASE("1D BILocalSearch VNS")
{
    Solution targetSolution{{ 2 }};
    Solution stepSolution{{ 1 }};
    LossFunction lossFunction(targetSolution);
    using LocalSearch = hike::BILocalSearch<Solution, LossFunction>;
    LocalSearch localSearch(lossFunction, stepSolution);
    testVNS(localSearch, targetSolution);
}
