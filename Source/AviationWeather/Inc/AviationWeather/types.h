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

//-----------------------------------------------------------------------------

namespace aw
{

//-----------------------------------------------------------------------------

inline double CelsiusToFahrenheit(double celsius)
{
    return celsius * ((9.0 / 5.0) + 32);
}

inline double MetresToFeet(double metres)
{
    return metres / 0.3048;
}

inline double MilesToFeet(double miles)
{
    return miles * 5280;
}

inline double InhgToHpa(double inhg)
{
    return inhg * 33.8638816;
}

inline double HpaToInhg(double hpa)
{
    return hpa / 33.8638816;
}

//-----------------------------------------------------------------------------

enum class weather_intensity
{
    light,
    moderate,
    heavy,
    in_the_vicinity
};

enum class weather_descriptor
{
    none,
    shallow,
    partial,
    patches,
    low_drifting,
    blowing,
    showers,
    thunderstorm,
    freezing
};

enum class weather_phenomena
{
    none,

    drizzle,
    rain,
    snow,
    snow_grains,
    ice_crystals,
    ice_pellets,
    hail,
    small_hail,
    unknown_precipitation,

    mist,
    fog,
    smoke,
    volcanic_ash,
    widespread_dust,
    sand,
    haze,
    spray,

    well_developed_dust_whirls,
    squalls,
    funnel_cloud_tornado_waterspout,
    sandstorm,
    duststorm
};

enum class sky_cover_type
{
    vertical_visibility,
    clear,
    few,
    scattered,
    broken,
    overcast
};

enum class sky_cover_cloud_type
{
    none,
    cumulonimbus,
    towering_cumulus
};

enum class speed_unit
{
    kt  = 0, // Knots
    mph = 1  // Miles per hour
};

enum class distance_unit
{
    ft = 0, // Feet
    m  = 1  // Metres
};

enum class pressure_unit
{
    hPa  = 0, // Hectopascals
    inHg = 1  // Inches of mercury
};

//-----------------------------------------------------------------------------

} // namespace aw
