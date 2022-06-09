#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

#include <iostream>
#include <sstream>
#include <thread>

#include <codecvt>

#include <inttypes.h>

#include "mpi.h"

// Need this in order to compile:
#define _WINSOCKAPI_
#include <Windows.h>
#include "QuicDriver.h"

using namespace std::chrono_literals;

void RunMpiTest( int iNumArgs, char** azArgs )
{
	MPI_Init( &iNumArgs, &azArgs );

	// Get number of processes and check only 3 processes are used
	int nRanks;
	MPI_Comm_size( MPI_COMM_WORLD, &nRanks );
	if ( nRanks != 2 )
	{
		printf( "This application is meant to be run with 2 processes.\n" );
		MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
	}

	// Set writer rank to 0:
	int iWriterRank = 0;

	int iLocalRank;
	MPI_Comm_rank( MPI_COMM_WORLD, &iLocalRank );

	Quic::S_RegisterDriver( new QuicDriver() );
	uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Registered Quic driver.\n", sec, iLocalRank);

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	if ( iLocalRank == iWriterRank )
	{
		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Creating Quic listener...\n", sec, iLocalRank );
		Quic::S_CreateListener( 4477 );

		// TODO: Make sure listener is connected and running before syncing with other threads.
		auto start = std::chrono::high_resolution_clock::now();
		while ( !Quic::S_IsListenerRunning() )
		{
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			if ( elapsed.count() > 10000 ) break;
			std::this_thread::sleep_for( 100ms );
		}
	}
	else
	{
		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Waiting for rank 0 to create Quic listener...\n", sec, iLocalRank );
	}

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	auto xIsWriterRank = [&iLocalRank, iWriterRank]() { return iLocalRank == iWriterRank; };

	// TODO: Send IP address and port information for non-writer threads to know what the client is connecting to.

	// Send/receive listener status:
	int buffer_send = xIsWriterRank() ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	int buffer_recv;
	int tag_send = 0;
	int tag_recv = tag_send;
	int peer = xIsWriterRank() ? 1 : 0;

	MPI_Sendrecv( &buffer_send, 1, MPI_INT, peer, tag_send,
		&buffer_recv, 1, MPI_INT, peer, tag_recv, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Received value %d from MPI rank %d.\n", sec, iLocalRank, buffer_recv, peer );

	if ( iLocalRank != iWriterRank )
	{
		if ( buffer_recv == 1 )
		{
			std::this_thread::sleep_for( 1000ms );
			sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			printf( "[%" PRIu64  "] Rank %d: Creating Quic client...\n", sec, iLocalRank );
			Quic::S_CreateClient( "127.0.0.1", 4477 );
		}
	}

	std::this_thread::sleep_for( 5000ms );

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	MPI_Finalize();
}

void RunQuicOnlyTest()
{
	Quic::S_RegisterDriver( new QuicDriver() );
	printf( "Registered Quic driver.\n" );

	printf( "Creating Quic listener...\n" );
	Quic::S_CreateListener( 4477 );

	std::this_thread::sleep_for( 100ms );
	printf( "Creating Quic client...\n" );
	Quic::S_CreateClient( "127.0.0.1", 4477 );

	std::this_thread::sleep_for( 10000ms );
}

// The Windows Console only supports Unicode I/O in the wide variants.
// Need to toggle between narrow and wide variant main function based on OS type.
#ifdef WIN32
int MainCommon( int iNumArgs, wchar_t** pazArgs )
{
	auto xConvertToString = []( std::wstring wzText ) {
		std::stringstream ss;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;

		ss << convert.to_bytes( wzText );
		return ( ss.fail() ? "" : ss.str() );
	};

	std::vector<std::string>	azArgsTemp( iNumArgs );
	for ( size_t i = 0; i < azArgsTemp.size(); ++i )
	{
		azArgsTemp[i] = xConvertToString( std::wstring( pazArgs[i] ) );
	}

	char** azArgs = new char* [azArgsTemp.size()];
	for ( size_t i = 0; i < azArgsTemp.size(); ++i )
	{
		azArgs[i] = new char[azArgsTemp[i].size() + 1];
		strcpy( azArgs[i], azArgsTemp[i].c_str() );
	}
#else
int MainCommon( int iNumArgs, char** azArgs )
{
#endif

	RunQuicOnlyTest();

	//RunMpiTest( iNumArgs, azArgs );

	return 0;
}

#ifdef WIN32
int wmain( int iNumArgs, wchar_t** azArgs )
{
	// ensure strings written to console are interpreted as utf8 strings (this setting will persist in the console after the app closes).
	SetConsoleOutputCP( 65001 );

	MainCommon( iNumArgs, azArgs );
}

int WINAPI wWinMain( HINSTANCE, HINSTANCE, PWSTR, int )
{
	int			iNumArgs;
	wchar_t** azArgs = CommandLineToArgvW( GetCommandLineW(), &iNumArgs );

	// ensure strings written to console are interpreted as utf8 strings (this setting will persist in the console after the app closes).
	SetConsoleOutputCP( 65001 );

	MainCommon( iNumArgs, azArgs );
}
#else
int main( int iNumArgs, char** azArgs )
{
	MainCommon( iNumArgs, azArgs );
}
#endif // WIN32


