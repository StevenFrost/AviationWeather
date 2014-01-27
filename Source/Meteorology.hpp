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

#ifndef METEORLOGY_H
#define METEORLOGY_H

namespace Meteorology {
	typedef double Altimeter;
	typedef double Dewpoint;
	typedef double Visibility;
	typedef double Temperature;

	static const double CENT_TO_FAR = 33.8;
	static const double METRES_TO_FEET = 3.2808399;
	static const double INHG_TO_HPA = 33.8638816;
	static const int MAX_VISIBILITY = 9999;

	/**
	* WeatherIntensity
	*   Enumerated list of weather intesity options. Each element is
	*   denoted by `-`, ``, `+` and `VC` respectively.
	*
	* See: Reference (1), Table 12-2
	*/
	typedef enum WeatherIntensity {
		WI_NONE,
		WI_LIGHT,
		WI_MODERATE,
		WI_HEAVY,
		WI_IN_VICINITY
	};

	/**
	* WeatherDescriptor
	*   Description of the present weather in a weather group. Only one
	*   element can be active in a weather group.
	*
	* See: Reference (1), Table 12-2
	*/
	typedef enum WeatherDescriptor {
		WD_NONE,
		WD_SHALLOW,
		WD_PARTIAL,
		WD_PATCHES,
		WD_LOW_DRIFTING,
		WD_BLOWING,
		WD_SHOWERS,
		WD_THUNDERSTORM,
		WD_FREEZING
	};

	/**
	* WeatherPhenomena
	*   Enumerated list of valid weather phenomena represented in METAR
	*   reports. Up to three can be used in one weather group.
	*
	* See: Reference (1), Table 12-2
	*/
	typedef enum WeatherPhenomena {
		WP_DRIZZLE,
		WP_RAIN,
		WP_SNOW,
		WP_SNOW_GRAINS,
		WP_ICE_CRYSTALS,
		WP_ICE_PELLETS,
		WP_HAIL,
		WP_SMALL_HAIL,
		WP_MIST,
		WP_FOG,
		WP_SMOKE,
		WP_VOLCANIC_ASH,
		WP_WIDESPREAD_DUST,
		WP_SAND,
		WP_HAZE,
		WP_SPRAY,
		WP_WELL_DEVELOPED_DUST_WHIRLS,
		WP_SQUALLS,
		WP_FUNNEL_CLOUD_TORNADO_WATERSPOUT,
		WP_SANDSTORM,
		WP_DUSTSTORM,
		WP_UNKNOWN_PRECIPITATION
	};

	/**
	* SkyCover
	*   Enumerated list of valid cloud layer coverage amounts.
	*
	* See: Reference (1), Table 12-3
	*/
	typedef enum SkyCover {
		SC_VERTICAL_VISIBILITY,
		SC_CLEAR,
		SC_FEW,
		SC_SCATTERED,
		SC_BROKEN,
		SC_OVERCAST
	};

	/**
	* SkyCoverClouds
	*   For manually edited METAR reports, cloud types may be appended
	*   to the Sky Cover group. If this is available, the enumeration
	*   will be set to a value other than NONE.
	*
	* See: Reference (1), Paragraph under Table 12-4
	*/
	typedef enum SkyCoverClouds {
		SCC_NONE,
		SCC_CUMULONIMBUS,
		SCC_TOWERING_CUMULUS
	};
}
#endif // !METEORLOGY_H