/****************************** Module Header ******************************\
Module Name:  Metar.hpp
Project:      Meteorology
Copyright (c) 2015 Steven Frost.

This module defines the METAR class used to download and/or process METAR
data from reference (4). Key structures are also defined to make the
transfer of full decoded METAR information much easier.
This document currently complies with North-American standards ONLY and may
not successfully decode International METAR information.

References:  (1) http://www.ofcm.gov/fmh-1/pdf/L-CH12.pdf
             (2) http://en.wikipedia.org/wiki/METAR
             (3) http://www.herk-gouge.com/2006/11/understanding-runway-visual-range-rvr.html
             (4) http://www.aviationweather.gov/adds/dataserver

Todo:        (1) Implement structures for International standards

This source is subject to the MIT License.
See http://opensource.org/licenses/MIT

All other rights reserved.
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <list>
#include <memory>
#include <string>

#include "Meteorology.hpp"

//-----------------------------------------------------------------------------

namespace Meteorology
{

//-----------------------------------------------------------------------------

enum class MetarElement
{
    ReportType,
    StationIdentifier,
    ObservationTime,
    ReportModifier,
    Wind,
    Visibility,
    RunwayVisualRange,
    WeatherGroup,
    SkyCondition,
    Temperature,
    Dewpoint,
    Altimeter,
    Remarks
};

enum class MetarReportType
{
    Metar,
    Special
};

enum class MetarModifier
{
    Automatic,
    Corrected
};

//-----------------------------------------------------------------------------

class MetarObservationTime
{
public:
    typedef std::shared_ptr<MetarObservationTime> Ptr;

    MetarObservationTime();
    virtual ~MetarObservationTime() {}

    unsigned int GetDayOfMonth()   const { return m_dayOfMonth;   }
    unsigned int GetHourOfDay()    const { return m_hourOfDay;    }
    unsigned int GetMinuteOfHour() const { return m_minuteOfHour; }

protected:
    unsigned int m_dayOfMonth;
    unsigned int m_hourOfDay;
    unsigned int m_minuteOfHour;
};

//-----------------------------------------------------------------------------

class MetarWind
{
public:
    typedef std::shared_ptr<MetarWind> Ptr;

    MetarWind();
    virtual ~MetarWind() {}

    std::string  GetUnit()           const { return m_unit;           }
    unsigned int GetDirection()      const { return m_direction;      }
    unsigned int GetSpeed()          const { return m_speed;          }
    unsigned int GetGustSpeed()      const { return m_gustSpeed;      }
    unsigned int GetVariationLower() const { return m_variationLower; }
    unsigned int GetVariationUpper() const { return m_variationUpper; }

protected:
    std::string  m_unit;
    unsigned int m_direction;
    unsigned int m_speed;
    unsigned int m_gustSpeed;
    unsigned int m_variationLower;
    unsigned int m_variationUpper;
};

//-----------------------------------------------------------------------------

class MetarRunwayVisualRange
{
public:
    typedef std::shared_ptr<MetarRunwayVisualRange> Ptr;

    MetarRunwayVisualRange();
    virtual ~MetarRunwayVisualRange() {}

    std::string  GetUnit()                const { return m_unit;                }
    unsigned int GetRunwayNumber()        const { return m_runwayNumber;        }
    char         GetDesignator()          const { return m_designator;          }
    int          GetVisibility()          const { return m_visibility;          }
    int          GetVisibilityVariation() const { return m_visibilityVariation; }

protected:
    std::string  m_unit;
    unsigned int m_runwayNumber;
    char         m_designator;
    int          m_visibility;
    int          m_visibilityVariation;
};

//-----------------------------------------------------------------------------

class MetarWeatherGroup
{
public:
    typedef std::shared_ptr<MetarWeatherGroup> Ptr;

    MetarWeatherGroup();
    virtual ~MetarWeatherGroup() {}

    WeatherIntensity            GetIntensity()  const { return m_intensity;  }
    WeatherDescriptor           GetDescriptor() const { return m_descriptor; }
    std::list<WeatherPhenomena> GetTypes()      const { return m_types;      }

protected:
    WeatherIntensity            m_intensity;
    WeatherDescriptor           m_descriptor;
    std::list<WeatherPhenomena> m_types;
};

//-----------------------------------------------------------------------------

class MetarSkyConditionGroup
{
public:
    typedef std::shared_ptr<MetarSkyConditionGroup> Ptr;

    MetarSkyConditionGroup();
    virtual ~MetarSkyConditionGroup() {}

    SkyCover       GetSkyCover()  const { return m_skyCover;  }
    unsigned int   GetHeight()    const { return m_height;    }
    SkyCoverClouds GetCloudType() const { return m_cloudType; }

protected:
    SkyCover       m_skyCover;
    unsigned int   m_height;
    SkyCoverClouds m_cloudType;
};

//-----------------------------------------------------------------------------

class Metar
{
public:
    Metar(std::string const& metar);

    MetarReportType                        GetReportType()            const { return m_reportType;            }
    std::string                            GetStationIdentifier()     const { return m_stationIdentifier;     }
    MetarObservationTime::Ptr              GetObservationTime()       const { return m_observationTime;       }
    MetarModifier                          GetModifier()              const { return m_modifier;              }
    MetarWind::Ptr                         GetWind()                  const { return m_wind;                  }
    double                                 GetVisibility()            const { return m_visibility;            }
    std::list<MetarRunwayVisualRange::Ptr> GetRunwayVisualRangeList() const { return m_runwayVisualRangeList; }
    std::list<MetarWeatherGroup::Ptr>      GetWeatherList()           const { return m_weatherList;           }
    std::list<MetarSkyConditionGroup::Ptr> GetSkyConditionList()      const { return m_skyConditionList;      }
    double                                 GetTemperature()           const { return m_temperature;           }
    double                                 GetDewpoint()              const { return m_dewpoint;              }
    double                                 GetAltimeter()             const { return m_altimeter;             }
    std::string                            GetRemarks()               const { return m_remarks;               }

private:
    void Parse();

    void ParseReportType();
    void ParseStationIdentifier();
    void ParseObservationTime();
    void ParseModifier();
    void ParseWind();
    void ParseVisibility();
    void ParseAltimeter();

private:
    std::string m_metar;

    MetarReportType                        m_reportType;
    std::string                            m_stationIdentifier;
    MetarObservationTime::Ptr              m_observationTime;
    MetarModifier                          m_modifier;
    MetarWind::Ptr                         m_wind;
    double                                 m_visibility;
    std::list<MetarRunwayVisualRange::Ptr> m_runwayVisualRangeList;
    std::list<MetarWeatherGroup::Ptr>      m_weatherList;
    std::list<MetarSkyConditionGroup::Ptr> m_skyConditionList;
    double                                 m_temperature;
    double                                 m_dewpoint;
    double                                 m_altimeter;
    std::string                            m_remarks;
};

//-----------------------------------------------------------------------------

} // namespace Meteorology
