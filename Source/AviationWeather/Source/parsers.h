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

#include <AviationWeather/metar.h>

#include <string>

//-----------------------------------------------------------------------------

namespace aw
{
namespace metar
{
namespace parsers
{

//-----------------------------------------------------------------------------

void parse_report_type         (metar_info& info, std::string& metar);
void parse_station_identifier  (metar_info& info, std::string& metar);
void parse_observation_time    (metar_info& info, std::string& metar);
void parse_modifier            (metar_info& info, std::string& metar);
void parse_wind                (metar_info& info, std::string& metar);
void parse_visibility          (metar_info& info, std::string& metar);
void parse_runway_visual_range (metar_info& info, std::string& metar);
void parse_weather             (metar_info& info, std::string& metar);
void parse_sky_condition       (metar_info& info, std::string& metar);
void parse_temperature_dewpoint(metar_info& info, std::string& metar);
void parse_altimeter           (metar_info& info, std::string& metar);
void parse_remarks             (metar_info& info, std::string& metar);

//-----------------------------------------------------------------------------

} // namespace parsers
} // namespace metar
} // namespace aw
