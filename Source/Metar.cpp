/**
* Module:      Metar.cpp
* Author(s):   Steven Frost
* Description: This module implements the METAR class used to download and/or process METAR
*              data from reference (4).
*              This document currently complies with North-American standards ONLY and
*              may not successfully decode International METAR information.
*
* References:  (1) http://www.ofcm.gov/fmh-1/pdf/L-CH12.pdf
*              (2) http://en.wikipedia.org/wiki/METAR
*              (3) http://www.herk-gouge.com/2006/11/understanding-runway-visual-range-rvr.html
*              (4) http://www.aviationweather.gov/adds/dataserver
*
* Todo:        (1) Implement structures for International standards
*/

#include "stdafx.hpp"
#include "Metar.hpp"

namespace Meteorology {
	const string Metar::m_Patterns[] = {
		"^(METAR|SPECI) ",
		"(^| )([A-Z]{4}) ",
		" ([0-9]{2})([0-9]{2})([0-9]{2})Z ",
		" (AUTO|COR) ",
		" ([0-9]{3}|VRB)([0-9]{2,3})(G([0-9]{2,3}))?(KT|MPS)( ([0-9]{3})V([0-9]{3}))? ",
		" (CAVOK|((M?([12]?)[ ]?([0-9])/([0-9]))|([0-9]{1,5}))(SM)?) ",
		" R[0-9]{2}[LRC]?/[0-9]{4}(V[0-9]{4})?FT ",
		" [+-]?[A-Z]{2}([A-Z]{2}){1,3} ",
		" ((SKC|CLR)|(((VV)|([A-Z]{2,3}))([0-9]{3}|///))) ",
		" (M)?[0-9]{2}/",
		"/(M)?[0-9]{2} ",
		" A[0-9]{4} ",
		" RMK (PK WND [0-9]{5,6}/([0-9]{2}|[0-9]{4}))?.*$"
	};

	Metar::Metar(void) {
		initialise();
	}

	Metar::Metar(string metar) {
		initialise();
		setMetarString(metar);
		processMetar();
	}

	void Metar::initialise() {
		m_MetarInfo = new MetarInfo;
	}

	Metar::~Metar(void) {
		delete(m_MetarInfo);
	}

	string Metar::getMetarString() {
		return m_Metar;
	}

	void Metar::setMetarString(string metar) {
		m_Metar = metar;
		processMetar();
	}

	void Metar::getMetarStruct(MetarInfo &dest) {
		dest = (*m_MetarInfo);
	}

	void Metar::processMetar() {
		for (unsigned int i = 0; i < m_Patterns->size(); i++) {
			regex regexp(m_Patterns[i].c_str());
			cmatch result;

			if (regex_search(m_Metar.c_str(), result, regexp)) {
				processMetarElement((MetarElement)i, result);
			}
		}
	}

	void Metar::processMetarElement(MetarElement elem, cmatch metar) {
		switch (elem) {
		case METAR_ELEMENT_REPORT_TYPE:
			processMetarElementReportType(metar);
			break;
		case METAR_ELEMENT_STATION_IDENTIFIER:
			processMetarElementStationIdentifier(metar);
			break;
		case METAR_ELEMENT_OBSERVATION_TIME:
			processMetarElementObservationTime(metar);
			break;
		case METAR_ELEMENT_REPORT_MODIFIER:
			processMetarElementReportModifier(metar);
			break;
		case METAR_ELEMENT_WIND:
			processMetarElementWind(metar);
			break;
		case METAR_ELEMENT_VISIBILITY:
			processMetarElementVisiblity(metar);
			break;
		case METAR_ELEMENT_RUNWAY_VISUAL_RANGE:
			processMetarElementRunwayVisualRange(metar);
			break;
		case METAR_ELEMENT_WEATHER_GROUP:
			processMetarElementWeather(metar);
			break;
		case METAR_ELEMENT_SKY_CONDITION:
			processMetarElementSkyCondition(metar);
			break;
		case METAR_ELEMENT_TEMPERATURE:
			processMetarElementTemperature(metar);
			break;
		case METAR_ELEMENT_DEWPOINT:
			processMetarElementDewpoint(metar);
			break;
		case METAR_ELEMENT_ALTIMETER:
			processMetarElementAltimeter(metar);
			break;
		case METAR_ELEMENT_REMARKS:
			processMetarElementRemarks(metar);
			break;
		}
	}

	void Metar::processMetarElementReportType(cmatch metar) {
		m_MetarInfo->reportType = (metar.str(1) == "METAR" ? METAR_REPORT_TYPE_METAR : METAR_REPORT_TYPE_SPECI);
	}

	void Metar::processMetarElementStationIdentifier(cmatch metar) {
		strncpy(m_MetarInfo->stationIdentifier, metar.str(2).c_str() + '\0', 5);
	}

	void Metar::processMetarElementObservationTime(cmatch metar) {
		MetarObservationTime *observationTime = new MetarObservationTime;

		observationTime->dayOfMonth = atoi(metar.str(1).c_str());
		observationTime->hourOfDay = atoi(metar.str(2).c_str());
		observationTime->minuteOfHour = atoi(metar.str(3).c_str());

		m_MetarInfo->observationTime = observationTime;
	}

	void Metar::processMetarElementReportModifier(cmatch metar) {
		m_MetarInfo->modifier = (metar[1] == "AUTO" ? METAR_MODIFIER_AUTO : METAR_MODIFIER_COR);
	}

	void Metar::processMetarElementWind(cmatch metar) {
		MetarWind *wind = new MetarWind;

		wind->direction = atoi(metar.str(1).c_str());
		wind->speed = atoi(metar.str(2).c_str());
		wind->gustSpeed = ((metar[3].matched) ? (atoi(metar.str(4).c_str())) : 0);
		wind->unit = metar[5];

		if (metar[6].matched) {
			wind->variationLower = atoi(metar.str(7).c_str());
			wind->variationUpper = atoi(metar.str(8).c_str());
		} else {
			wind->variationLower = 0;
			wind->variationUpper = 0;
		}

		m_MetarInfo->wind = wind;
	}

	void Metar::processMetarElementVisiblity(cmatch metar) {
		if (metar.str(1) == "CAVOK") {
			m_MetarInfo->visibility = (double)9999;
		} else {
			if (metar[3].matched) {
				m_MetarInfo->visibility = (metar.length(4) ? (atof(metar.str(4).c_str())) : (double)0);
				m_MetarInfo->visibility += atof(metar.str(5).c_str()) / atof(metar.str(6).c_str());
			} else {
				m_MetarInfo->visibility = atof(metar.str(7).c_str());
			}

			if (metar[8].matched) {
				m_MetarInfo->visibility *= 1609.344;
			}
		}
	}

	void Metar::processMetarElementRunwayVisualRange(cmatch metar) {

	}

	void Metar::processMetarElementWeather(cmatch metar) {

	}

	void Metar::processMetarElementSkyCondition(cmatch metar) {

	}

	void Metar::processMetarElementTemperature(cmatch metar) {

	}

	void Metar::processMetarElementDewpoint(cmatch metar) {

	}

	void Metar::processMetarElementAltimeter(cmatch metar) {

	}

	void Metar::processMetarElementRemarks(cmatch metar) {

	}
}