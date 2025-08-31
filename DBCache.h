#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mysql.h>

namespace HY {

	enum polarization
	{
		HH,  // ˮƽ-ˮƽ
		HV   // ˮƽ-��ֱ
	};


	struct RCSRecord {
		std::string aircraft_name;
		double frequency;      // GHz
		std::string polarization; // "HH" or "VV"
		double azimuth;        // deg
		double elevation;      // deg
		double rcs_value;   // dBsm

		RCSRecord(std::string name, double freq, std::string pol,
			double az, double el, double rcs)
			: aircraft_name(std::move(name)), frequency(freq),
			polarization(std::move(pol)), azimuth(az),
			elevation(el), rcs_value(rcs) {
		}
	};





	class RCSCache {
	private:
		std::vector<RCSRecord> records; // 
		
		//std::unordered_map<std::string, std::vector<const RCSRecord*>> name_index;


	public:

		std::unordered_map<std::string, std::vector<HY::RCSRecord>> name_index;

		bool loadFromDB(const std::string& host, const std::string& user, const std::string& password, const std::string& database);


		std::vector<HY::RCSRecord> findByAircraft(const std::string& name);


	};






}