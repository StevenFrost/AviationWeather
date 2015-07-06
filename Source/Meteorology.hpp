/****************************** Module Header ******************************\
Module Name:  Metar.hpp
Project:      Meteorology
Copyright (c) 2014 Steven Frost.

This module defines the Weather class including general meteorology methods,
meteorology-releated enumerations and type definitions for different
meteorology properties.

References:  (1) http://en.wikipedia.org/wiki/Weather

Todo:        (1) Complete type definitions
             (2) Complete enumerated list of cloud type and cover

This source is subject to the MIT License.
See http://opensource.org/licenses/MIT

All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

//-----------------------------------------------------------------------------

namespace Meteorology
{

//-----------------------------------------------------------------------------

inline double CentigradeToFahrenheit(double centigrade)
{
    return centigrade * 33.8;
}

inline double MetresToFeet(double metres)
{
    return metres * 3.2808399;
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

enum class WeatherIntensity
{
    Light,
    Moderate,
    Heavy,
    InTheVicinity
};

enum class WeatherDescriptor
{
    None,
    Shallow,
    Partial,
    Patches,
    LowDrifting,
    Blowing,
    Showers,
    Thunderstorm,
    Freezing
};

enum class WeatherPhenomena
{
    None,

    Drizzle,
    Rain,
    Snow,
    SnowGrains,
    IceCrystals,
    IcePellets,
    Hail,
    SmallHail,
    UnknownPrecipitation,

    Mist,
    Fog,
    Smoke,
    VolcanicAsh,
    WidespreadDust,
    Sand,
    Haze,
    Spray,

    WellDevelopedDustWhirls,
    Squalls,
    FunnelCloudTornadoWaterspout,
    Sandstorm,
    Duststorm
};

enum class SkyCover
{
    VerticalVisibility,
    Clear,
    Few,
    Scattered,
    Broken,
    Overcast
};

enum class SkyCoverClouds
{
    None,
    Cumulonimbus,
    ToweringCumulus
};

//-----------------------------------------------------------------------------

} // namespace Meterology
