/**********************************************************************************
 *                                                                                *
 * Copyright (c) 2015 Steven Frost, Orion Lyau. All rights reserved.              *
 *                                                                                *
 * This source is subject to the MIT License.                                     *
 * See http://opensource.org/licenses/MIT                                         *
 *                                                                                *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,    *
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED          *
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.         *
 *                                                                                *
 * NOT TO BE USED AS A SOLE SOURCE OF INFORMATION FOR FLIGHT CRITICAL OPERATIONS. *
 *                                                                                *
 **********************************************************************************/

#pragma once

#include <array>

#include <AviationWeather/types.h>

namespace aw
{
namespace detail
{

//-----------------------------------------------------------------------------

double lookup_ratio(distance_unit from, distance_unit to);
double lookup_ratio(pressure_unit from, pressure_unit to);

//-----------------------------------------------------------------------------

} // namespace detail

//-----------------------------------------------------------------------------

template <class TReturn = double, class TValue, class TA, class TB>
TReturn convert(TValue const& value, TA const& from, TB const& to)
{
    if (from == to)
    {
        return value;
    }
    return static_cast<TReturn>(value * detail::lookup_ratio(from, to));
}

//-----------------------------------------------------------------------------

} // namespace aw
