#include <array>
#include <catch.hpp>
#include "hike_fi_local_search.h"
#include "hike_bi_local_search.h"
#include "hike_vns.h"

namespace
{
    using Solution = std::array<int, 3>;

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

            for(int p1 = -10; p1 <= 10; ++p1)
            {
                for(int p2 = -10; p2 <= 10; ++p2)
                {
                    for(int p3 = -10; p3 <= 10; ++p3)
                    {
                        Solution solution{{ p1, p2, p3 }};
                        bool optimized;
                        Solution optimizedSolution = vns.optimize(solution, optimized);
                        REQUIRE(optimized == (solution != targetSolution));
                        REQUIRE(optimizedSolution == targetSolution);
                    }
                }
            }
        }
    }
}

TEST_CASE("3D FILocalSearch VNS")
{
    Solution targetSolution{{ 2, 5, -10 }};
    Solution stepSolution{{ 1, 1, 1 }};
    LossFunction lossFunction(targetSolution);
    using LocalSearch = hike::FILocalSearch<Solution, LossFunction>;
    LocalSearch localSearch(lossFunction, stepSolution);
    testVNS(localSearch, targetSolution);
}

TEST_CASE("3D BILocalSearch VNS")
{
    Solution targetSolution{{ 2, 5, -10 }};
    Solution stepSolution{{ 1, 1, 1 }};
    LossFunction lossFunction(targetSolution);
    using LocalSearch = hike::BILocalSearch<Solution, LossFunction>;
    LocalSearch localSearch(lossFunction, stepSolution);
    testVNS(localSearch, targetSolution);
}
