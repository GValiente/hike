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

#ifndef HIKE_PARALLEL_BI_LOCAL_SEARCH_H
#define HIKE_PARALLEL_BI_LOCAL_SEARCH_H

#include "hike_local_search_base.h"
#include "hike_thread_pool.h"

namespace hike
{

/**
 * @brief Multithread best improvement (highest descent) local search.
 *
 * https://en.wikipedia.org/wiki/Variable_neighborhood_search
 *
 * Please note than the given loss function must be thread safe.
 */
template<class Solution, class LossFunction>
class ParallelBILocalSearch : public LocalSearchBase<LossFunction>
{

public:
    /**
     * @brief Class constructor.
     * @param lossFunction A solution that minimizes this function is an optimal solution. It must be thread safe.
     * @param stepSolution Candidate solutions are generated adding and subtracting
     * the parameters of this solution to the input one.
     * @param neighborhood Distance between the candidate solutions and the input one.
     */
    template<class LossFunctionType, class SolutionType>
    ParallelBILocalSearch(LossFunctionType&& lossFunction, SolutionType&& stepSolution, int neighborhood = 1) :
        LocalSearchBase<LossFunction>(std::forward<LossFunctionType>(lossFunction), neighborhood),
        _stepSolution(std::forward<SolutionType>(stepSolution)),
        _threadPool(new ThreadPool<LossTask>())
    {
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
     * @param optimized Output parameter which indicates if the given solution has been optimized or not.
     * @return The optimized solution.
     */
    template<class SolutionType>
    Solution optimize(SolutionType&& solution, bool& optimized)
    {
        HIKE_ASSERT(solution.size() == _stepSolution.size());

        Solution bestSolution = std::forward<SolutionType>(solution);
        _optimize<false>(0, solution);

        for(SolutionLossPair& solutionAndLoss : _solutionsAndLosses)
        {
            _threadPool->add(LossTask(_BaseClass::_lossFunction, solutionAndLoss));
        }

        auto bestLoss = _BaseClass::_lossFunction(bestSolution);
        _threadPool->join();
        optimized = false;

        for(const SolutionLossPair& solutionAndLoss : _solutionsAndLosses)
        {
            if(solutionAndLoss.second < bestLoss)
            {
                bestSolution = std::move(solutionAndLoss.first);
                bestLoss = solutionAndLoss.second;
                optimized = true;
            }
        }

        _solutionsAndLosses.clear();

        return bestSolution;
    }

protected:
    ///@cond INTERNAL

    using _BaseClass = LocalSearchBase<LossFunction>;
    using LossType = typename std::result_of<LossFunction(const Solution&)>::type;
    using SolutionLossPair = std::pair<Solution, LossType>;

    class LossTask
    {

    public:
        LossTask(LossFunction& lossFunction, SolutionLossPair& solutionAndLoss) :
            _lossFunction(lossFunction),
            _solutionAndLoss(solutionAndLoss)
        {
        }

        void operator()()
        {
            _solutionAndLoss.second = _lossFunction(_solutionAndLoss.first);
        }

    protected:
        LossFunction& _lossFunction;
        SolutionLossPair& _solutionAndLoss;
    };

    Solution _stepSolution;
    std::vector<SolutionLossPair> _solutionsAndLosses;
    std::unique_ptr<ThreadPool<LossTask>> _threadPool;

    template<bool checkCurrentStep>
    void _optimize(std::size_t paramIndex, Solution& solution)
    {
        if(paramIndex < solution.size())
        {
            auto currentParam = solution[paramIndex];
            auto stepParam = _stepSolution[paramIndex] * _BaseClass::_neighborhood;

            // Previous step check:

            solution[paramIndex] = currentParam - stepParam;
            _solutionsAndLosses.push_back(std::make_pair(solution, LossType()));

            _optimize<true>(paramIndex + 1, solution);

            // Current step check:

            solution[paramIndex] = currentParam;

            if(checkCurrentStep)
            {
                _solutionsAndLosses.push_back(std::make_pair(solution, LossType()));
            }

            _optimize<true>(paramIndex + 1, solution);

            // Next step check:

            solution[paramIndex] = currentParam + stepParam;
            _solutionsAndLosses.push_back(std::make_pair(solution, LossType()));

            _optimize<true>(paramIndex + 1, solution);

            // Restore solution:

            solution[paramIndex] = currentParam;
        }
    }

    ///@endcond
};

}

#endif
