#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <format>
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
	//MPI_Comm_size( mCommunicator, &nRanks );
	//if ( nRanks != 2 )
	//{
	//	printf( "This application is meant to be run with 2 processes.\n" );
	//	MPI_Abort( mCommunicator, EXIT_FAILURE );
	//}

	//int iLocalRank;
	//MPI_Comm_rank( mCommunicator, &iLocalRank );

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
	//MPI_Barrier( mCommunicator );

	//// Send/receive listener status:
	//int buffer_send = bIsReceiverRank ? ( Quic::S_IsListenerRunning() ? 1 : 0 ) : 1;
	//int buffer_recv;
	//int tag_send = 0;
	//int tag_recv = tag_send;
	//int peer = bIsReceiverRank ? 1 : 0;

	//MPI_Sendrecv( &buffer_send, 1, MPI_INT, peer, tag_send,
	//	&buffer_recv, 1, MPI_INT, peer, tag_recv, mCommunicator, MPI_STATUS_IGNORE );

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
	////	&buffer_recv, 1, MPI_INT, peer, tag_recv, mCommunicator, MPI_STATUS_IGNORE );

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

	//	MPI_Gather( &iSendBufferSize, 1, MPI_INT, rcounts, 1, MPI_INT, iReceiverRank, mCommunicator );

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
	//		MPI_Igatherv( zBuffer.c_str(), iSendBufferSize, MPI_CHAR, rbuf, rcounts, displs, MPI_CHAR, iReceiverRank, mCommunicator, &gatherRequest );

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
	//		MPI_Igatherv( zBuffer.c_str(), iSendBufferSize, MPI_CHAR, NULL, NULL, NULL, MPI_CHAR, iReceiverRank, mCommunicator, &gatherRequest );

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

void PrintLogLine( std::string zMsg, int iLocalRank )
{
	uint64_t uiSeconds = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "[%" PRIu64  "] Rank %d: %s\n", uiSeconds, iLocalRank, zMsg.c_str() );
}

