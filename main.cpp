#include "DBCache.h"
#include <iostream>

using namespace HY;

int main() {
	RCSCache cache;


	if (!cache.loadFromDB("localhost", "root", "123456", "zds_test_rcs")) {
		return 1;
	}


	auto f16_records = cache.findByAircraft("F-16");
	std::cout << "�ҵ� " << f16_records.size() << " ��F-16��¼" << std::endl;


	return 0;
}