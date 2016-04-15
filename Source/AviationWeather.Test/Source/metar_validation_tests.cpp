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

#include <fstream>
#include <sstream>
#include <string>

#include <AviationWeather/converters.h>
#include <AviationWeather/metar.h>
#include <AviationWeather/types.h>

#include <JSON/json.h>

//-----------------------------------------------------------------------------

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nlohmann;

//-----------------------------------------------------------------------------

namespace
{

std::string element_type_strings[] =
{
    STRINGIFY(report_type),
    STRINGIFY(station_identifier),
    STRINGIFY(observation_time),
    STRINGIFY(report_modifier),
    STRINGIFY(wind),
    STRINGIFY(visibility),
    STRINGIFY(runway_visual_range),
    STRINGIFY(weather),
    STRINGIFY(sky_condition),
    STRINGIFY(temperature_dewpoint),
    STRINGIFY(altimeter),
    STRINGIFY(remarks)
};

std::string metar_report_type_strings[] =
{
    STRINGIFY(metar),
    STRINGIFY(special)
};

std::string metar_modifier_type_strings[] =
{
    STRINGIFY(none),
    STRINGIFY(automatic),
    STRINGIFY(corrected)
};

std::string runway_designator_type_strings[] =
{
    STRINGIFY(none),
    STRINGIFY(left),
    STRINGIFY(right),
    STRINGIFY(center)
};

std::string visibility_modifier_type_strings[] =
{
    STRINGIFY(none),
    STRINGIFY(less_than),
    STRINGIFY(greater_than)
};

std::string weather_intensity_strings[] =
{
    STRINGIFY(light),
    STRINGIFY(moderate),
    STRINGIFY(heavy),
    STRINGIFY(in_the_vicinity)
};

std::string weather_descriptor_strings[] =
{
    STRINGIFY(none),
    STRINGIFY(shallow),
    STRINGIFY(partial),
    STRINGIFY(patches),
    STRINGIFY(low_drifting),
    STRINGIFY(blowing),
    STRINGIFY(showers),
    STRINGIFY(thunderstorm),
    STRINGIFY(freezing)
};

std::string weather_phenomena_strings[] =
{
    STRINGIFY(none),

    STRINGIFY(drizzle),
    STRINGIFY(rain),
    STRINGIFY(snow),
    STRINGIFY(snow_grains),
    STRINGIFY(ice_crystals),
    STRINGIFY(ice_pellets),
    STRINGIFY(hail),
    STRINGIFY(small_hail),
    STRINGIFY(unknown_precipitation),

    STRINGIFY(mist),
    STRINGIFY(fog),
    STRINGIFY(smoke),
    STRINGIFY(volcanic_ash),
    STRINGIFY(widespread_dust),
    STRINGIFY(sand),
    STRINGIFY(haze),
    STRINGIFY(spray),

    STRINGIFY(well_developed_dust_whirls),
    STRINGIFY(squalls),
    STRINGIFY(funnel_cloud_tornado_waterspout),
    STRINGIFY(sandstorm),
    STRINGIFY(duststorm)
};

std::string sky_cover_type_strings[] =
{
    STRINGIFY(vertical_visibility),
    STRINGIFY(sky_clear),
    STRINGIFY(clear_below_12000),
    STRINGIFY(few),
    STRINGIFY(scattered),
    STRINGIFY(broken),
    STRINGIFY(overcast)
};

std::string sky_cover_cloud_type_strings[] =
{
    STRINGIFY(unspecified),
    STRINGIFY(none),
    STRINGIFY(cumulonimbus),
    STRINGIFY(towering_cumulus)
};

std::string flight_category_strings[] =
{
    STRINGIFY(vfr),
    STRINGIFY(mvfr),
    STRINGIFY(ifr),
    STRINGIFY(lifr),
    STRINGIFY(unknown)
};

std::string speed_unit_strings[] =
{
    STRINGIFY(kt),
    STRINGIFY(mph)
};

std::string distance_unit_strings[] =
{
    STRINGIFY(feet),
    STRINGIFY(metres),
    STRINGIFY(statute_miles),
    STRINGIFY(nautical_miles)
};

std::string pressure_unit_strings[] =
{
    STRINGIFY(hPa),
    STRINGIFY(inHg)
};

template <class TEnum>
size_t etoi(TEnum e)
{
    return static_cast<size_t>(e);
}

} // namespace

//-----------------------------------------------------------------------------

