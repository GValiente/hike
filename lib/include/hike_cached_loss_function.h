#ifndef HIKE_CACHED_LOSS_FUNCTION_H
#define HIKE_CACHED_LOSS_FUNCTION_H

#include <unordered_map>

namespace hike
{

/**
 * @brief Remembers previously calculated losses.
 */
template<class Solution, class LossFunction, typename LossType>
class CachedLossFunction
{

public:
    /**
     * @brief Class constructor.
     * @param lossFunction Loss function used to calculate the loss of new solutions.
     */
    explicit CachedLossFunction(const LossFunction& lossFunction) :
        _lossFunction(lossFunction)
    {
    }

    /**
     * @brief Class constructor.
     * @param lossFunction Loss function used to calculate the loss of new solutions.
     */
    explicit CachedLossFunction(LossFunction&& lossFunction) :
        _lossFunction(std::move(lossFunction))
    {
    }

    /**
     * @brief Returns the loss of the given solution.
     */
    LossType operator()(const Solution& solution)
    {
        auto lossIt = _losses.find(solution);

        if(lossIt != _losses.end())
        {
            return lossIt->second;
        }

        auto loss = _lossFunction(solution);
        static_assert(std::is_same<decltype(loss), LossType>::value, "Loss function return type doesn't match LossType");
        _losses.insert(std::make_pair(solution, loss));

        return loss;
    }

protected:
    ///@cond INTERNAL

    LossFunction _lossFunction;
    std::unordered_map<Solution, LossType> _losses;

    ///@endcond
};

}

#endif
