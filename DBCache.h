#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mysqlx/xdevapi.h> // ʹ��MySQL Connector/C++

namespace HY {

// RCS���ݼ�¼�ṹ
struct RCSRecord {
	std::string aircraft_name;
	double frequency;      // GHz
	std::string polarization; // "HH" or "VV"
	int azimuth;        // �Ƕ�
	int elevation;      // �Ƕ�
	double rcs_value;   // dBsm

						// ���캯��
	RCSRecord(std::string name, double freq, std::string pol,
		int az, int el, double rcs)
		: aircraft_name(std::move(name)), frequency(freq),
		polarization(std::move(pol)), azimuth(az),
		elevation(el), rcs_value(rcs) {}
};

// ����Ŀ���������ݽṹ....

// ESM����������ݽ�?...

// ECM����������ݽ�?...



// ���ݻ����� RCS
class RCSCache {
private:
	std::vector<RCSRecord> records; // ���м�¼�洢
									// �����ṹ
	std::unordered_map<std::string, std::vector<const RCSRecord*>> name_index;
	//std::unordered_map<int, std::vector<const RCSRecord*>> freq_index;

public:
	// �����ݿ��������?
	bool loadFromDB(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

	// ��ѯ�ӿ�
	const std::vector<const RCSRecord*>& findByAircraft(const std::string& name);
	//const std::vector<const RCSRecord*>& findByFrequency(int freq);



};

// ���ݻ����� IR


// ���ݻ����� ESM


// ���ݻ����� ECM





}