void RunMpiTest( int iNumArgs, char** azArgs )
{
	MPI_Init( &iNumArgs, &azArgs );

	MPI_Comm mCommunicator = MPI_COMM_WORLD;

	// Get number of processes and check only 3 processes are used
	int nRanks;
	MPI_Comm_size( mCommunicator, &nRanks );
	if ( nRanks != 2 )
	{
		PrintLogLine( "This application is meant to be run with 2 processes.", -1 );
		MPI_Abort( mCommunicator, EXIT_FAILURE );
	}

	// Set receiver rank to 0:
	int iReceiverRank = 0;
	int iLocalRank;
	MPI_Comm_rank( mCommunicator, &iLocalRank );

	bool bIsReceiverRank = iLocalRank == iReceiverRank;

	Quic::S_RegisterDriver( new QuicDriver() );
	PrintLogLine( "Registered Quic driver.", iLocalRank );

	// Sync up here.
	MPI_Barrier( mCommunicator );

	uint16_t iPort = 4477;

	// Check host names:
	char cHostName[MPI_MAX_PROCESSOR_NAME];
	int iNameLen;
	MPI_Get_processor_name( cHostName, &iNameLen );
	PrintLogLine( std::format( "Hostname: {}.", cHostName ), iLocalRank );

	// This will be set in non-receiver ranks via MPI_Bcast later:
	char cIpAddress[INET_ADDRSTRLEN];

	if ( bIsReceiverRank )
	{
		PrintLogLine( "Creating Quic listener...", iLocalRank );
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
			PrintLogLine( "Could not get address info.", iLocalRank );
			MPI_Abort( mCommunicator, EXIT_FAILURE );
		}

		for ( struct addrinfo* ptr = pResult; ptr != NULL; ptr = ptr->ai_next )
		{
			if ( ptr->ai_family == AF_INET )
			{
				PrintLogLine( "AF_INET (IPv4)", iLocalRank );
				struct sockaddr_in* pSockAddrIpv4 = (struct sockaddr_in*)ptr->ai_addr;
				IN_ADDR ipAddress = pSockAddrIpv4->sin_addr;
				inet_ntop( AF_INET, &ipAddress, cIpAddress, INET_ADDRSTRLEN );

				PrintLogLine( std::format( "\tIPv4 address {}", cIpAddress ), iLocalRank );
				break;
			}
		}

		PrintLogLine( std::format( "Listening on IP Address {}", cIpAddress ), iLocalRank );
	}
	else
	{
		PrintLogLine( "Waiting for rank 0 to create Quic listener...", iLocalRank );
	}

	// Broadcast receiver status:
	int iReceiverRunningStatus = 0;
	if ( bIsReceiverRank )
	{
		iReceiverRunningStatus = Quic::S_IsListenerRunning() ? 1 : 0;
	}
	MPI_Bcast( &iReceiverRunningStatus, 1, MPI_INT, iReceiverRank, mCommunicator );

	if ( !bIsReceiverRank )
	{
		PrintLogLine( std::format("Received status {} from receiver rank {}.", iReceiverRunningStatus, iReceiverRank ), iLocalRank );
	}

	if ( iReceiverRunningStatus == 0 )
	{
		PrintLogLine( "Receiver rank not set up for receiving messages, aborting run.", iLocalRank );
		return;
	}

	// Broadcast IP address and port information for non-writer threads to know what the client is connecting to.
	MPI_Bcast( &cIpAddress, INET_ADDRSTRLEN, MPI_INT, iReceiverRank, mCommunicator );

	// Create client with the IP address information:
	if ( !bIsReceiverRank )
	{
		PrintLogLine( std::format( "Creating Quic client, connecting to {} on port {}...", cIpAddress, iPort ), iLocalRank );
		Quic::S_CreateClient( cIpAddress, iPort );
	}

	// TODO: Set up something to check whether listener connection has finished starting and streams can be created.
	// 200ms seems like enough time for now - this might vary between machines.
	std::this_thread::sleep_for( 200ms );

	int iIterations = 1;
	int nDataSize = 10;
	for ( size_t i = 0; i < iIterations; ++i )
	{
		// do work
		std::this_thread::sleep_for( 200ms );

		// aggregate data
		if ( !bIsReceiverRank )
		{
			PrintLogLine( "Sending data from client to listener.", iLocalRank );
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
		else
		{
			PrintLogLine( "Storing data.", iLocalRank );
		}
	}

	PrintLogLine( std::format( "Open streams: {}.", Quic::S_HasOpenStreams() ? 1 : 0 ), iLocalRank );

	// All ranks need to remain open while any other ranks are still sending or processing data.
	// Receiver rank will be one to figure out whether all ranks are waiting.
	bool bWait = true;
	do
	{
		// Receiver rank will gather open stream statuses from all other ranks:
		int* rcounts = NULL;
		int iStatusBuffer = Quic::S_HasOpenStreams() ? 1 : 0;

		if ( bIsReceiverRank )
		{
			rcounts = new int[nRanks];
		}
		MPI_Gather( &iStatusBuffer, 1, MPI_INT, rcounts, 1, MPI_INT, iReceiverRank, mCommunicator );

		bWait = Quic::S_HasOpenStreams();
		if ( bIsReceiverRank )
		{
			if ( !bWait )
			{
				// Receiver rank will also wait if any other ranks are sending data .
				bWait = false;
				for ( int i = 1; i < nRanks; ++i )
				{
					if ( rcounts[i] != 0 ) bWait = true;
					PrintLogLine( std::format( "Received status from rank {}: {}.", i, rcounts[i] ), iLocalRank );
				}
			}

			delete rcounts;
		}

		// Broadcast whether receiver rank is waiting or not:
		int iReceiverWaitStatus;
		if ( bIsReceiverRank ) iReceiverWaitStatus = bWait ? 1 : 0;
		MPI_Bcast( &iReceiverWaitStatus, 1, MPI_INT, iReceiverRank, mCommunicator );

		if ( !bIsReceiverRank )
		{
			bWait = iReceiverWaitStatus == 1;
			PrintLogLine( std::format( "Receiver wait status: {}.", iReceiverWaitStatus ), iLocalRank );
		}

		if ( bWait ) PrintLogLine( "Waiting for data sending to finish.", iLocalRank );

		std::this_thread::sleep_for( 100ms );
	} while ( bWait );

	PrintLogLine( "Destroying Quic driver...", iLocalRank );
	Quic::S_DestroyDriver();

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


