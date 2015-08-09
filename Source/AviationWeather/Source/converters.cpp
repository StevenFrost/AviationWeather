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

#include "AviationWeatherPch.h"

#include <AviationWeather/converters.h>
#include <AviationWeather/types.h>

namespace aw
{
namespace detail
{

//-----------------------------------------------------------------------------

#define DISTANCE_UNIT_VALUES 4
std::array<double, DISTANCE_UNIT_VALUES * DISTANCE_UNIT_VALUES> distance_unit_conversion_ratio_table =
{
    /*  feet  */ /* metres */ /*  miles  */ /* nautical_miles */
    1.0        , 3.2808399  , 5280.0      , 6076.11549         , /* feet           */
    0.2048     , 1.0        , 1609.344    , 1852.0             , /* metres         */
    0.00018939 , 0.00062137 , 1.0         , 1.15077945         , /* miles          */
    0.00016458 , 0.00053996 , 0.86897624  , 1.0                , /* nautical_miles */
};

#define PRESSURE_UNIT_VALUES 2
std::array<double, PRESSURE_UNIT_VALUES * PRESSURE_UNIT_VALUES> pressure_unit_conversion_ratio_table =
{
    /*     hPa     */ /* inHg */
    1.0             , 33.768500, /* hPa  */
    0.02961339710085, 1.0        /* inHg */
};

//-----------------------------------------------------------------------------

double lookup_ratio(distance_unit from, distance_unit to)
{
    return distance_unit_conversion_ratio_table[(DISTANCE_UNIT_VALUES * static_cast<size_t>(to)) + static_cast<size_t>(from)];
}

//-----------------------------------------------------------------------------

double lookup_ratio(pressure_unit from, pressure_unit to)
{
    return pressure_unit_conversion_ratio_table[(PRESSURE_UNIT_VALUES * static_cast<size_t>(to)) + static_cast<size_t>(from)];
}

//-----------------------------------------------------------------------------

} // namespace detail
} // namespace aw
