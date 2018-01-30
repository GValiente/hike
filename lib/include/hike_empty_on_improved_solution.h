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

#ifndef HIKE_EMPTY_ON_IMPROVED_SOLUTION_H
#define HIKE_EMPTY_ON_IMPROVED_SOLUTION_H

#include "hike_common.h"

namespace hike
{

/**
 * @brief Empty improved solution callback.
 */
class EmptyOnImprovedSolution
{

public:
    /**
     * @brief Called when a given solution has been improved in an optimization algorithm.
     * @param inputLoss Input solution loss.
     * @param improvedSolution Solution with lower loss.
     * @param improvedLoss Improved solution loss.
     * @param params Optimization algorithm parameters.
     */
    template<class Solution, typename LossType, typename Params>
    void operator()(LossType inputLoss, const Solution& improvedSolution, LossType improvedLoss,
                    Params params) const noexcept
    {
	HIKE_UNUSED(inputLoss);
	HIKE_UNUSED(improvedSolution);
	HIKE_UNUSED(improvedLoss);
	HIKE_UNUSED(params);
    }

    /**
     * @brief Called when a given solution has been improved in an optimization algorithm.
     * @param inputSolution Solution with higher loss.
     * @param inputLoss Input solution loss.
     * @param improvedSolution Solution with lower loss.
     * @param improvedLoss Improved solution loss.
     * @param params Optimization algorithm parameters.
     */
    template<class Solution, typename LossType, typename Params>
    void operator()(const Solution& inputSolution, LossType inputLoss, const Solution& improvedSolution,
                    LossType improvedLoss, Params params) const noexcept
    {
	HIKE_UNUSED(inputSolution);
	HIKE_UNUSED(inputLoss);
	HIKE_UNUSED(improvedSolution);
	HIKE_UNUSED(improvedLoss);
	HIKE_UNUSED(params);
    }
};

}

#endif
