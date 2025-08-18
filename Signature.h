#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "DBCache.h"


namespace HY{

class Signature {
public:




	// Ŀ������
	double current_RCS = 0.0;

	double current_IR = 0.0;

	double current_ESM = 0.0;

	double current_ECM = 0.0;




private:

	RCSCache cache;

	//std::vector<RCSRecord> records_RCS; // RCS
	//std::vector<RCSRecord> records; // IR
	//std::vector<RCSRecord> records; // ESM
	//std::vector<RCSRecord> records; // ECM


};

}