namespace aw
{
namespace test
{

//-----------------------------------------------------------------------------

TEST_CLASS(MetarValidationTests)
{
public:
    TEST_CLASS_INITIALIZE(MetarValidationTests_Init);

    TEST_METHOD(METAR_Validation);

private:
    void ValidateReportType         (aw::metar const& metar, basic_json<> const& test);
    void ValidateStationIdentifier  (aw::metar const& metar, basic_json<> const& test);
    void ValidateObservationTime    (aw::metar const& metar, basic_json<> const& test);
    void ValidateReportModifier     (aw::metar const& metar, basic_json<> const& test);
    void ValidateWind               (aw::metar const& metar, basic_json<> const& test);
    void ValidateVisibility         (aw::metar const& metar, basic_json<> const& test);
    void ValidateRunwayVisualRange  (aw::metar const& metar, basic_json<> const& test);
    void ValidateWeather            (aw::metar const& metar, basic_json<> const& test);
    void ValidateSkyCondition       (aw::metar const& metar, basic_json<> const& test);
    void ValidateTemperatureDewpoint(aw::metar const& metar, basic_json<> const& test);
    void ValidateAltimeter          (aw::metar const& metar, basic_json<> const& test);
    void ValidateRemarks            (aw::metar const& metar, basic_json<> const& test);

private:
    static basic_json<> m_expectationFile;
};

//-----------------------------------------------------------------------------

void MetarValidationTests::MetarValidationTests_Init()
{
    std::wstring resourcesPath(RESOURCES_DIR);
    std::wstring expectationFile = L"metar.json";

    std::ifstream stream(resourcesPath + expectationFile);
    std::stringstream buffer;
    buffer << stream.rdbuf();

    try
    {
        m_expectationFile = json::parse(buffer.str());
    }
    catch (std::exception const&)
    {
        wchar_t buf[256];
        swprintf_s(
            buf, sizeof(buffer),
            L"Failed to parse JSON expectation file '%ws'. Try to validate the file using JSONLint.",
            expectationFile.data()
        );
        Assert::Fail(buf);
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::METAR_Validation()
{
    Assert::AreEqual(std::string("METAR"), m_expectationFile["module"].get<std::string>(), L"Expectation file is not valid for this test.");

    auto tests = m_expectationFile["tests"];
    for (auto test : tests)
    {
        if (test.find("broken") != test.end())
        {
            continue;
        }

        aw::metar metar(test["string"].get<std::string>());

        ValidateReportType(metar, test);
        ValidateStationIdentifier(metar, test);
        ValidateObservationTime(metar, test);
        ValidateReportModifier(metar, test);
        ValidateWind(metar, test);
        ValidateVisibility(metar, test);
        ValidateRunwayVisualRange(metar, test);
        ValidateWeather(metar, test);
        ValidateSkyCondition(metar, test);
        ValidateTemperatureDewpoint(metar, test);
        ValidateAltimeter(metar, test);
        ValidateRemarks(metar, test);
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateReportType(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("report_type") != test.end())
    {
        Assert::AreEqual(test["report_type"].get<std::string>(), metar_report_type_strings[etoi(metar.type)]);
    }
    else
    {
        Assert::AreEqual(std::string("metar"), metar_report_type_strings[etoi(metar.type)]);
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateStationIdentifier(aw::metar const& metar, basic_json<> const& test)
{
    Assert::IsTrue(test.find("station_identifier") != test.end());
    Assert::AreEqual(test["station_identifier"].get<std::string>(), metar.identifier);
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateObservationTime(aw::metar const& metar, basic_json<> const& test)
{
    Assert::IsTrue(test.find("observation_time") != test.end());

    auto observation_time = test["observation_time"];
    Assert::AreEqual(observation_time["day_of_month"].get<uint8_t>(), metar.report_time.day_of_month);
    Assert::AreEqual(observation_time["hour_of_day"].get<uint8_t>(), metar.report_time.hour_of_day);
    Assert::AreEqual(observation_time["minute_of_hour"].get<uint8_t>(), metar.report_time.minute_of_hour);
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateReportModifier(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("report_modifier") != test.end())
    {
        Assert::AreEqual(test["report_modifier"].get<std::string>(), metar_modifier_type_strings[etoi(metar.modifier)]);
    }
    else
    {
        Assert::AreEqual(std::string("none"), metar_modifier_type_strings[etoi(metar.modifier)]);
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateWind(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("wind") != test.end())
    {
        auto wind = test["wind"];
        Assert::AreEqual(wind["unit"].get<std::string>(), speed_unit_strings[etoi(metar.wind_group->unit)]);
        Assert::AreEqual(wind["direction"].get<uint16_t>(), metar.wind_group->direction);
        Assert::AreEqual(wind["wind_speed"].get<uint8_t>(), metar.wind_group->wind_speed);

        if (wind.find("gust_speed") != wind.end())
        {
            Assert::AreEqual(wind["gust_speed"].get<uint8_t>(), metar.wind_group->gust_speed);
        }
        else
        {
            Assert::AreEqual(uint8_t(0), metar.wind_group->gust_speed);
        }

        if (wind.find("variation_lower") != wind.end() && wind.find("variation_upper") != wind.end())
        {
            Assert::AreEqual(wind["variation_lower"].get<uint16_t>(), *(metar.wind_group->variation_lower));
            Assert::AreEqual(wind["variation_upper"].get<uint16_t>(), *(metar.wind_group->variation_upper));
        }
        else
        {
            Assert::IsFalse(static_cast<bool>(metar.wind_group->variation_lower));
            Assert::IsFalse(static_cast<bool>(metar.wind_group->variation_upper));
        }
    }
    else
    {
        Assert::IsFalse(static_cast<bool>(metar.wind_group));
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateVisibility(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("visibility") != test.end())
    {
        auto visibility = test["visibility"];
        Assert::AreEqual(visibility["unit"].get<std::string>(), distance_unit_strings[etoi(metar.visibility_group->unit)]);
        Assert::AreEqual(visibility["distance"].get<double>(), metar.visibility_group->distance, 0.001);

        if (visibility.find("modifier") != visibility.end())
        {
            Assert::AreEqual(visibility["modifier"].get<std::string>(), visibility_modifier_type_strings[etoi(metar.visibility_group->modifier)]);
        }
        else
        {
            Assert::AreEqual(aw::visibility_modifier_type::none, metar.visibility_group->modifier);
        }
    }
    else
    {
        Assert::IsFalse(static_cast<bool>(metar.visibility_group));
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateRunwayVisualRange(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("runway_visual_range") != test.end())
    {
        auto rvrGroup = test["runway_visual_range"];
        Assert::AreEqual(rvrGroup.size(), metar.runway_visual_range_group.size());

        for (size_t i = 0; i < rvrGroup.size(); ++i)
        {
            auto expected = rvrGroup.at(i);
            auto actual = metar.runway_visual_range_group.at(i);

            Assert::AreEqual(expected["unit"].get<std::string>(), distance_unit_strings[etoi(actual.visibility_min.unit)]);
            Assert::AreEqual(expected["unit"].get<std::string>(), distance_unit_strings[etoi(actual.visibility_max.unit)]);
            Assert::AreEqual(expected["runway_number"].get<uint8_t>(), actual.runway_number);
            Assert::AreEqual(static_cast<double>(expected["visibility_min"].get<uint16_t>()), actual.visibility_min.distance);

            if (expected.find("runway_designator") != expected.end())
            {
                Assert::AreEqual(expected["runway_designator"].get<std::string>(), runway_designator_type_strings[etoi(actual.runway_designator)]);
            }
            else
            {
                Assert::AreEqual(runway_designator_type::none, actual.runway_designator);
            }

            if (expected.find("visibility_max") != expected.end())
            {
                Assert::AreEqual(static_cast<double>(expected["visibility_max"].get<uint16_t>()), actual.visibility_max.distance);
            }
            else
            {
                Assert::AreEqual(actual.visibility_min, actual.visibility_max);
            }

            if (expected.find("visibility_min_modifier") != expected.end())
            {
                Assert::AreEqual(expected["visibility_min_modifier"].get<std::string>(), visibility_modifier_type_strings[etoi(actual.visibility_min.modifier)]);
            }
            else
            {
                Assert::AreEqual(aw::visibility_modifier_type::none, actual.visibility_min.modifier);
            }

            if (expected.find("visibility_max_modifier") != expected.end())
            {
                Assert::AreEqual(expected["visibility_max_modifier"].get<std::string>(), visibility_modifier_type_strings[etoi(actual.visibility_max.modifier)]);
            }
            else
            {
                Assert::AreEqual(aw::visibility_modifier_type::none, actual.visibility_max.modifier);
            }
        }
    }
    else
    {
        Assert::AreEqual(size_t(0), metar.runway_visual_range_group.size());
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateWeather(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("weather") != test.end())
    {
        auto weatherGroup = test["weather"];
        Assert::AreEqual(weatherGroup.size(), metar.weather_group.size());

        for (size_t i = 0; i < weatherGroup.size(); ++i)
        {
            auto expectedWeather = weatherGroup.at(i);
            auto actualWeather = metar.weather_group.at(i);

            if (expectedWeather.find("intensity") != expectedWeather.end())
            {
                Assert::AreEqual(expectedWeather["intensity"].get<std::string>(), weather_intensity_strings[etoi(actualWeather.intensity)]);
            }
            else
            {
                Assert::AreEqual(aw::weather_intensity::moderate, actualWeather.intensity);
            }

            if (expectedWeather.find("descriptor") != expectedWeather.end())
            {
                Assert::AreEqual(expectedWeather["descriptor"].get<std::string>(), weather_descriptor_strings[etoi(actualWeather.descriptor)]);
            }
            else
            {
                Assert::AreEqual(aw::weather_descriptor::none, actualWeather.descriptor);
            }

            if (expectedWeather.find("phenomena") != expectedWeather.end())
            {
                auto phenomena = expectedWeather["phenomena"];
                Assert::AreEqual(phenomena.size(), actualWeather.phenomena.size());

                for (size_t j = 0; j < phenomena.size(); ++j)
                {
                    auto expectationPhenomenon = phenomena.at(j);
                    auto actualPhenomenon = actualWeather.phenomena.at(j);

                    Assert::AreEqual(expectationPhenomenon.get<std::string>(), weather_phenomena_strings[etoi(actualPhenomenon)]);
                }
            }
            else
            {
                Assert::AreEqual(size_t(0), actualWeather.phenomena.size());
            }
        }
    }
    else
    {
        Assert::AreEqual(size_t(0), metar.weather_group.size());
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateSkyCondition(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("sky_condition") != test.end())
    {
        auto skyConditionGroup = test["sky_condition"];
        Assert::AreEqual(skyConditionGroup.size(), metar.sky_condition_group.size());

        for (size_t i = 0; i < skyConditionGroup.size(); ++i)
        {
            auto expectedSkyCondition = skyConditionGroup.at(i);
            auto actualSkyCondition = metar.sky_condition_group.at(i);
            bool noClouds = false;

            if (expectedSkyCondition.find("unit") != expectedSkyCondition.end())
            {
                Assert::AreEqual(expectedSkyCondition["unit"].get<std::string>(), distance_unit_strings[etoi(actualSkyCondition.unit)]);
            }
            else
            {
                Assert::AreEqual(aw::distance_unit::feet, actualSkyCondition.unit);
            }

            if (expectedSkyCondition.find("sky_cover") != expectedSkyCondition.end())
            {
                Assert::AreEqual(expectedSkyCondition["sky_cover"].get<std::string>(), sky_cover_type_strings[etoi(actualSkyCondition.sky_cover)]);

                // If CLR or SKC are specified, we have a sky_cover_cloud_type of 'none' so we need to track this here.
                noClouds = (
                    actualSkyCondition.sky_cover == aw::sky_cover_type::clear_below_12000 ||
                    actualSkyCondition.sky_cover == aw::sky_cover_type::sky_clear
                );
            }
            else
            {
                Assert::AreEqual(aw::sky_cover_type::sky_clear, actualSkyCondition.sky_cover);
            }

            if (expectedSkyCondition.find("layer_height") != expectedSkyCondition.end())
            {
                Assert::AreEqual(expectedSkyCondition["layer_height"].get<uint32_t>(), actualSkyCondition.layer_height);
            }
            else
            {
                Assert::AreEqual(UINT32_MAX, actualSkyCondition.layer_height);
            }

            if (expectedSkyCondition.find("cloud_type") != expectedSkyCondition.end())
            {
                Assert::AreEqual(expectedSkyCondition["cloud_type"].get<std::string>(), sky_cover_cloud_type_strings[etoi(actualSkyCondition.cloud_type)]);
            }
            else
            {
                auto expectedCover = noClouds ? aw::sky_cover_cloud_type::none : aw::sky_cover_cloud_type::unspecified;
                Assert::AreEqual(expectedCover, actualSkyCondition.cloud_type);
            }
        }
    }
    else
    {
        Assert::AreEqual(size_t(0), metar.sky_condition_group.size());
    }
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateTemperatureDewpoint(aw::metar const& metar, basic_json<> const& test)
{
    // TODO: Check that we actually have temperature and dewpoint values in the expectation file
    Assert::AreEqual(test["temperature_dewpoint"]["temperature"].get<int8_t>(), *(metar.temperature));
    Assert::AreEqual(test["temperature_dewpoint"]["dewpoint"].get<int8_t>(), *(metar.dewpoint));
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateAltimeter(aw::metar const& metar, basic_json<> const& test)
{
    // TODO: Check that we actually have the altimeter value in the expectation file
    Assert::AreEqual(test["altimeter"]["unit"].get<std::string>(), pressure_unit_strings[etoi(metar.altimeter_group->unit)]);
    Assert::AreEqual(test["altimeter"]["pressure"].get<double>(), metar.altimeter_group->pressure);
}

//-----------------------------------------------------------------------------

void MetarValidationTests::ValidateRemarks(aw::metar const& metar, basic_json<> const& test)
{
    if (test.find("remarks") != test.end())
    {
        Assert::IsTrue(!metar.remarks.empty() == test["remarks"].get<bool>());
    }
    else
    {
        Assert::AreEqual(size_t(0), metar.remarks.size());
    }
}

//-----------------------------------------------------------------------------

basic_json<> MetarValidationTests::m_expectationFile;

//-----------------------------------------------------------------------------

} // namespace test
} // namespace aw
