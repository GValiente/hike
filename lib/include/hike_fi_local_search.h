#ifndef HIKE_FI_LOCAL_SEARCH_H
#define HIKE_FI_LOCAL_SEARCH_H

#include "hike_local_search_base.h"

namespace hike
{

/**
 * @brief First improvement (first descent) local search.
 *
 * https://en.wikipedia.org/wiki/Variable_neighborhood_search
 */
template<class Solution, class LossFunction>
class FILocalSearch : public LocalSearchBase<LossFunction>
{

public:
    /**
     * @brief Class constructor.
     * @param lossFunction A solution that minimizes this function is an optimal solution.
     * @param stepSolution Candidate solutions are generated adding and subtracting
     * the parameters of this solution to the input one.
     * @param neighborhood Distance between the candidate solutions and the input one.
     */
    template<class LossFunctionType, class SolutionType>
    FILocalSearch(LossFunctionType&& lossFunction, SolutionType&& stepSolution, int neighborhood = 1) :
        LocalSearchBase<LossFunction>(std::forward<LossFunctionType>(lossFunction), neighborhood),
        _stepSolution(std::forward<SolutionType>(stepSolution))
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
        auto bestLoss = _BaseClass::_lossFunction(bestSolution);
        optimized = _optimize<false>(0, bestLoss, bestSolution);

        return bestSolution;
    }

protected:
    ///@cond INTERNAL

    using _BaseClass = LocalSearchBase<LossFunction>;

    Solution _stepSolution;

    template<bool checkCurrentStep, typename LossType>
    bool _optimize(std::size_t paramIndex, LossType bestLoss, Solution& solution)
    {
        if(paramIndex >= solution.size())
        {
            return false;
        }

        auto currentParam = solution[paramIndex];
        auto stepParam = _stepSolution[paramIndex] * _BaseClass::_neighborhood;

        // Previous step check:

        solution[paramIndex] = currentParam - stepParam;

        auto currentLoss = _BaseClass::_lossFunction(solution);

        if(currentLoss < bestLoss)
        {
            return true;
        }

        if(_optimize<true>(paramIndex + 1, bestLoss, solution))
        {
            return true;
        }

        // Current step check:

        solution[paramIndex] = currentParam;

        if(checkCurrentStep)
        {
            currentLoss = _BaseClass::_lossFunction(solution);

            if(currentLoss < bestLoss)
            {
                return true;
            }
        }

        if(_optimize<true>(paramIndex + 1, bestLoss, solution))
        {
            return true;
        }

        // Next step check:

        solution[paramIndex] = currentParam + stepParam;
        currentLoss = _BaseClass::_lossFunction(solution);

        if(currentLoss < bestLoss)
        {
            return true;
        }

        if(_optimize<true>(paramIndex + 1, bestLoss, solution))
        {
            return true;
        }

        // Restore solution:

        solution[paramIndex] = currentParam;

        return false;
    }

    ///@endcond
};

}

#endif
