// Copyright (c) 2018 Gustavo Valiente gustavo.valiente.m@gmail.com
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef HIKE_VNS_H
#define HIKE_VNS_H

#include <utility>
#include "hike_common.h"

namespace hike
{

/**
 * @brief Basic variable neighborhood search.
 *
 * It is aimed for solving linear program problems, integer program problems, mixed integer program problems,
 * nonlinear program problems, etc.
 *
 * https://en.wikipedia.org/wiki/Variable_neighborhood_search
 */
template<class Solution, class LocalSearch>
class VNS
{

public:
    /**
     * @brief Class constructor.
     * @param localSearch Object applied repeatedly to get from solutions in the neighborhood to local optima.
     * @param kmax Maximum distance between the candidate solutions and the input one.
     */
    template<class LocalSearchType>
    VNS(LocalSearchType&& localSearch, int kmax) :
        _localSearch(std::forward<LocalSearchType>(localSearch)),
        _kmax(kmax)
    {
        setKmax(kmax);
    }

    /**
     * @brief Returns the object applied repeatedly to get from solutions in the neighborhood to local optima.
     */
    const LocalSearch& getLocalSearch() const noexcept
    {
        return _localSearch;
    }

    /**
     * @brief Returns the object applied repeatedly to get from solutions in the neighborhood to local optima.
     */
    LocalSearch& getLocalSearch() noexcept
    {
        return _localSearch;
    }

    /**
     * @brief Returns the maximum distance between the candidate solutions and the input one.
     */
    int getKmax() const noexcept
    {
        return _kmax;
    }

    /**
     * @brief Specifies the maximum distance between the candidate solutions and the input one.
     */
    void setKmax(int kmax)
    {
        HIKE_ASSERT(kmax > 0);

        _kmax = kmax;
    }

    /**
     * @brief optimize Minimizes the loss function with the given solution.
     * @param solution The solution to optimize.
     * @return The optimized solution.
     */
    template<class SolutionType>
    Solution optimize(SolutionType&& solution)
    {
        bool optimized;

        return optimize(std::forward<SolutionType>(solution), optimized);
    }

    /**
     * @brief optimize Minimizes the loss function with the given solution.
     * @param solution The solution to optimize.
     * @param optimized Output parameter which indicates if the input solution has been optimized or not.
     * @return The optimized solution.
     */
    template<class SolutionType>
    Solution optimize(SolutionType&& solution, bool& optimized)
    {
        Solution bestSolution = std::forward<SolutionType>(solution);
        auto& lossFunction = _localSearch.getLossFunction();
        auto bestLoss = lossFunction(bestSolution);
        int k = 1;
        optimized = false;

        while(k <= _kmax)
        {
            _localSearch.setNeighborhood(k);

            bool localSearchOptimized;
            Solution currentSolution = _localSearch.optimize(bestSolution, localSearchOptimized);

            if(localSearchOptimized)
            {
                auto currentLoss = lossFunction(currentSolution);

                if(currentLoss < bestLoss)
                {
                    bestSolution = std::move(currentSolution);
                    bestLoss = currentLoss;
                    k = 1;
                    optimized = true;
                }
                else
                {
                    ++k;
                }
            }
            else
            {
                ++k;
            }
        }

        return bestSolution;
    }

protected:
    ///@cond INTERNAL

    LocalSearch _localSearch;
    int _kmax;

    ///@endcond
};

}

#endif
