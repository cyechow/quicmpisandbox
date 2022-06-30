#pragma once
#include <vector>
#include <unordered_map>

class TimeKeeper
{
public:
	TimeKeeper() {}
	~TimeKeeper() {}

	void StoreTime( std::string zName, double dElapsedMs );

	void SaveToFile( std::string zFilePath );

private:
	std::unordered_map<std::string, std::vector<double>> m_aNamedTimes;
};
