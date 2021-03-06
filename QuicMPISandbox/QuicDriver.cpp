#include "QuicDriver.h"

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <ip2string.h>

#include <format>
#include <sstream>
#include <thread>
#include <vector>
#include <inttypes.h>

using namespace std::chrono_literals;

QuicDriver::QuicDriver() :
	m_MsQuic( nullptr ),
	m_Registration( nullptr ),
	m_ListenerConfiguration( nullptr ),
	m_Listener( nullptr ),
	m_ClientConfiguration( nullptr ),
	m_ClientConnection( nullptr ),
	m_Stream( nullptr ),
	m_ListenerStatus( QUIC_STATUS_NOT_FOUND ),
	m_ClientStatus( QUIC_STATUS_NOT_FOUND ),
	m_bListenerRunning( false ),
	m_bClientConnected( false ),
	m_bClientStreamReady( false ),
	m_iOpenStreams( 0 )
{}

QuicDriver::~QuicDriver()
{}

void
QuicDriver::Init()
{
	QUIC_STATUS Status = QUIC_STATUS_SUCCESS;

	//
	// Open a handle to the library and get the API function table.
	//
	if ( QUIC_FAILED( Status = MsQuicOpen2( &m_MsQuic ) ) )
	{
		printf( "MsQuicOpen2 failed, 0x%x!\n", Status );
		Fini();
	}

	//
	// Create a registration for the app's connections.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->RegistrationOpen( &m_RegConfig, &m_Registration ) ) )
	{
		printf( "RegistrationOpen failed, 0x%x!\n", Status );
		Fini();
	}
}

void
QuicDriver::Fini()
{
	if ( m_MsQuic != nullptr )
	{
		if ( m_ClientConnection != nullptr )
		{
			ShutdownClientConnection( m_ClientConnection );
		}
		if ( m_Listener != nullptr )
		{
			CloseListener( m_Listener );
		}
		if ( m_ListenerConfiguration != nullptr )
		{
			m_MsQuic->ConfigurationClose( m_ListenerConfiguration );
		}
		if ( m_ClientConfiguration != nullptr )
		{
			m_MsQuic->ConfigurationClose( m_ClientConfiguration );
		}
		if ( m_Registration != nullptr )
		{
			//
			// This will block until all outstanding child objects have been
			// closed.
			//
			m_MsQuic->RegistrationClose( m_Registration );
		}
		MsQuicClose( m_MsQuic );
	}
}

void
QuicDriver::CreateListener( uint16_t iPort )
{
	//
	// Configures the address used for the listener to listen on all IP
	// addresses and the given UDP port.
	//
	QUIC_ADDR Address{ 0 };
	QuicAddrSetFamily( &Address, QUIC_ADDRESS_FAMILY_INET );
	QuicAddrSetPort( &Address, iPort );

	std::string zCertFile = Quic::S_GetTestCertFileName();
	std::string zKeyFile = Quic::S_GetTestKeyFileName();
	//std::string zPassPhrase = Quic::S_GetTestPassPhrase();
	printf( "%s: Cert - %s. Key - %s\n", __FUNCTION__, zCertFile.c_str(), zKeyFile.c_str() );
	if ( !ServerLoadConfiguration( zCertFile.c_str(), zKeyFile.c_str() ) ) return;

	//
	// Create/allocate a new listener object.
	//
	if ( QUIC_FAILED( m_ListenerStatus = GetMsQuic()->ListenerOpen( GetRegistration(), Quic::S_ServerListenerCallback, nullptr, &m_Listener ) ) )
	{
		printf( "ListenerOpen failed, 0x%x!\n", m_ListenerStatus );
		CloseListener( m_Listener );
		Fini();
		return;
	}

	//
	// Starts listening for incoming connections.
	//
	if ( QUIC_FAILED( m_ListenerStatus = GetMsQuic()->ListenerStart( m_Listener, &m_Alpn, 1, &Address ) ) )
	{
		printf( "ListenerStart failed, 0x%x!\n", m_ListenerStatus );
		CloseListener( m_Listener );
		Fini();
		return;
	}

	SetListenerRunning( true );
}

