#include "Signature.h"
#include <cmath>

double HY::Signature::GetRCS(POSITION Pos, double maxRange, double minAz, double maxAz, double minEle, double maxEle, double freq, polarization pol, std::string platformName, POSITION myPos)
{
	/*判断是否进入探测方视场*/
	const GeoCoord GeoPos = { Pos.lat,Pos.lon,Pos.alt};
	const GeoCoord myGeoPos = { myPos.lat,myPos.lon,myPos.alt };
	const FOV fov = { minAz ,maxAz ,minEle,maxEle,maxRange };
	if (InFOV(GeoPos, Pos.heading, Pos.pitch, fov, myGeoPos)) {
		return 1;
	}
	return 0;
}
