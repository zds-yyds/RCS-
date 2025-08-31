#include "Signature.h"
#include <cmath>

double HY::Signature::GetRCS(POSITION Pos, double maxRange, double minAz, double maxAz, double minEle, double maxEle, double freq, polarization pol, std::string platformName, POSITION myPos)
{
	/*判断是否进入探测方视场*/
	const GeoCoord GeoPos = { Pos.lat,Pos.lon,Pos.alt};
	const GeoCoord myGeoPos = { myPos.lat,myPos.lon,myPos.alt };
	const FOV fov = { minAz ,maxAz ,minEle,maxEle,maxRange };
	if (InFOV(GeoPos, Pos.heading, Pos.pitch, fov, myGeoPos)) {
		double relAz_deg = 0, relEl_deg = 0;
		//RelativeAzEl(GeoPos, myGeoPos, myPos.heading, myPos.pitch, relAz_deg, relEl_deg);
		return GetRCS(Pos, freq, pol, platformName, myPos);
	}
	return 0;
}

void HY::Signature::RelativeAzEl(const GeoCoord& observer, const GeoCoord& target, double heading_deg, double pitch_deg, double& relAz_deg, double& relEl_deg)
{
	// 1. 转换到 ECEF
	double ox, oy, oz;
	LLA2ECEF(observer, ox, oy, oz);

	double tx, ty, tz;
	LLA2ECEF(target, tx, ty, tz);

	// 2. ECEF -> ENU (以 target 为原点)
	double ex, ey, ez;
	ECEF2ENU(target, ox, oy, oz, ex, ey, ez);

	// 3. 计算 az / el (ENU 下：az = atan2(East, North))
	double az = atan2(ex, ey) * RAD2DEG;
	double el = atan2(ez, sqrt(ex * ex + ey * ey)) * RAD2DEG;

	// 4. 考虑 target 的姿态
	az -= heading_deg;
	el -= pitch_deg;

	// 5. 归一化到 [-180, 180]
	auto normDeg = [](double a) {
		while (a > 180) a -= 360;
		while (a < -180) a += 360;
		return a;
		};
	relAz_deg = normDeg(az);
	relEl_deg = normDeg(el);
}

inline bool nearlyEqual(double a, double b, double eps = 1e-6) {
	return std::fabs(a - b) < eps;
}

inline std::string polarizationToString(HY::polarization p)
{
	switch (p)
	{
	case HY::polarization::HH: return "HH";
	case HY::polarization::HV: return "HV";
	default: return "Unknown";
	}
}

inline int azIndex(double relAz_deg) {
	if ((relAz_deg >= -180 && relAz_deg < -135) || (relAz_deg > 135 && relAz_deg <= 180))
		return 0; // 后
	else if (relAz_deg >= -135 && relAz_deg < -45)
		return 1; // 左
	else if (relAz_deg >= -45 && relAz_deg <= 45)
		return 2; // 前
	else if (relAz_deg > 45 && relAz_deg <= 135)
		return 3; // 右
	return -1; // 无效
}

inline int elIndex(double relEl_deg) {
	if (relEl_deg >= -90 && relEl_deg < -30) return 0; // 下
	if (relEl_deg >= -30 && relEl_deg < 30)  return 1; // 平
	if (relEl_deg >= 30 && relEl_deg <= 90)  return 2; // 上
	return -1;// 无效
}

inline int findFreqIndex(double freq) {
	double freqBands[] = { 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5 };
	for (int i = 0; i < 11; ++i) {
		if ((i == 0 && freq > 0 && freq <= freqBands[i]) ||
			(i > 0 && freq > freqBands[i - 1] && freq <= freqBands[i]))
			return i + 1; // 返回 1,2,3...
	}
	if (freq > 11.5) return 12;
	return -1; // 无效
}

double HY::Signature::findRCS(std::vector<RCSRecord>& records, const std::string& name, double frequency, const std::string& polarization, double azimuth, double elevation)
{
	for (RCSRecord rec : records) {
		//if (!rec) continue;
		std::string x = rec.aircraft_name;
		if (rec.aircraft_name == name &&
			nearlyEqual(rec.frequency, frequency) &&
			rec.polarization == polarization &&
			nearlyEqual(rec.azimuth, azimuth) &&
			nearlyEqual(rec.elevation, elevation))
		{
			return rec.rcs_value;
		}
	}
	return 0;
}

double HY::Signature::GetRCS(POSITION Pos, double freq, polarization pol, std::string platformName, POSITION myPos)
{
	const GeoCoord GeoPos = { Pos.lat,Pos.lon,Pos.alt };
	const GeoCoord myGeoPos = { myPos.lat,myPos.lon,myPos.alt };
	double relAz_deg = 0, relEl_deg = 0;
	RelativeAzEl(GeoPos, myGeoPos, myPos.heading, myPos.pitch, relAz_deg, relEl_deg);
	if (cache.name_index.find(platformName) != cache.name_index.end()) {
		auto it = cache.name_index.find(platformName);
		if (it != cache.name_index.end() && !it->second.empty()) {
			std::vector<RCSRecord> data_name = it->second;

			int freqIndex = findFreqIndex(freq/1000);
			int az = azIndex(relAz_deg);
			int el = elIndex(relEl_deg);

			if (freqIndex > 0 && az >= 0 && el >= 0) {
				// 对应的固定 az/el/freq 查找
				double azVal[] = { -180, -90, 0, 90 };   // 示例
				double elVal[] = { -90, 0, 90 };

				return findRCS(data_name, platformName, freqIndex, polarizationToString(pol),azVal[az], elVal[el]);
			}

		}
		else {
			std::cout << "数据库未正确读取！ " << " \n" << std::endl;
			return 0;
		}
	}
	else {
		std::cout << "不存在的平台: " << platformName << " \n" << std::endl;
	}
	
	return 0;
}
