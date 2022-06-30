#include "Log.h"

#include "mpi.h"
#include <format>

Logger::Logger() :
	m_pStream( NULL )
{}

Logger::~Logger()
{
	if ( NULL == m_pStream ) return;
	if ( m_pStream->is_open() ) m_pStream->close();
	delete m_pStream;
	m_pStream = NULL;
}

void
Logger::OpenLog( std::string zFilePath )
{
	m_pStream = new std::ofstream( zFilePath.c_str() );
}

bool
Logger::AddToLog( std::string zMsg )
{
	if ( NULL == m_pStream ) return false;

	( *m_pStream ) << zMsg;
	( *m_pStream ) << "\n";
	m_pStream->flush();

	return true;
}

void
Logger::S_Log( std::string zMsg )
{
	static Logger		s_Log;
	if ( !s_Log.IsValid() )
	{
		int iLocalRank;
		MPI_Comm_rank( MPI_COMM_WORLD, &iLocalRank );
		s_Log.OpenLog( std::format( "DebugLog_Rank_{}.txt", iLocalRank ) );
	}

	s_Log.AddToLog( zMsg );
}

