#ifndef HIKE_LOCAL_SEARCH_BASE_H
#define HIKE_LOCAL_SEARCH_BASE_H

#include <utility>
#include "hike_common.h"

namespace hike
{

/**
 * @brief Local search base class.
 *
 * https://en.wikipedia.org/wiki/Variable_neighborhood_search
 */
template<class LossFunction>
class LocalSearchBase
{

public:
    /**
     * @brief Returns the loss function used to optimize solutions.
     */
    const LossFunction& getLossFunction() const noexcept
    {
        return _lossFunction;
    }

    /**
     * @brief Returns the loss function used to optimize solutions.
     */
    LossFunction& getLossFunction() noexcept
    {
        return _lossFunction;
    }

    /**
     * @brief Returns the distance between the candidate solutions and the input one.
     */
    int getNeighborhood() const noexcept
    {
        return _neighborhood;
    }

    /**
     * @brief Specifies the distance between the candidate solutions and the input one.
     */
    void setNeighborhood(int neighborhood)
    {
        HIKE_ASSERT(neighborhood > 0);

        _neighborhood = neighborhood;
    }

protected:
    ///@cond INTERNAL

    LossFunction _lossFunction;
    int _neighborhood;

    template<class LossFunctionType>
    LocalSearchBase(LossFunctionType&& lossFunction, int neighborhood) :
        _lossFunction(std::forward<LossFunctionType>(lossFunction))
    {
        setNeighborhood(neighborhood);
    }

    ///@endcond
};

}

#endif
