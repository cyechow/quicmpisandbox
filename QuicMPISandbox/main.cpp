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

void MpiTest( int iNumArgs, char** azArgs )
{
	//MPI_Init( &iNumArgs, &azArgs );

	//// Get number of processes and check only 3 processes are used
	//int nRanks;
	//MPI_Comm_size( MPI_COMM_WORLD, &nRanks );
	//if ( nRanks != 2 )
	//{
	//	printf( "This application is meant to be run with 2 processes.\n" );
	//	MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
	//}

	//int iLocalRank;
	//MPI_Comm_rank( MPI_COMM_WORLD, &iLocalRank );

	//Quic::S_RegisterDriver( new QuicDriver() );
	//uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//printf( "[%" PRIu64  "] Rank %d: Registered Quic driver.\n", sec, iLocalRank);

	//// Set receiver rank to 0:
	//int										iReceiverRank = 0;
	//bool									bIsReceiverRank = iLocalRank == iReceiverRank;

	//// Set up listener in the receiver rank
	//if ( bIsReceiverRank )
	//{
	//	sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//	printf( "[%" PRIu64  "] Rank %d: Creating Quic listener...\n", sec, iLocalRank );
	//	Quic::S_CreateListener( 4477 );

	//	// TODO: Make sure listener is connected and running before syncing with other threads.
	//	auto start = std::chrono::high_resolution_clock::now();
	//	while ( !Quic::S_IsListenerRunning() )
	//	{
	//		auto end = std::chrono::high_resolution_clock::now();
	//		std::chrono::duration<double, std::milli> elapsed = end - start;
	//		if ( elapsed.count() > 10000 ) break;
	//		std::this_thread::sleep_for( 100ms );
	//	}
	//}
	//else
	//{
	//	sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//	printf( "[%" PRIu64  "] Rank %d: Waiting for rank 0 to create Quic listener...\n", sec, iLocalRank );
	//}

	//// Sync up here.
	//MPI_Barrier( MPI_COMM_WORLD );

	//// Send/receive listener status:
	//int buffer_send = bIsReceiverRank ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	//int buffer_recv;
	//int tag_send = 0;
	//int tag_recv = tag_send;
	//int peer = bIsReceiverRank ? 1 : 0;

	//MPI_Sendrecv( &buffer_send, 1, MPI_INT, peer, tag_send,
	//	&buffer_recv, 1, MPI_INT, peer, tag_recv, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	//sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//printf( "[%" PRIu64  "] Rank %d: Received value %d from MPI rank %d.\n", sec, iLocalRank, buffer_recv, peer );

	//if ( bIsReceiverRank && !Quic::S_IsListenerRunning()
	//	|| buffer_recv != 1 )
	//{
	//	printf( "Process %d: Listener not running, exiting...", iLocalRank );
	//	MPI_Finalize();
	//	return;
	//}

	//// Send IP and port info:

	////int buffer_send = bIsReceiverRank ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	////int buffer_recv;
	////int tag_send = 0;
	////int tag_recv = tag_send;
	////int peer = bIsReceiverRank ? 1 : 0;

	////MPI_Sendrecv( &buffer_send, 1, MPI_INT, peer, tag_send,
	////	&buffer_recv, 1, MPI_INT, peer, tag_recv, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	////sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	////printf( "[%" PRIu64  "] Rank %d: Received value %d from MPI rank %d.\n", sec, iLocalRank, buffer_recv, peer );

	//if ( iLocalRank != iReceiverRank )
	//{
	//	if ( buffer_recv == 1 )
	//	{
	//		std::this_thread::sleep_for( 1000ms );
	//		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//		printf( "[%" PRIu64  "] Rank %d: Creating Quic client...\n", sec, iLocalRank );
	//		Quic::S_CreateClient( "127.0.0.1", 4477 );
	//	}
	//}

	//// we use this to return debug logs in the actual application since we can't print to console.
	//std::stringstream						ssLog;

	//// Part of our MPI driver properties:
	//bool									bGatherStarted = false;
	//MPI_Request								gatherRequest;

	//int										nIterations = 10;

	//// !! CHANGE THIS TO REPRODUCE
	//int										nDataSize = 10000;

	//printf( "Process %d: Starting loop.\n", iLocalRank );
	//for ( size_t i = 0; i < nIterations; ++i )
	//{
	//	// Work takes around 100-200ms
	//	std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	//	if ( bGatherStarted )
	//	{
	//		printf( "Process %d: Iteration %d, waiting...\n", iLocalRank, (int)i );
	//		double							tStart = MPI_Wtime();
	//		MPI_Wait( &gatherRequest, MPI_STATUS_IGNORE );
	//		double							tEnd = MPI_Wtime();
	//		double							tElapsed = 1000 * ( tEnd - tStart );
	//		std::cout << "Process " << iLocalRank << ": Iteration " << i << ", waited for " << tElapsed << "ms.\n";
	//	}

	//	// Create our data to be gathered:
	//	std::stringstream					stream;
	//	int									my_value = int( i + 1 ) * ( 10 + iLocalRank );
	//	for ( int j = 0; j < nDataSize; ++j )
	//	{
	//		int								iVal = my_value + j;
	//		stream.write( reinterpret_cast<const char*>( &iVal ), sizeof( int ) );
	//		if ( j == 0 )
	//		{
	//			printf( "First value sent from process %d: %d. Size of vector: %d.\n", iLocalRank, iVal, nDataSize );
	//		}
	//	}

	//	// Double the size for next iteration:
	//	nDataSize = nDataSize + 1;

	//	std::string							zBuffer = stream.str();

	//	int* rcounts = NULL;
	//	int									iSendBufferSize = (int)zBuffer.size();

	//	if ( bIsReceiverRank )
	//	{
	//		rcounts = new int[nRanks];
	//	}

	//	MPI_Gather( &iSendBufferSize, 1, MPI_INT, rcounts, 1, MPI_INT, iReceiverRank, MPI_COMM_WORLD );

	//	if ( bIsReceiverRank )
	//	{
	//		// Received buffer, total buffer length, displacements:
	//		char* rbuf = NULL;
	//		int								iRecvBufferSize = 0;
	//		int* displs = new int[nRanks];

	//		displs[0] = 0;
	//		iRecvBufferSize += rcounts[0];

	//		if ( iRecvBufferSize > INT_MAX )
	//		{
	//			printf( "Received buffer is greater than INT_MAX.\n" );
	//		}

	//		//printf( "Process %d: Rank %d: count = %d, displs: = %d\n", my_rank, 0, rcounts[0], displs[0] );

	//		for ( int i = 1; i < nRanks; ++i )
	//		{
	//			iRecvBufferSize += rcounts[i];
	//			displs[i] = displs[i - 1] + rcounts[i - 1];

	//			ssLog << "Rank " << i << ": " << rcounts[i] << "\n";
	//			printf( "Process %d: Rank %d: count = %d, displs: = %d\n", iLocalRank, i, rcounts[i], displs[i] );
	//		}

	//		iRecvBufferSize += 1;
	//		rbuf = new char[iRecvBufferSize];
	//		rbuf[iRecvBufferSize - 1] = '\0'; // null terminate the char buffer

	//		ssLog << "RecvBufferSize: " << iRecvBufferSize << "\n";
	//		printf( "RecvBufferSize: %d\n", iRecvBufferSize );

	//		// Gather all buffers:
	//		MPI_Igatherv( zBuffer.c_str(), iSendBufferSize, MPI_CHAR, rbuf, rcounts, displs, MPI_CHAR, iReceiverRank, MPI_COMM_WORLD, &gatherRequest );

	//		printf( "Wait for completion.\n" );
	//		double							tWaitStart = MPI_Wtime();
	//		MPI_Wait( &gatherRequest, MPI_STATUS_IGNORE );
	//		double							tWaitEnd = MPI_Wtime();
	//		double							tWaitElapsed = 1000 * ( tWaitEnd - tWaitStart );
	//		std::cout << "Gather completed, wait took " << tWaitElapsed << "ms.\n";

	//		printf( "Checking displacements: %d, %d, %d\n", displs[0], displs[1], displs[2] );

	//		printf( "Values gathered in the buffer on process %d:\n", iLocalRank );

	//		// We pass it out in a custom stream class that wraps a stringstream but for the sake of this, we'll just put it into a vector of strings:
	//		std::vector<std::string>	out_Streams;

	//		// Process the received data:
	//		int								iCount = 0;
	//		for ( int i = 0; i < nRanks; ++i )
	//		{
	//			std::stringstream	 		stream;
	//			stream.write( rbuf, rcounts[i] );
	//			out_Streams.push_back( stream.str() );

	//			// For checking the data that's been received:
	//			int							test;
	//			stream.read( reinterpret_cast<char*>( &test ), sizeof( int ) );
	//			printf( "First value from process %d: %d.\n", i, test );

	//			rbuf += rcounts[i];
	//			iCount += rcounts[i];
	//		}

	//		// Reset pointer to the beginning:
	//		rbuf -= iCount;

	//		delete[] rcounts;
	//		delete[] displs;
	//		delete[] rbuf;
	//	}
	//	else
	//	{
	//		// Send buffer (non-blocking):
	//		MPI_Igatherv( zBuffer.c_str(), iSendBufferSize, MPI_CHAR, NULL, NULL, NULL, MPI_CHAR, iReceiverRank, MPI_COMM_WORLD, &gatherRequest );

	//		if ( i == ( nIterations - 1 ) )
	//		{
	//			// Last iteration, call wait now:
	//			MPI_Wait( &gatherRequest, MPI_STATUS_IGNORE );
	//		}
	//		else
	//		{
	//			int							iGatherComplete = 0;
	//			MPI_Test( &gatherRequest, &iGatherComplete, MPI_STATUS_IGNORE );
	//			if ( iGatherComplete )
	//			{
	//				ssLog << "Gather request fulfilled.\n";
	//				printf( "Gather request fulfilled.\n" );
	//			}
	//			else
	//			{
	//				ssLog << "Moving on to process more of the simulation before coming back here to send the next batch of data.\n";
	//				printf( "Moving on to process more of the simulation before coming back here to send the next batch of data.\n" );
	//				bGatherStarted = true;
	//			}
	//		}
	//	}
	//}

	//printf( "Process %d: Exiting...", iLocalRank );
	//MPI_Finalize();
}

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
	uint64_t uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Registered Quic driver.\n", uiSeconds, iLocalRank);

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	uint16_t iPort = 4477;

	// Check host names:
	char cHostName[MPI_MAX_PROCESSOR_NAME];
	int iNameLen;
	MPI_Get_processor_name( cHostName, &iNameLen );
	uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: Hostname: %s.\n", uiSeconds, iLocalRank, cHostName);

	// Will set this in the receiver rank:
	char cIpAddress[INET_ADDRSTRLEN];

	if ( bIsReceiverRank )
	{
		uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Creating Quic listener...\n", uiSeconds, iLocalRank );
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
		struct addrinfo* pResult = NULL;
		struct addrinfo addrHints;
		ZeroMemory( &addrHints, sizeof( addrHints ) );
		addrHints.ai_family = AF_INET;
		addrHints.ai_socktype = SOCK_STREAM;
		addrHints.ai_protocol = IPPROTO_UDP;
		char cPort[80];
		sprintf( cPort, "%" PRIu16, iPort );

		if ( getaddrinfo( cHostName, cPort, &addrHints, &pResult ) != 0 )
		{
			uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			printf( "[%" PRIu64  "] Rank %d: Could not get address info.\n", uiSeconds, iLocalRank );
			MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
			return;
		}

		for ( struct addrinfo* ptr = pResult; ptr != NULL; ptr = ptr->ai_next )
		{
			if ( ptr->ai_family == AF_INET )
			{
				uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
				printf( "[%" PRIu64  "] Rank %d: AF_INET (IPv4)\n", uiSeconds, iLocalRank );
				struct sockaddr_in* pSockAddrIpv4 = (struct sockaddr_in*)ptr->ai_addr;
				IN_ADDR ipAddress = pSockAddrIpv4->sin_addr;
				inet_ntop( AF_INET, &ipAddress, cIpAddress, INET_ADDRSTRLEN );

				uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
				printf( "[%" PRIu64  "] Rank %d: \tIPv4 address %s\n", uiSeconds, iLocalRank, cIpAddress );
				break;
			}
		}

		uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: IP Address is %s...\n", uiSeconds, iLocalRank, cIpAddress );
	}
	else
	{
		uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Waiting for rank 0 to create Quic listener...\n", uiSeconds, iLocalRank );
	}

	// Sync up here.
	MPI_Barrier( MPI_COMM_WORLD );

	// Send/receive listener status:
	int iSendBuffer = bIsReceiverRank ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	int iRecvBuffer;
	int iSendTag = 0;
	int iRecvTag = iSendTag;
	int iTargetRank = bIsReceiverRank ? iReceiverRank + 1 : iReceiverRank; // This won't work with more than 2 ranks.

	MPI_Sendrecv( &iSendBuffer, 1, MPI_INT, iTargetRank, iSendTag,
		&iRecvBuffer, 1, MPI_INT, iTargetRank, iRecvTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	if ( bIsReceiverRank )
	{
		if ( !Quic::S_IsListenerRunning() )
		{
			MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
			return;
		}
	}
	else
	{
		uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Received value %d from MPI rank %d.\n", uiSeconds, iLocalRank, iRecvBuffer, iTargetRank );
	}

	// TODO: Send IP address and port information for non-writer threads to know what the client is connecting to.
	char cIpAddress_recv[INET_ADDRSTRLEN];
	MPI_Sendrecv( &cIpAddress, INET_ADDRSTRLEN, MPI_INT, iTargetRank, iSendTag,
		&cIpAddress_recv, INET_ADDRSTRLEN, MPI_INT, iTargetRank, iRecvTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	// Create client with the IP address information:
	if ( !bIsReceiverRank )
	{
		printf( "[%" PRIu64  "] Rank %d: Received value %s from MPI rank %d.\n", uiSeconds, iLocalRank, cIpAddress_recv, iTargetRank );
		if ( iRecvBuffer == 1 )
		{
			std::this_thread::sleep_for( 1000ms );
			uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			printf( "[%" PRIu64  "] Rank %d: Creating Quic client, connecting to %s on port %d...\n", uiSeconds, iLocalRank, cIpAddress_recv, iPort );
			Quic::S_CreateClient( cIpAddress_recv, iPort );
		}
	}

	std::this_thread::sleep_for( 5000ms );

	if ( !bIsReceiverRank )
	{
		uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		printf( "[%" PRIu64  "] Rank %d: Sending data from client to listener.\n", uiSeconds, iLocalRank );

		int								iIterations = 1;
		int								nDataSize = 10;
		for ( size_t i = 0; i < iIterations; ++i )
		{
			std::stringstream				ssData;
			ssData << "testing";
			//int								my_value = int( i + 1 ) * ( 10 + iLocalRank );
			//for ( int j = 0; j < nDataSize; ++j )
			//{
			//	int								iVal = my_value + j;
			//	ssData.write( reinterpret_cast<const char*>( &iVal ), sizeof( int ) );
			//	//if ( j == 0 )
			//	{
			//		sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
			//		printf( "[%" PRIu64  "] Rank %d: First value sent: %d. Size of vector: %d.\n", sec, iLocalRank, iVal, nDataSize );
			//	}
			//}
			Quic::S_GetDriver()->ClientSendData( ssData.str() );
		}
	}

	//uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//printf( "[%" PRIu64  "] Rank %d: Destroying Quic driver...\n", uiSeconds, iLocalRank );
	//Quic::S_DestroyDriver();

	printf( "[%" PRIu64  "] Rank %d: Open streams: %i.\n", uiSeconds, iLocalRank, Quic::S_HasOpenStreams() ? 1 : 0 );
	//int iCount = 0;
	//while ( Quic::S_HasOpenStreams() )
	//{
	//	if ( iCount > 100 )break;
	//	printf( "[%" PRIu64  "] Rank %d: Quic still running, waiting for it to close.\n", uiSeconds, iLocalRank );
	//	std::this_thread::sleep_for( 100ms );
	//	iCount++;
	//}

	//iSendBuffer = Quic::S_HasOpenStreams() ? 1 : 0;
	//MPI_Sendrecv( &iSendBuffer, 1, MPI_INT, iTargetRank, iSendTag,
	//	&iRecvBuffer, 1, MPI_INT, iTargetRank, iRecvTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

	//while ( iRecvBuffer == 1 || iSendBuffer == 1 )
	//{
	//	uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	//	printf( "[%" PRIu64  "] Rank %d: Quic still running, waiting for it to close.\n", uiSeconds, iLocalRank );
	//	std::this_thread::sleep_for( 100ms );
	//	iSendBuffer = Quic::S_HasOpenStreams() ? 1 : 0;
	//	MPI_Sendrecv( &iSendBuffer, 1, MPI_INT, iTargetRank, iSendTag,
	//		&iRecvBuffer, 1, MPI_INT, iTargetRank, iRecvTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	//}

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

	std::this_thread::sleep_for( 2000ms );

	int								iIterations = 1;
	int								nDataSize = 10;
	for ( size_t i = 0; i < iIterations; ++i )
	{
		std::stringstream				ssData;
		ssData << "testing";
		//int								my_value = int( i + 1 ) * ( 10 );
		//for ( int j = 0; j < nDataSize; ++j )
		//{
		//	int								iVal = my_value + j;
		//	ssData.write( reinterpret_cast<const char*>( &iVal ), sizeof( int ) );
		//	//if ( j == 0 )
		//	{
		//		uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
		//		printf( "[%" PRIu64  "] First value sent: %d. Size of vector: %d.\n", sec, iVal, nDataSize );
		//	}
		//}
		Quic::S_GetDriver()->ClientSendData( ssData.str() );
	}
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


