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

#include "AviationWeather.TestPch.h"

#include <string>

#include <AviationWeather/converters.h>
#include <AviationWeather/metar.h>
#include <AviationWeather/types.h>

#include "framework.h"

//-----------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//-----------------------------------------------------------------------------

namespace aw
{
namespace test
{

//-----------------------------------------------------------------------------

TEST_CLASS(MetarTests)
{
public:
    TEST_METHOD(METAR_VariableWinds);
    TEST_METHOD(METAR_FractionalVisibility);
    TEST_METHOD(METAR_VisibilityComparison);
    TEST_METHOD(METAR_Phenomena);
    TEST_METHOD(METAR_TemperatureDewpointSpread);
    TEST_METHOD(METAR_CeilingAndFlightCategory);
};

//-----------------------------------------------------------------------------

void MetarTests::METAR_VariableWinds()
{
    aw::metar m1("KSFO 112056Z VRB03KT 10SM FEW010 SCT180 22/12 A2994 RMK AO2 SLP137 T02220122 58006");

    Assert::IsTrue(m1.wind_group->is_variable());
    Assert::AreEqual(UINT16_MAX, m1.wind_group->direction);
    Assert::AreEqual(static_cast<uint8_t>(3), m1.wind_group->wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group->gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(0), m1.wind_group->gust_factor());
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_lower));
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_upper));

    aw::metar m2("KRHV 101451Z 29014G18KT 10SM SKC 27/13 A2990");

    Assert::IsFalse(m2.wind_group->is_variable());
    Assert::AreEqual(static_cast<uint16_t>(290), m2.wind_group->direction);
    Assert::AreEqual(static_cast<uint8_t>(14), m2.wind_group->wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(18), m2.wind_group->gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(4), m2.wind_group->gust_factor());
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_lower));
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_upper));

    aw::metar m3("KRHV 101850Z VRB02G06KT 10SM SKC 27/13 A2990");

    Assert::IsTrue(m3.wind_group->is_variable());
    Assert::AreEqual(UINT16_MAX, m3.wind_group->direction);
    Assert::AreEqual(static_cast<uint8_t>(2), m3.wind_group->wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(6), m3.wind_group->gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(4), m3.wind_group->gust_factor());
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_lower));
    Assert::IsFalse(static_cast<bool>(m1.wind_group->variation_upper));

    aw::metar m5("KRHV 112150Z 22512G15KT 200V250 10SM SKC 27/13 A2990");

    Assert::IsTrue(m5.wind_group->is_variable());
    Assert::AreEqual(static_cast<uint16_t>(225), m5.wind_group->direction);
    Assert::AreEqual(static_cast<uint8_t>(12), m5.wind_group->wind_speed);
    Assert::AreEqual(static_cast<uint8_t>(15), m5.wind_group->gust_speed);
    Assert::AreEqual(static_cast<uint8_t>(3), m5.wind_group->gust_factor());
    Assert::AreEqual(static_cast<uint16_t>(200), *(m5.wind_group->variation_lower));
    Assert::AreEqual(static_cast<uint16_t>(250), *(m5.wind_group->variation_upper));
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_FractionalVisibility()
{
    aw::metar m1("KSFO 121156Z 28005KT M1/4SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m1.visibility_group->unit);
    Assert::AreEqual(aw::visibility_modifier_type::less_than, m1.visibility_group->modifier);
    Assert::AreEqual(1.0 / 4.0, m1.visibility_group->distance, 0.01);

    aw::metar m2("KSFO 121156Z 28005KT 1/2SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m2.visibility_group->unit);
    Assert::AreEqual(aw::visibility_modifier_type::none, m2.visibility_group->modifier);
    Assert::AreEqual(1.0 / 2.0, m2.visibility_group->distance, 0.01);

    aw::metar m3("KSFO 121156Z 28005KT 1 1/2SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m3.visibility_group->unit);
    Assert::AreEqual(aw::visibility_modifier_type::none, m3.visibility_group->modifier);
    Assert::AreEqual(1.0 + (1.0 / 2.0), m3.visibility_group->distance, 0.01);

    aw::metar m4("KSFO 121156Z 28005KT 15SM FEW006 15/12 A3000");

    Assert::AreEqual(aw::distance_unit::statute_miles, m4.visibility_group->unit);
    Assert::AreEqual(aw::visibility_modifier_type::none, m4.visibility_group->modifier);
    Assert::AreEqual(15.0, m4.visibility_group->distance, 0.01);
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_VisibilityComparison()
{
    const double distance = 10.0;

    aw::visibility v1(distance, aw::distance_unit::statute_miles, aw::visibility_modifier_type::none);
    aw::visibility v2(distance, aw::distance_unit::statute_miles, aw::visibility_modifier_type::less_than);
    aw::visibility v3(distance - 1.0, aw::distance_unit::statute_miles, aw::visibility_modifier_type::none);
    aw::visibility v4(distance - 1.0, aw::distance_unit::statute_miles, aw::visibility_modifier_type::less_than);

    // v1, v2
    Assert::IsFalse(v1 == v2); Assert::IsTrue(v1 != v2);
    Assert::IsFalse(v1 <= v2); Assert::IsTrue(v1 >= v2);
    Assert::IsFalse(v1 < v2);  Assert::IsTrue(v1 > v2);

    // v1, v3
    Assert::IsFalse(v1 == v3); Assert::IsTrue(v1 != v3);
    Assert::IsFalse(v1 <= v3); Assert::IsTrue(v1 >= v3);
    Assert::IsFalse(v1 < v3);  Assert::IsTrue(v1 > v3);

    // v1, v4
    Assert::IsFalse(v1 == v4); Assert::IsTrue(v1 != v4);
    Assert::IsFalse(v1 <= v4); Assert::IsTrue(v1 >= v4);
    Assert::IsFalse(v1 < v4);  Assert::IsTrue(v1 > v4);


    // v2, v1
    Assert::IsFalse(v2 == v1); Assert::IsTrue(v2 != v1);
    Assert::IsTrue(v2 <= v1);  Assert::IsFalse(v2 >= v1);
    Assert::IsTrue(v2 < v1);   Assert::IsFalse(v2 > v1);

    // v2, v3
    Assert::IsFalse(v2 == v3); Assert::IsTrue(v2 != v3);
    Assert::IsFalse(v2 <= v3); Assert::IsTrue(v2 >= v3);
    Assert::IsFalse(v2 < v3);  Assert::IsTrue(v2 > v3);

    // v2, v4
    Assert::IsFalse(v2 == v4); Assert::IsTrue(v2 != v4);
    Assert::IsFalse(v2 <= v4); Assert::IsTrue(v2 >= v4);
    Assert::IsFalse(v2 < v4);  Assert::IsTrue(v2 > v4);


    // v3, v1
    Assert::IsFalse(v3 == v1); Assert::IsTrue(v3 != v1);
    Assert::IsTrue(v3 <= v1);  Assert::IsFalse(v3 >= v1);
    Assert::IsTrue(v3 < v1);   Assert::IsFalse(v3 > v1);

    // v3, v2
    Assert::IsFalse(v3 == v2); Assert::IsTrue(v3 != v2);
    Assert::IsTrue(v3 <= v2);  Assert::IsFalse(v3 >= v2);
    Assert::IsTrue(v3 < v2);   Assert::IsFalse(v3 > v2);

    // v3, v4
    Assert::IsFalse(v3 == v4); Assert::IsTrue(v3 != v4);
    Assert::IsFalse(v3 <= v4); Assert::IsTrue(v3 >= v4);
    Assert::IsFalse(v3 < v4);  Assert::IsTrue(v3 > v4);


    // v4, v1
    Assert::IsFalse(v4 == v1); Assert::IsTrue(v4 != v1);
    Assert::IsTrue(v4 <= v1);  Assert::IsFalse(v4 >= v1);
    Assert::IsTrue(v4 < v1);   Assert::IsFalse(v4 > v1);

    // v4, v2
    Assert::IsFalse(v4 == v2); Assert::IsTrue(v4 != v2);
    Assert::IsTrue(v4 <= v2);  Assert::IsFalse(v4 >= v2);
    Assert::IsTrue(v4 < v2);   Assert::IsFalse(v4 > v2);

    // v4, v3
    Assert::IsFalse(v4 == v3); Assert::IsTrue(v4 != v3);
    Assert::IsTrue(v4 <= v3);  Assert::IsFalse(v4 >= v3);
    Assert::IsTrue(v4 < v3);   Assert::IsFalse(v4 > v3);
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_Phenomena()
{
    aw::metar m1("KCCR 181953Z 28011KT 10SM FU CLR 27/12 A2990 RMK AO2 SLP110 T02720122");

    Assert::AreEqual(1U, static_cast<uint32_t>(m1.weather_group.size()));
    auto it = m1.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());

    aw::metar m2("KRHV 181947Z 33008/KT 6SM HZ SKC 29/12 A2992");

    Assert::AreEqual(1U, static_cast<uint32_t>(m2.weather_group.size()));
    it = m2.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());

    aw::metar m3("KSTS 181253Z AUTO 00000KT 10SM -RA HZ FU OVC007 14/12 A2990 RMK AO2 RAB30 SLP116 P0000 T01390122");

    Assert::AreEqual(3U, static_cast<uint32_t>(m3.weather_group.size()));
    it = m3.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::light, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::rain, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());

    aw::metar m4("KSTS 182253Z 19008KT 9SM FU HZ CLR 25/13 A2988 RMK AO2 SLP112 T02500133");

    Assert::AreEqual(2U, static_cast<uint32_t>(m4.weather_group.size()));
    it = m4.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::smoke, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::haze, it->phenomena.front());

    aw::metar m5("KWVI 171553Z AUTO 00000KT 1 1/4SM BR OVC002 17/15 A2989 RMK AO2 SLP122 T01670150");

    Assert::AreEqual(1U, static_cast<uint32_t>(m5.weather_group.size()));
    it = m5.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::mist, it->phenomena.front());

    aw::metar m6("KWVI 171453Z AUTO 00000KT 1/4SM FG VV002 16/15 A2989 RMK AO2 SLP119 T01610150 53007");

    Assert::AreEqual(1U, static_cast<uint32_t>(m6.weather_group.size()));
    it = m6.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::fog, it->phenomena.front());

    aw::metar m7("KORD 190151Z 19010KT 5SM TSRA BR FEW027 BKN048CB OVC090 21/19 A2971 RMK AO2 PK WND 18028/0112 SLP057 FRQ LTGICCG OHD TS OHD MOV NE P0023 T02060194");

    Assert::AreEqual(2U, static_cast<uint32_t>(m7.weather_group.size()));
    it = m7.weather_group.begin();
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::thunderstorm, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::rain, it->phenomena.front());
    it++;
    Assert::AreEqual(aw::weather_intensity::moderate, it->intensity);
    Assert::AreEqual(aw::weather_descriptor::none, it->descriptor);
    Assert::AreEqual(1U, static_cast<uint32_t>(it->phenomena.size()));
    Assert::AreEqual(aw::weather_phenomena::mist, it->phenomena.front());
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_TemperatureDewpointSpread()
{
    aw::metar m1("KSFO 010956Z 00000KT 10SM FEW011 16/13 A2987 RMK AO2 SLP114 T01610133");
    Assert::AreEqual(int8_t(16), *(m1.temperature));
    Assert::AreEqual(int8_t(13), *(m1.dewpoint));
    Assert::AreEqual(int16_t(3), m1.temperature_dewpoint_spread());

    aw::metar m2("ENSB 010950Z 03003KT 340V060 9999 FEW030 03/M04 Q1018 NOSIG RMK WIND 1400FT 02004KT");
    Assert::AreEqual(int8_t(3), *(m2.temperature));
    Assert::AreEqual(int8_t(-4), *(m2.dewpoint));
    Assert::AreEqual(int16_t(7), m2.temperature_dewpoint_spread());

    aw::metar m3("EGBE 311150Z 35008KT 6000 -RA SCT008 BKN011 OVC014 14/14 Q1014");
    Assert::AreEqual(int8_t(14), *(m3.temperature));
    Assert::AreEqual(int8_t(14), *(m3.dewpoint));
    Assert::AreEqual(int16_t(0), m3.temperature_dewpoint_spread());
}

