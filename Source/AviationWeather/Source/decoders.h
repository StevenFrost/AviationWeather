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

#include <AviationWeather/converters.h>
#include <AviationWeather/metar.h>

#include <string>

namespace aw
{

//-----------------------------------------------------------------------------

weather_intensity    decode_weather_intensity   (std::string const& symbol);
weather_descriptor   decode_weather_descriptor  (std::string const& symbol);
weather_phenomena    decode_weather_phenomena   (std::string const& symbol);
sky_cover_type       decode_sky_cover           (std::string const& symbol);
sky_cover_cloud_type decode_sky_cover_cloud_type(std::string const& symbol);
distance_unit        decode_distance_unit       (std::string const& symbol);
speed_unit           decode_speed_unit          (std::string const& symbol);

//-----------------------------------------------------------------------------

} // namespace aw
