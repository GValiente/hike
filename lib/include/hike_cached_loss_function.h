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

#ifndef HIKE_CACHED_LOSS_FUNCTION_H
#define HIKE_CACHED_LOSS_FUNCTION_H

#include <unordered_map>

namespace hike
{

/**
 * @brief Remembers previously calculated losses.
 */
template<class Solution, class LossFunction>
class CachedLossFunction
{

public:
    /**
     * Loss function return type.
     */
    using LossType = typename std::result_of<LossFunction(const Solution&)>::type;

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
