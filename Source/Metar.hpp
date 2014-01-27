/****************************** Module Header ******************************\
Module Name:  Metar.hpp
Project:      Meteorology
Copyright (c) 2014 Steven Frost.

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

#include <ctime>
#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "Meteorology.hpp"
using namespace std;

#ifndef METEOROLOGY_METAR_H
#define METEOROLOGY_METAR_H

namespace Meteorology {
	class Metar {
	public:
		/**
		* Enumerated list of internally supported METAR elements
		*/
		typedef enum MetarElement {
			METAR_ELEMENT_REPORT_TYPE,
			METAR_ELEMENT_STATION_IDENTIFIER,
			METAR_ELEMENT_OBSERVATION_TIME,
			METAR_ELEMENT_REPORT_MODIFIER,
			METAR_ELEMENT_WIND,
			METAR_ELEMENT_VISIBILITY,
			METAR_ELEMENT_RUNWAY_VISUAL_RANGE,
			METAR_ELEMENT_WEATHER_GROUP,
			METAR_ELEMENT_SKY_CONDITION,
			METAR_ELEMENT_TEMPERATURE,
			METAR_ELEMENT_DEWPOINT,
			METAR_ELEMENT_ALTIMETER,
			METAR_ELEMENT_REMARKS
		};

		/**
		* Enumerated list of METAR report types
		*/
		typedef enum MetarReportType {
			METAR_REPORT_TYPE_METAR,
			METAR_REPORT_TYPE_SPECI
		};

		/**
		* METAR Station Identifier
		*/
		typedef string StationIdentifier;

		/**
		* Enumerated list of METAR modifiers, specifying wether the report was
		* modified before release.
		*/
		typedef enum MetarModifier {
			METAR_MODIFIER_AUTO,
			METAR_MODIFIER_COR
		};

		/**
		* The day of month, hour of day and minute of hour the METAR report was
		* compiled (times are UTC/Zulu).
		*/
		typedef struct MetarObservationTime {
			unsigned int dayOfMonth;
			unsigned int hourOfDay;
			unsigned int minuteOfHour;

			MetarObservationTime() {
				time_t t = time(0);
				struct tm *now = localtime(&t);
				dayOfMonth = now->tm_mon;
				hourOfDay = now->tm_hour;
				minuteOfHour = now->tm_min;
			}
		};

		/**
		* Data structure containing the direction, speed, gust and variance of
		* the wind, along with the unit of speed measurement.
		* Any unknown or default quantities are represented by NULL.
		*/
		typedef struct MetarWind {
			string unit;
			unsigned int direction;
			unsigned int speed;
			unsigned int gustSpeed;
			unsigned int variationLower;
			unsigned int variationUpper;

			MetarWind() : unit("KT"),
				direction(NULL),
				speed(NULL),
				gustSpeed(NULL),
				variationLower(NULL),
				variationUpper(NULL) {
			};
		};

		/**
		* Data structure that contains details of the runway visual range for
		* one end of a runway.
		* Any unknown quantity is represented by NULL.
		*/
		typedef struct MetarRunwayVisualRange {
			string unit;
			int runwayNum;
			char designator;
			int visibility;
			int visibilityVariation;

			MetarRunwayVisualRange() : unit("KT"),
				runwayNum(NULL),
				designator('C'),
				visibility(MAX_VISIBILITY),
				visibilityVariation(NULL) {
			};
		};

		/**
		* Data structure holding a single METAR weather group. Intensity,
		* descriptor and phenomena are contained in this structure.
		*/
		typedef struct MetarWeatherGroup {
			WeatherIntensity intensity;
			WeatherDescriptor descriptor;
			list<WeatherPhenomena> *type;

			MetarWeatherGroup() : intensity(WI_NONE),
				descriptor(WD_NONE),
				type(new list<WeatherPhenomena>) {
			};

			~MetarWeatherGroup() {
				delete(type);
			}
		};

		/**
		* Data structure holding the sky cover grouped with the height of the
		* layer.
		* The height value is NULL if the cloud layer is below the station
		* reporting the METAR.
		*/
		typedef struct MetarSkyConditionGroup {
			SkyCover skyCover;
			unsigned int height;
			SkyCoverClouds cloudType;

			MetarSkyConditionGroup() : skyCover(SC_CLEAR),
				height(NULL),
				cloudType(SCC_NONE) {
			};
		};

		/**
		* Data structure holding the individual native data elements in the
		* METAR report.
		* The actual instance of this structure is private to the class and
		* elements can be accessed via getters
		*/
		typedef struct MetarInfo {
			MetarReportType reportType;
			StationIdentifier stationIdentifier;
			MetarObservationTime *observationTime;
			MetarModifier modifier;
			MetarWind *wind;
			Visibility visibility;
			list<MetarRunwayVisualRange> *const runwayVisualRange;
			list<MetarWeatherGroup> *const weather;
			list<MetarSkyConditionGroup> *const skyCondition;
			Temperature temperature;
			Dewpoint dewpoint;
			Altimeter altimeter;
			string remarks;

			MetarInfo(void) : reportType(METAR_REPORT_TYPE_METAR),
				observationTime(new MetarObservationTime),
				modifier(METAR_MODIFIER_AUTO),
				wind(new MetarWind),
				visibility(MAX_VISIBILITY),
				runwayVisualRange(new list<MetarRunwayVisualRange>),
				weather(new list<MetarWeatherGroup>),
				skyCondition(new list<MetarSkyConditionGroup>),
				temperature(NAN),
				dewpoint(NAN),
				altimeter(NAN) {
			};

			~MetarInfo() {
				stationIdentifier.clear();
				runwayVisualRange->clear();
				weather->clear();
				skyCondition->clear();
				remarks.clear();

				delete(observationTime, wind, runwayVisualRange, weather, skyCondition);
			}
		};

		Metar(void) : m_MetarInfo(new MetarInfo) {}
		Metar(string metar);
		~Metar(void);

		MetarReportType                     getReportType()        { return m_MetarInfo->reportType; }
		StationIdentifier                   getStationIdentifier() { return m_MetarInfo->stationIdentifier; }
		MetarObservationTime *              getObservationTime()   { return m_MetarInfo->observationTime; }
		MetarModifier                       getModifier()          { return m_MetarInfo->modifier; }
		MetarWind *                         getWind()              { return m_MetarInfo->wind; }
		Visibility                          getVisibilityF()       { return m_MetarInfo->visibility * METRES_TO_FEET; }
		Visibility                          getVisibilityM()       { return m_MetarInfo->visibility; }
		list<MetarRunwayVisualRange> *const getRunwayVisualRange() { return m_MetarInfo->runwayVisualRange; }
		list<MetarWeatherGroup> *const      getWeather()           { return m_MetarInfo->weather; }
		list<MetarSkyConditionGroup> *const getSkyCondition()      { return m_MetarInfo->skyCondition; }
		Temperature                         getTemperatureC()      { return m_MetarInfo->temperature; }
		Temperature                         getTemperatureF()      { return m_MetarInfo->temperature * CENT_TO_FAR; }
		Dewpoint                            getDewpointC()         { return m_MetarInfo->dewpoint; }
		Dewpoint                            getDewpointF()         { return m_MetarInfo->dewpoint * CENT_TO_FAR; }
		Altimeter                           getAltimeterinHg()     { return m_MetarInfo->altimeter; }
		Altimeter                           getAltimeterhPa()      { return m_MetarInfo->altimeter * INHG_TO_HPA; }
		string                              getRemarks()           { return m_MetarInfo->remarks; }
		string                              getMetarString()       { return m_Metar; processMetar(); }

		void                                setMetarString(string metar) { m_Metar = metar; }
	private:
		string m_Metar;                     /* Full METAR string */
		MetarInfo *const m_MetarInfo;       /* Main Metar Info structure holds all native data */
		static const string m_Patterns[];   /* List of Regular Expressions to obtain native METAR data */

		void initialise();
		void processMetar();
		void processMetarElement(MetarElement elem, cmatch result);

		void setReportType(MetarReportType type);
		void setReportType(string type);
		void setStationIdentifier(string ident);
		void setObservationTime(MetarObservationTime time);
		void setObservationTime(unsigned int day, unsigned int hour, unsigned int minute);
		void setReportModifier(MetarModifier modifier);
		
		void processMetarElementReportType(cmatch metar);
		void processMetarElementStationIdentifier(cmatch metar);
		void processMetarElementObservationTime(cmatch metar);
		void processMetarElementReportModifier(cmatch metar);
		void processMetarElementWind(cmatch metar);
		void processMetarElementVisiblity(cmatch metar);
		void processMetarElementRunwayVisualRange(cmatch metar);
		void processMetarElementWeather(cmatch metar);
		void processMetarElementSkyCondition(cmatch metar);
		void processMetarElementTemperature(cmatch metar);
		void processMetarElementDewpoint(cmatch metar);
		void processMetarElementAltimeter(cmatch metar);
		void processMetarElementRemarks(cmatch metar);
	};
}

#endif // METEOROLOGY_METAR_H