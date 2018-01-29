#ifndef HIKE_TS_CACHED_LOSS_FUNCTION_H
#define HIKE_TS_CACHED_LOSS_FUNCTION_H

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <mutex>

namespace hike
{

/**
 * @brief Remembers previously calculated losses from multiple threads.
 *
 * This class is thread safe as long as the child loss function is thread safe too.
 */
template<class Solution, class LossFunction>
class TSCachedLossFunction
{

public:
    /**
     * Loss function return type.
     */
    using LossType = typename std::result_of<LossFunction(const Solution&)>::type;

    /**
     * @brief Class constructor.
     * @param lossFunction Thread safe loss function used to calculate the loss of new solutions.
     */
    explicit TSCachedLossFunction(const LossFunction& lossFunction) :
        _lossFunction(lossFunction),
        _mutex(new std::mutex())
    {
    }

    /**
     * @brief Class constructor.
     * @param lossFunction Thread safe loss function used to calculate the loss of new solutions.
     */
    explicit TSCachedLossFunction(LossFunction&& lossFunction) :
        _lossFunction(std::move(lossFunction)),
        _mutex(new std::mutex())
    {
    }

    /**
     * @brief Returns the loss of the given solution.
     */
    LossType operator()(const Solution& solution)
    {
        {
            std::lock_guard<std::mutex> lock(*_mutex);

            auto lossIt = _losses.find(solution);

            if(lossIt != _losses.end())
            {
                return lossIt->second;
            }
        }

        auto loss = _lossFunction(solution);

        {
            std::lock_guard<std::mutex> lock(*_mutex);

            _losses.insert(std::make_pair(solution, loss));
        }

        return loss;
    }

protected:
    ///@cond INTERNAL

    LossFunction _lossFunction;
    std::unordered_map<Solution, LossType> _losses;
    std::unique_ptr<std::mutex> _mutex;

    ///@endcond
};

}

#endif
