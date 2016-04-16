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

#include "decoders.h"

namespace aw
{

//-----------------------------------------------------------------------------

weather_intensity decode_weather_intensity(std::string const& symbol)
{
    if (symbol == "-") {
        return weather_intensity::light;
    }
    else if (symbol == "+") {
        return weather_intensity::heavy;
    }
    else if (symbol == "VC") {
        return weather_intensity::in_the_vicinity;
    }
    else {
        return weather_intensity::moderate;
    }
}

//-----------------------------------------------------------------------------

weather_descriptor decode_weather_descriptor(std::string const& symbol)
{
    if (symbol == "MI") {
        return weather_descriptor::shallow;
    }
    else if (symbol == "PR") {
        return weather_descriptor::partial;
    }
    else if (symbol == "BC") {
        return weather_descriptor::patches;
    }
    else if (symbol == "DR") {
        return weather_descriptor::low_drifting;
    }
    else if (symbol == "BL") {
        return weather_descriptor::blowing;
    }
    else if (symbol == "SH") {
        return weather_descriptor::showers;
    }
    else if (symbol == "TS") {
        return weather_descriptor::thunderstorm;
    }
    else if (symbol == "FZ") {
        return weather_descriptor::freezing;
    }
    else {
        return weather_descriptor::none;
    }
}

//-----------------------------------------------------------------------------

weather_phenomena decode_weather_phenomena(std::string const& symbol)
{
    if (symbol == "DZ") {
        return weather_phenomena::drizzle;
    }
    else if (symbol == "RA") {
        return weather_phenomena::rain;
    }
    else if (symbol == "SN") {
        return weather_phenomena::snow;
    }
    else if (symbol == "SG") {
        return weather_phenomena::snow_grains;
    }
    else if (symbol == "IC") {
        return weather_phenomena::ice_crystals;
    }
    else if (symbol == "PL") {
        return weather_phenomena::ice_pellets;
    }
    else if (symbol == "GR") {
        return weather_phenomena::hail;
    }
    else if (symbol == "GS") {
        return weather_phenomena::small_hail;
    }
    else if (symbol == "UP") {
        return weather_phenomena::small_hail;
    }
    else if (symbol == "BR") {
        return weather_phenomena::mist;
    }
    else if (symbol == "FG") {
        return weather_phenomena::fog;
    }
    else if (symbol == "FU") {
        return weather_phenomena::smoke;
    }
    else if (symbol == "VA") {
        return weather_phenomena::volcanic_ash;
    }
    else if (symbol == "DU") {
        return weather_phenomena::widespread_dust;
    }
    else if (symbol == "SA") {
        return weather_phenomena::sand;
    }
    else if (symbol == "HZ") {
        return weather_phenomena::haze;
    }
    else if (symbol == "PY") {
        return weather_phenomena::spray;
    }
    else if (symbol == "PO") {
        return weather_phenomena::well_developed_dust_whirls;
    }
    else if (symbol == "SQ") {
        return weather_phenomena::squalls;
    }
    else if (symbol == "FC") {
        return weather_phenomena::funnel_cloud_tornado_waterspout;
    }
    else if (symbol == "SS") {
        return weather_phenomena::sandstorm;
    }
    else if (symbol == "DS") {
        return weather_phenomena::duststorm;
    }
    else {
        return weather_phenomena::none;
    }
}

//-----------------------------------------------------------------------------

sky_cover_type decode_sky_cover(std::string const& symbol)
{
    if (symbol == "VV") {
        return sky_cover_type::vertical_visibility;
    }
    else if (symbol == "FEW") {
        return sky_cover_type::few;
    }
    else if (symbol == "SCT") {
        return sky_cover_type::scattered;
    }
    else if (symbol == "BKN") {
        return sky_cover_type::broken;
    }
    else if (symbol == "OVC") {
        return sky_cover_type::overcast;
    }
    else {
        return sky_cover_type::sky_clear;
    }
}

//-----------------------------------------------------------------------------

sky_cover_cloud_type decode_sky_cover_cloud_type(std::string const& symbol)
{
    if (symbol == "CB") {
        return sky_cover_cloud_type::cumulonimbus;
    }
    else if (symbol == "TCU") {
        return sky_cover_cloud_type::towering_cumulus;
    }
    else {
        return sky_cover_cloud_type::unspecified;
    }
}

//-----------------------------------------------------------------------------

distance_unit decode_distance_unit(std::string const& symbol)
{
    if (symbol == "FT") {
        return distance_unit::feet;
    }
    else {
        return distance_unit::metres;
    }
}

//-----------------------------------------------------------------------------

speed_unit decode_speed_unit(std::string const& symbol)
{
    if (symbol == "KT") {
        return speed_unit::kt;
    }
    else {
        return speed_unit::mph;
    }
}

//-----------------------------------------------------------------------------

} // namespace aw