//-----------------------------------------------------------------------------

void MetarTests::METAR_CeilingAndFlightCategory()
{
    aw::metar m1("KRHV 042147Z 32010KT 10SM SKC 25/10 A2986");
    auto ceiling = m1.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::sky_clear, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::none, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::vfr, m1.flight_category());

    aw::metar m2("KHWD 042054Z 26014G18KT 10SM CLR 20/09 A2989 RMK AO2 SLP130 T02000094 58004");
    ceiling = m2.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::clear_below_12000, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::none, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::vfr, m2.flight_category());

    aw::metar m3("KOAK 042053Z 24012KT 10SM FEW010 22/09 A2987 RMK AO2 SLP116 T02170094 58007");
    ceiling = m3.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::sky_clear, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::vfr, m3.flight_category());

    aw::metar m4("KHWD 291917Z 29010KT 10SM BKN010 BKN018 OVC023 21/18 A3001 RMK AO2");
    ceiling = m4.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(1000), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::broken, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m4.flight_category());

    aw::metar m5("KSEA 041604Z 36007KT 10SM OVC010 12/08 A3002 RMK AO2");
    ceiling = m5.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(1000), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::overcast, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m5.flight_category());

    aw::metar m6("KLAX 040953Z VRB04KT 10SM OVC030 20/14 A2986 RMK AO2 SLP108 T02000144 $");
    ceiling = m6.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(3000), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::overcast, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m6.flight_category());

    aw::metar m7("KSFO 102356Z 27019G24KT 10SM FEW008 SCT012 BKN015 21/14 A2994 RMK AO2 PK WND 27028/2303 SLP137 T02060144 10228 20200 58012");
    ceiling = m7.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(1500), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::broken, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m7.flight_category());

    aw::metar m8("KSTS 152153Z 24008KT 3SM HZ FU CLR 35/09 A2989 RMK AO2 SLP116 T03500089");
    ceiling = m8.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::clear_below_12000, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::none, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m8.flight_category());

    aw::metar m9("KPAO 152147Z 34010KT 4SM HZ SCT150 34/12 2992");
    ceiling = m9.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::sky_clear, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m9.flight_category());

    aw::metar m10("KAPC 151954Z 21007KT 170V230 5SM HZ FU CLR 31/12 A2996 RMK AO2 SLP135 SMOKE ALQDS UP TO 7500 MSL FLIGHT VIS 4-5 MI T03110117");
    ceiling = m10.ceiling();
    Assert::IsTrue(ceiling.is_unlimited());
    Assert::AreEqual(UINT32_MAX, ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::clear_below_12000, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::none, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::mvfr, m10.flight_category());

    aw::metar m11("KSEA 041453Z 36007KT 10SM OVC007 11/09 A3002 RMK AO2 SLP171 T01110089 53006");
    ceiling = m11.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(700), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::overcast, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::ifr, m11.flight_category());

    aw::metar m12("KPDX 041653Z 28005KT 7SM SCT003 BKN007 OVC016 12/09 A3000 RMK AO2 SLP159 T01170094");
    ceiling = m12.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(700), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::broken, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::ifr, m12.flight_category());

    aw::metar m13("KPDX 041453Z 32007KT 1SM R10R/3000VP6000FT BR BKN003 OVC010 10/09 A3000 RMK AO2 SFC VIS 4 SLP157 VIS SW-NW 1 1/2 T01000094 53005");
    ceiling = m13.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(300), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::broken, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::lifr, m13.flight_category());

    aw::metar m14("KPDX 041053Z 35003KT 10SM OVC004 11/10 A2998 RMK AO2 SLP150 T01110100");
    ceiling = m14.ceiling();
    Assert::IsFalse(ceiling.is_unlimited());
    Assert::AreEqual(uint32_t(400), ceiling.layer_height);
    Assert::AreEqual(aw::distance_unit::feet, ceiling.unit);
    Assert::AreEqual(aw::sky_cover_type::overcast, ceiling.sky_cover);
    Assert::AreEqual(aw::sky_cover_cloud_type::unspecified, ceiling.cloud_type);
    Assert::AreEqual(aw::flight_category::lifr, m14.flight_category());

    aw::metar::metar_info m15("KHAF 291935Z AUTO 24004KT 10SM 18/18 A3004 RMK AO2");
    Assert::ExpectException<aw_exception>([&m15]
    {
        m15.ceiling();
    });
    Assert::AreEqual(aw::flight_category::unknown, m15.flight_category());
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
