#include "Log.h"

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