bool
QuicDriver::ServerLoadConfiguration( const char* pCertHash )
{
	QUIC_SETTINGS Settings{ 0 };
	//
	// Configures the server's idle timeout.
	//
	Settings.IdleTimeoutMs = m_IdleTimeoutMs;
	Settings.IsSet.IdleTimeoutMs = true;
	//
	// Configures the server's resumption level to allow for resumption and
	// 0-RTT.
	//
	Settings.ServerResumptionLevel = QUIC_SERVER_RESUME_AND_ZERORTT;
	Settings.IsSet.ServerResumptionLevel = true;
	//
	// Configures the server's settings to allow for the peer to open a single
	// bidirectional stream. By default connections are not configured to allow
	// any streams from the peer.
	//
	Settings.PeerBidiStreamCount = 1;
	Settings.IsSet.PeerBidiStreamCount = true;

	QUIC_CREDENTIAL_CONFIG_HELPER Config;
	memset( &Config, 0, sizeof( Config ) );
	Config.CredConfig.Flags = QUIC_CREDENTIAL_FLAG_NONE;

	if ( !SetupCredentialCertHash( pCertHash, Config ) )
	{
		return false;
	}

	//
	// Allocate/initialize the configuration object, with the configured ALPN
	// and settings.
	//
	QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationOpen( m_Registration, &m_Alpn, 1, &Settings, sizeof( Settings ), nullptr, &m_ListenerConfiguration ) ) )
	{
		printf( "%s: ConfigurationOpen failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	//
	// Loads the TLS credential part of the configuration.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationLoadCredential( m_ListenerConfiguration, &Config.CredConfig ) ) )
	{
		printf( "%s: ConfigurationLoadCredential failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	return true;
}

bool
QuicDriver::ServerLoadConfiguration( const char* pCertFile, const char* pKeyFile, const char* pPassword )
{
	QUIC_SETTINGS Settings{ 0 };
	//
	// Configures the server's idle timeout.
	//
	Settings.IdleTimeoutMs = m_IdleTimeoutMs;
	Settings.IsSet.IdleTimeoutMs = true;
	//
	// Configures the server's resumption level to allow for resumption and
	// 0-RTT.
	//
	Settings.ServerResumptionLevel = QUIC_SERVER_RESUME_AND_ZERORTT;
	Settings.IsSet.ServerResumptionLevel = true;
	//
	// Configures the server's settings to allow for the peer to open a single
	// bidirectional stream. By default connections are not configured to allow
	// any streams from the peer.
	//
	Settings.PeerBidiStreamCount = 1;
	Settings.IsSet.PeerBidiStreamCount = true;

	QUIC_CREDENTIAL_CONFIG_HELPER Config;
	memset( &Config, 0, sizeof( Config ) );
	Config.CredConfig.Flags = QUIC_CREDENTIAL_FLAG_NONE;

	if ( !SetupCredentialCertFile( pCertFile, pKeyFile, pPassword, Config ) )
	{
		return false;
	}

	//
	// Allocate/initialize the configuration object, with the configured ALPN
	// and settings.
	//
	QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationOpen( m_Registration, &m_Alpn, 1, &Settings, sizeof( Settings ), nullptr, &m_ListenerConfiguration ) ) )
	{
		printf( "%s: ConfigurationOpen failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	//
	// Loads the TLS credential part of the configuration.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationLoadCredential( m_ListenerConfiguration, &Config.CredConfig ) ) )
	{
		printf( "%s: ConfigurationLoadCredential failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	return true;
}

void
QuicDriver::ServerSend( HQUIC Stream )
{
	//
	// Allocates and builds the buffer to send over the stream.
	//
	DataPacket* pData = new DataPacket( "Server sending test data" );

	printf( "[strm-server][%p] Sending data: %p\n", Stream, pData->GetCBuffer() );

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	QUIC_STATUS Status;
	if ( QUIC_FAILED( Status = m_MsQuic->StreamSend( Stream, pData->GetQuicBuffer(), pData->GetQuicBufferCount(), QUIC_SEND_FLAG_FIN, pData ) ) )
	{
		printf( "StreamSend failed, 0x%x!\n", Status );
		delete pData;
		m_MsQuic->StreamShutdown( Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0 );
	}
}

void
QuicDriver::CloseListener( HQUIC Listener )
{
	if ( Listener != nullptr )
	{
		GetMsQuic()->ListenerClose( Listener );
		SetListenerRunning( false );
	}
}

void
QuicDriver::CreateClient( const char* pTargetAddress, uint16_t iPort, bool bUnsecure, const char* pResumptionTicket )
{
	//
	// Load the client configuration based on the "unsecure" command line option.
	//
	if ( !ClientLoadConfiguration( bUnsecure ) )
	{
		return;
	}

	//
	// Allocate a new connection object.
	//
	if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->ConnectionOpen( m_Registration, Quic::S_ClientConnectionCallback, nullptr, &m_ClientConnection ) ) )
	{
		printf( "ConnectionOpen failed, 0x%x!\n", m_ClientStatus );
		CloseClientConnection( m_ClientConnection );
		return;
	}

	if ( pResumptionTicket != nullptr )
	{
		//
		// If provided at the command line, set the resumption ticket that can
		// be used to resume a previous session.
		//
		uint8_t ResumptionTicket[1024];
		uint16_t TicketLength = (uint16_t)DecodeHexBuffer( pResumptionTicket, sizeof( ResumptionTicket ), ResumptionTicket );
		if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->SetParam( m_ClientConnection, QUIC_PARAM_CONN_RESUMPTION_TICKET, TicketLength, ResumptionTicket ) ) )
		{
			printf( "SetParam(QUIC_PARAM_CONN_RESUMPTION_TICKET) failed, 0x%x!\n", m_ClientStatus );
			CloseClientConnection( m_ClientConnection );
			return;
		}
	}

	//
	// Get the target / server name or IP from the command line.
	//
	if ( pTargetAddress == nullptr )
	{
		m_ClientStatus = QUIC_STATUS_INVALID_PARAMETER;
		CloseClientConnection( m_ClientConnection );
		return;
	}

	printf( "[conn-client][%p] Connecting...\n", m_ClientConnection );

	//
	// Start the connection to the server.
	//
	if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->ConnectionStart( m_ClientConnection, m_ClientConfiguration, QUIC_ADDRESS_FAMILY_UNSPEC, pTargetAddress, iPort ) ) )
	{
		printf( "ConnectionStart failed, 0x%x!\n", m_ClientStatus );
		CloseClientConnection( m_ClientConnection );
		return;
	}
}

