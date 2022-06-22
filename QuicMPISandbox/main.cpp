#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

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

	// Set receiver rank to 0:
	int iReceiverRank = 0;
	int iLocalRank;
	MPI_Comm_rank( MPI_COMM_WORLD, &iLocalRank );

	bool									bIsReceiverRank = iLocalRank == iReceiverRank;

	Quic::S_RegisterDriver( new QuicDriver() );
	uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Registered Quic driver.\n", sec, iLocalRank);

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	uint16_t iPort = 4477;

	// Check host names:
	char cHostName[MPI_MAX_PROCESSOR_NAME];
	int iNameLen;
	MPI_Get_processor_name( cHostName, &iNameLen );
	sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Hostname: %s.\n", sec, iLocalRank, cHostName);

	// Will set this in the receiver rank:
	char cIpAddress[INET_ADDRSTRLEN];

	if ( bIsReceiverRank )
	{
		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Creating Quic listener...\n", sec, iLocalRank );
		Quic::S_CreateListener( iPort );

		// TODO: Make sure listener is connected and running before syncing with other threads.
		auto start = std::chrono::high_resolution_clock::now();
		while ( !Quic::S_IsListenerRunning() )
		{
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			if ( elapsed.count() > 10000 ) break;
			std::this_thread::sleep_for( 100ms );
		}


		// Get IP address to send to client threads.
		// Listener is listening on all IP addresses to take the first (preferred) one.
		struct addrinfo* result = NULL;
		struct addrinfo* ptr = NULL;
		struct addrinfo hints;
		ZeroMemory( &hints, sizeof( hints ) );
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_UDP;
		char cPort[80];
		sprintf( cPort, "%" PRIu16, iPort );

		DWORD dwRetval = getaddrinfo( cHostName, cPort, &hints, &result );
		if ( dwRetval != 0 )
		{
			sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			printf( "[%" PRIu64  "] Rank %d: Could not get address info.\n", sec, iLocalRank );
			MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
			return;
		}

		for ( ptr = result; ptr != NULL; ptr = ptr->ai_next )
		{
			if ( ptr->ai_family == AF_INET )
			{
				sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
				printf( "[%" PRIu64  "] Rank %d: AF_INET (IPv4)\n", sec, iLocalRank );
				struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
				IN_ADDR ipAddress = sockaddr_ipv4->sin_addr;
				inet_ntop( AF_INET, &ipAddress, cIpAddress, INET_ADDRSTRLEN );

				sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
				printf( "[%" PRIu64  "] Rank %d: \tIPv4 address %s\n", sec, iLocalRank, cIpAddress );
				break;
			}
		}

		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: IP Address is %s...\n", sec, iLocalRank, cIpAddress );
	}
	else
	{
		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Waiting for rank 0 to create Quic listener...\n", sec, iLocalRank );
	}

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	// Send/receive listener status:
	int buffer_send = bIsReceiverRank ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	int buffer_recv;
	int tag_send = 0;
	int tag_recv = tag_send;
	int peer = bIsReceiverRank ? 1 : 0;

	MPI_Sendrecv( &buffer_send, 1, MPI_INT, peer, tag_send,
		&buffer_recv, 1, MPI_INT, peer, tag_recv, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Received value %d from MPI rank %d.\n", sec, iLocalRank, buffer_recv, peer );

	if ( bIsReceiverRank )
	{
		if ( !Quic::S_IsListenerRunning() )
		{
			MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
			return;
		}
	}

	// TODO: Send IP address and port information for non-writer threads to know what the client is connecting to.
	char cIpAddress_recv[INET_ADDRSTRLEN];
	MPI_Sendrecv( &cIpAddress, INET_ADDRSTRLEN, MPI_INT, peer, tag_send,
		&cIpAddress_recv, INET_ADDRSTRLEN, MPI_INT, peer, tag_recv, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	printf( "[%" PRIu64  "] Rank %d: Received value %s from MPI rank %d.\n", sec, iLocalRank, cIpAddress_recv, peer );

	// Create client with the IP address information:
	if ( !bIsReceiverRank )
	{
		if ( buffer_recv == 1 )
		{
			std::this_thread::sleep_for( 1000ms );
			sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			printf( "[%" PRIu64  "] Rank %d: Creating Quic client, connecting to %s on port %d...\n", sec, iLocalRank, cIpAddress_recv, iPort );
			Quic::S_CreateClient( cIpAddress_recv, iPort );
		}
	}

	std::this_thread::sleep_for( 5000ms );

	//if ( !bIsReceiverRank )
	//{
	//	Quic::S_GetDriver()->ClientSend();
	//}

	//sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//printf( "[%" PRIu64  "] Rank %d: Destroying Quic driver...\n", sec, iLocalRank );
	//Quic::S_DestroyDriver();

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	MPI_Finalize();
}

void RunQuicOnlyTest()
{
	uint16_t iPort = 4477;
	Quic::S_RegisterDriver( new QuicDriver() );
	printf( "Registered Quic driver.\n" );

	printf( "Creating Quic listener...\n" );
	Quic::S_CreateListener( iPort );

	std::this_thread::sleep_for( 100ms );
	printf( "Creating Quic client...\n" );
	Quic::S_CreateClient( "127.0.0.1", iPort );

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

	//RunQuicOnlyTest();

	RunMpiTest( iNumArgs, azArgs );

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


