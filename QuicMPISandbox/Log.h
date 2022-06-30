#pragma once

#include <string>
#include <fstream>

#include <memory>

class Logger
{
public:
	Logger();
	virtual ~Logger();

	bool IsValid() { return NULL != m_pStream; }

	void OpenLog( std::string zFilePath );
	bool AddToLog( std::string zMsg );

	static void S_Log( std::string zMsg );

private:
	std::ofstream* m_pStream;

};
