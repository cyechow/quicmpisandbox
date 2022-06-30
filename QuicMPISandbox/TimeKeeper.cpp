#include "TimeKeeper.h"

#include "Log.h"
#include <format>

void
TimeKeeper::StoreTime( std::string zName, double dElapsedMs )
{
	Logger::S_Log( std::format( "Storing in {}: {}ms. Creating new entry? {}", zName, dElapsedMs, !m_aNamedTimes.contains( zName ) ) );

	if ( !m_aNamedTimes.contains( zName ) )
	{
		m_aNamedTimes.insert_or_assign( zName, std::vector<double>() );
	}

	m_aNamedTimes.at( zName ).push_back( dElapsedMs );
}

void
TimeKeeper::SaveToFile( std::string zFilePath )
{
	Logger::S_Log( std::format( "Named Times Map Count: {}", m_aNamedTimes.size() ) );

	Logger logFile;
	logFile.OpenLog( zFilePath );

	for ( auto aTimes : m_aNamedTimes )
	{
		Logger::S_Log( std::format( "Exporting {}", aTimes.first ) );
		logFile.AddToLog( aTimes.first );
		logFile.AddToLog( "------------" );
		for ( auto t : aTimes.second )
		{
			logFile.AddToLog( std::format( "{}", t ) );
		}
		logFile.AddToLog( "" );
	}
}