bool
QuicDriver::ClientLoadConfiguration( bool bUnsecure )
{
	QUIC_SETTINGS Settings{ 0 };
	//
	// Configures the client's idle timeout.
	//
	Settings.IdleTimeoutMs = m_IdleTimeoutMs;
	Settings.IsSet.IdleTimeoutMs = true;

	//
	// Configures a default client configuration, optionally disabling
	// server certificate validation.
	//
	QUIC_CREDENTIAL_CONFIG CredConfig;
	memset( &CredConfig, 0, sizeof( CredConfig ) );
	CredConfig.Type = QUIC_CREDENTIAL_TYPE_NONE;
	CredConfig.Flags = QUIC_CREDENTIAL_FLAG_CLIENT;
	if ( bUnsecure )
	{
		CredConfig.Flags |= QUIC_CREDENTIAL_FLAG_NO_CERTIFICATE_VALIDATION;
	}

	//
	// Allocate/initialize the configuration object, with the configured ALPN
	// and settings.
	//
	QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationOpen( m_Registration, &m_Alpn, 1, &Settings, sizeof( Settings ), nullptr, &m_ClientConfiguration ) ) )
	{
		printf( "%s: ConfigurationOpen failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	//
	// Loads the TLS credential part of the configuration. This is required even
	// on client side, to indicate if a certificate is required or not.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->ConfigurationLoadCredential( m_ClientConfiguration, &CredConfig ) ) )
	{
		printf( "%s: ConfigurationLoadCredential failed, 0x%x!\n", __FUNCTION__, Status );
		return false;
	}

	return true;
}

void
QuicDriver::ClientSend( HQUIC Connection )
{
	QUIC_STATUS Status;
	HQUIC Stream = nullptr;

	//
	// Create/allocate a new bidirectional stream. The stream is just allocated
	// and no QUIC stream identifier is assigned until it's started.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamOpen( Connection, QUIC_STREAM_OPEN_FLAG_NONE, Quic::S_ClientStreamCallback, nullptr, &Stream ) ) )
	{
		printf( "StreamOpen failed, 0x%x!\n", Status );
		return;
	}

	printf( "[strm-client][%p] Starting...\n", Stream );

	//
	// Starts the bidirectional stream. By default, the peer is not notified of
	// the stream being started until data is sent on the stream.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamStart( Stream, QUIC_STREAM_START_FLAG_NONE ) ) )
	{
		printf( "StreamStart failed, 0x%x!\n", Status );
		m_MsQuic->StreamClose( Stream );
		return;
	}

	//
	// Allocates and builds the buffer to send over the stream.
	//
	DataPacket* pData = new DataPacket( "Client sending test data" );

	printf( "[strm-server][%p] Sending data: %p\n", Stream, pData->GetCBuffer() );

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	pData->SetTimeSentNow();
	if ( QUIC_FAILED( Status = m_MsQuic->StreamSend( Stream, pData->GetQuicBuffer(), pData->GetQuicBufferCount(), QUIC_SEND_FLAG_FIN, pData ) ) )
	{
		printf( "StreamSend failed, 0x%x!\n", Status );
		delete pData;
		return;
	}
}

/// <summary>
/// Uses the client-server connection that was opened.
/// Creates new unidirectional stream, sends data, and closes it once done.
/// </summary>
void
QuicDriver::ClientSendData( const std::string zDataBuffer )
{
	auto tStart = std::chrono::steady_clock::now();
	if ( !m_bClientConnected )
	{
		printf( "Client not connected to server! Cannot send data\n" );
		return;
	}

	QUIC_STATUS Status;
	HQUIC Stream = nullptr;

	//
	// Create/allocate a new bidirectional stream. The stream is just allocated
	// and no QUIC stream identifier is assigned until it's started.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamOpen( m_ClientConnection, QUIC_STREAM_OPEN_FLAG_NONE, Quic::S_ClientStreamCallback, nullptr, &Stream ) ) )
	{
		printf( "StreamOpen failed, 0x%x!\n", Status );
		return;
	}

	printf( "[strm-client][%p] Starting...\n", Stream );

	//
	// Starts the bidirectional stream. By default, the peer is not notified of
	// the stream being started until data is sent on the stream.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamStart( Stream, QUIC_STREAM_START_FLAG_IMMEDIATE ) ) )
	{
		printf( "StreamStart failed, 0x%x!\n", Status );
		m_MsQuic->StreamClose( Stream );
		return;
	}

	auto tWaitStart = std::chrono::steady_clock::now();
	while ( !IsClientStreamReady() )
	{
		std::this_thread::sleep_for( 0.001ms );
	}
	auto tWaitEnd = std::chrono::steady_clock::now();
	double dWaitElapsedMS = double( std::chrono::duration_cast<std::chrono::milliseconds>( tWaitEnd - tWaitStart ).count() );
	double dTotalStreamStartElapsedMs = double( std::chrono::duration_cast<std::chrono::milliseconds>( tWaitEnd - tStart ).count() );
	printf( "[strm-client][%p] Waiting for stream start to complete took %f ms.\n", Stream, dWaitElapsedMS );
	Quic::S_StoreTime( "StartStream", dTotalStreamStartElapsedMs );

	//
	// Allocates and builds the buffer to send over the stream.
	// Create new heap-allocated data packet.
	//
	DataPacket* pData = new DataPacket( zDataBuffer );

	printf( "[strm-client][%p] Sending data: %p. Buffer length: %" PRIu64 ". Original string: %s.\n", Stream, pData->GetCBuffer(), pData->GetBufferSize(), zDataBuffer.c_str() );

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	pData->SetTimeSentNow();
	if ( QUIC_FAILED( Status = m_MsQuic->StreamSend( Stream, pData->GetQuicBuffer(), pData->GetQuicBufferCount(), QUIC_SEND_FLAG_FIN, pData ) ) )
	{
		printf( "StreamSend failed, 0x%x!\n", Status );
		delete pData;
	}
	auto tEnd = std::chrono::steady_clock::now();
	double dElapsedMS = double( std::chrono::duration_cast<std::chrono::milliseconds>( tEnd - tStart ).count() );
	printf( "Full ClientSendData call - elapsed time: %f\n", dElapsedMS );
}

HQUIC
QuicDriver::CreateStream()
{
	auto tStart = std::chrono::steady_clock::now();
	if ( !m_bClientConnected )
	{
		printf( "Client not connected to server! Cannot send data\n" );
		return m_Stream;
	}

	QUIC_STATUS Status;

	//
	// Create/allocate a new bidirectional stream. The stream is just allocated
	// and no QUIC stream identifier is assigned until it's started.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamOpen( m_ClientConnection, QUIC_STREAM_OPEN_FLAG_NONE, Quic::S_ClientStreamCallback, nullptr, &m_Stream ) ) )
	{
		printf( "StreamOpen failed, 0x%x!\n", Status );
		return m_Stream;
	}

	printf( "[strm-client][%p] Starting...\n", m_Stream );

	//
	// Starts the bidirectional stream. By default, the peer is not notified of
	// the stream being started until data is sent on the stream.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamStart( m_Stream, QUIC_STREAM_START_FLAG_IMMEDIATE ) ) )
	{
		printf( "StreamStart failed, 0x%x!\n", Status );
		m_MsQuic->StreamClose( m_Stream );
		return m_Stream;
	}

	while ( !IsClientStreamReady() )
	{
		printf( "[strm-client][%p] Waiting for stream start to complete.\n", m_Stream );
		std::this_thread::sleep_for( 1ms );
	}

	auto tEnd = std::chrono::steady_clock::now();
	double dElapsedMS = double( std::chrono::duration_cast<std::chrono::milliseconds>( tEnd - tStart ).count() );
	printf( "Created stream - elapsed time: %f\n", dElapsedMS );
	return m_Stream;
}

bool
QuicDriver::CloseStream( HQUIC Stream )
{
	// TODO.
	m_MsQuic->StreamClose( Stream );
	return true;
}

void
QuicDriver::StoreStreamData( HQUIC Stream, int iBufferCount, const QUIC_BUFFER* pIncBuffers )
{
	printf( "[%p]: Storing data...\n", Stream );
	if ( !m_aStreamDataBuffers.contains( Stream ) )
	{
		m_aStreamDataBuffers.insert_or_assign( Stream, std::vector<std::stringstream*>() );
	}
	std::vector<std::stringstream*>* paBuffers  = &m_aStreamDataBuffers.at( Stream );

	printf( "[%p] Buffers: %I64d\n", Stream, paBuffers->size() );

	for ( int i = 0; i < iBufferCount; ++i )
	{
		uint8_t* pCBuffer = pIncBuffers[i].Buffer;
		size_t sBufferLength = pIncBuffers[i].Length;

		if ( paBuffers->size() <= i )
			paBuffers->push_back( new std::stringstream() );

		std::stringstream* pDataBuffer = paBuffers->at( i );
		pDataBuffer->write( (char*)pCBuffer, sBufferLength );
		
		//printf( "[%p] Data received: %p. String: %s\n", Stream, pCBuffer, pDataBuffer->str().c_str() );
		printf( "[%p] Data received: %p\n", Stream, pCBuffer );
	}
}

void
QuicDriver::ProcessData( HQUIC Stream )
{
	printf( "[%p]: Processing data...\n", Stream );
	auto tStart = std::chrono::steady_clock::now();
	double dTimeReceived = double( std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count() );

	if ( m_aStreamDataBuffers.contains( Stream ) )
	{
		std::vector<std::stringstream*> aBuffers = m_aStreamDataBuffers[Stream];
		printf( "[%p]: Streams stored: %I64d\n", Stream, aBuffers.size() );
		for ( std::stringstream* pDataBuffer : aBuffers )
		{
			printf( "[%p] Data in storage: %s\n", Stream, pDataBuffer->str().c_str() );

			// TEST
			std::string zPacketType;
			size_t iSize;
			*pDataBuffer >> iSize;
			if ( iSize )
			{
				char* azData = new char[iSize + 1];
				pDataBuffer->read( azData, iSize );
				azData[iSize] = 0;
				zPacketType.assign( azData );
				delete[] azData;
			}
			else
			{
				zPacketType.clear();
			}

			if ( zPacketType == "data" )
			{
				int iCount;
				if ( pDataBuffer->read( reinterpret_cast<char*>( &iCount ), sizeof( int ) ) )
				{
					printf( "[%p] # of values: %d\n", Stream, iCount );
					if ( iCount < 1000000 )
					{
						std::vector<int> aValues;
						for ( int j = 0; j < iCount; ++j )
						{
							int iResult;
							if ( pDataBuffer->read( reinterpret_cast<char*>( &iResult ), sizeof( int ) ) )
							{
								//printf( "[%p] Retrieved value: %d\n", Stream, iResult );
								aValues.push_back( iResult );
							}
						}
						printf( "[%p] Retrieved %d values\n", Stream, aValues.size() );
					}
				}
				double dTimeSent;
				if ( pDataBuffer->read( reinterpret_cast<char*>( &dTimeSent ), sizeof( double ) ) )
				{
					printf( "[%p] Time sent: %f\n", Stream, dTimeSent );
					double dElapsedMs = double( dTimeReceived ) - dTimeSent;
					Quic::S_StoreTime( "ReceiveData", dElapsedMs );
					printf( "[%p] Data fully received. Elapsed time: %f.\n", Stream, dElapsedMs );
				}
			}
			// ENDTEST

			// Done with data:
			delete pDataBuffer;
		}

		m_aStreamDataBuffers.erase( Stream );
	}

	auto tEnd = std::chrono::steady_clock::now();
	double dElapsedMS = double( std::chrono::duration_cast<std::chrono::milliseconds>( tEnd - tStart ).count() );

	Quic::S_StoreTime( "Process", dElapsedMS );

	printf( "[%p] Processing time: %f ms\n", Stream, dElapsedMS );
}

void
QuicDriver::CloseClientConnection( HQUIC Connection )
{
	if ( Connection != nullptr )
	{
		m_MsQuic->ConnectionClose( Connection );
		SetClientConnected( false );
	}
}

void
QuicDriver::ShutdownClientConnection( HQUIC Connection )
{
	if ( Connection != nullptr )
	{
		m_MsQuic->ConnectionShutdown( Connection, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0 );
		SetClientConnected( false );
	}
}

bool
QuicDriver::SetupCredentialCertHash( const char* pCertHash, QUIC_CREDENTIAL_CONFIG_HELPER& Config )
{
	//
	// Load the server's certificate from the default certificate store,
	// using the provided certificate hash.
	//
	uint32_t CertHashLen =
		DecodeHexBuffer(
			pCertHash,
			sizeof( Config.CertHash.ShaHash ),
			Config.CertHash.ShaHash );
	if ( CertHashLen != sizeof( Config.CertHash.ShaHash ) )
	{
		return false;
	}
	Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_HASH;
	Config.CredConfig.CertificateHash = &Config.CertHash;

	return true;
}

bool
QuicDriver::SetupCredentialCertFile( const char* pCertFile, const char* pKeyFile, const char* pPassword, QUIC_CREDENTIAL_CONFIG_HELPER& Config )
{
	//
	// Loads the server's certificate from the file.
	//
	if ( pPassword != nullptr )
	{
		Config.CertFileProtected.CertificateFile = (char*)pCertFile;
		Config.CertFileProtected.PrivateKeyFile = (char*)pKeyFile;
		Config.CertFileProtected.PrivateKeyPassword = (char*)pPassword;
		Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE_PROTECTED;
		Config.CredConfig.CertificateFileProtected = &Config.CertFileProtected;
	}
	else
	{
		Config.CertFile.CertificateFile = (char*)pCertFile;
		Config.CertFile.PrivateKeyFile = (char*)pKeyFile;
		Config.CredConfig.Type = QUIC_CREDENTIAL_TYPE_CERTIFICATE_FILE;
		Config.CredConfig.CertificateFile = &Config.CertFile;
	}

    return true;
}

uint32_t
QuicDriver::DecodeHexBuffer( const char* HexBuffer, uint32_t OutBufferLen, uint8_t* OutBuffer )
{
    uint32_t HexBufferLen = (uint32_t)strlen( HexBuffer ) / 2;
    if ( HexBufferLen > OutBufferLen )
    {
        return 0;
    }

    for ( uint32_t i = 0; i < HexBufferLen; i++ )
    {
        OutBuffer[i] =
            ( DecodeHexChar( HexBuffer[i * 2] ) << 4 ) |
            DecodeHexChar( HexBuffer[i * 2 + 1] );
    }

    return HexBufferLen;
}

uint8_t
QuicDriver::DecodeHexChar( char c )
{
    if ( c >= '0' && c <= '9' ) return c - '0';
    if ( c >= 'A' && c <= 'F' ) return 10 + c - 'A';
    if ( c >= 'a' && c <= 'f' ) return 10 + c - 'a';
    return 0;
}
