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

#include <CppUnitTest.h>
#include <cstdint>
#include <string>

#include <AviationWeather/metar.h>
#include <AviationWeather/types.h>

namespace
{

#define STRINGIFY(x) #x

#define DEFINE_ENUM_TOSTRING_GROUP(x) \
    template<> inline std::wstring ToString<x>(const x& t) { RETURN_WIDE_STRING(static_cast<uint32_t>(t)); } \
    template<> inline std::wstring ToString<x>(const x* t) { RETURN_WIDE_STRING(t); } \
    template<> inline std::wstring ToString<x>(x* t)       { RETURN_WIDE_STRING(t); }

}

namespace Microsoft
{
namespace VisualStudio
{
namespace CppUnitTestFramework
{

// uint16_t
template<> inline std::wstring ToString<uint16_t>(const uint16_t& t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<uint16_t>(const uint16_t* t) { RETURN_WIDE_STRING(t); }
template<> inline std::wstring ToString<uint16_t>(uint16_t* t)       { RETURN_WIDE_STRING(t); }

DEFINE_ENUM_TOSTRING_GROUP(aw::weather_intensity)
DEFINE_ENUM_TOSTRING_GROUP(aw::weather_descriptor)
DEFINE_ENUM_TOSTRING_GROUP(aw::weather_phenomena)
DEFINE_ENUM_TOSTRING_GROUP(aw::sky_cover_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::sky_cover_cloud_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::flight_category);
DEFINE_ENUM_TOSTRING_GROUP(aw::speed_unit)
DEFINE_ENUM_TOSTRING_GROUP(aw::distance_unit)
DEFINE_ENUM_TOSTRING_GROUP(aw::pressure_unit)

DEFINE_ENUM_TOSTRING_GROUP(aw::metar::element_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::metar::report_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::metar::modifier_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::metar::runway_designator_type)
DEFINE_ENUM_TOSTRING_GROUP(aw::metar::visibility_modifier_type)

} // namespace CppUnitTestFramework
} // namespace VisualStudio
} // namespace Microsoft
