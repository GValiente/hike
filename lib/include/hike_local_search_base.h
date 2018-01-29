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
