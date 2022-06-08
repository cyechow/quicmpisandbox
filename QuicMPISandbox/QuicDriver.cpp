#include "QuicDriver.h"

#include <stdio.h>
#include <stdlib.h>

#include <string>

QuicDriver::QuicDriver() :
	m_MsQuic( nullptr ),
	m_Registration( nullptr ),
	m_ListenerConfiguration( nullptr ),
	m_ClientConfiguration( nullptr ),
	m_ListenerStatus( QUIC_STATUS_NOT_FOUND ),
	m_ClientStatus( QUIC_STATUS_NOT_FOUND ),
	m_bListenerRunning( false ),
	m_bClientConnected( false )
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
	HQUIC Listener = nullptr;
	//
	// Configures the address used for the listener to listen on all IP
	// addresses and the given UDP port.
	//
	QUIC_ADDR Address{ 0 };
	QuicAddrSetFamily( &Address, QUIC_ADDRESS_FAMILY_UNSPEC );
	QuicAddrSetPort( &Address, iPort );

	std::string zCertFile = Quic::S_GetTestCertFileName();
	std::string zKeyFile = Quic::S_GetTestKeyFileName();
	printf( "%s: Cert - %s. Key - %s\n", __FUNCTION__, zCertFile.c_str(), zKeyFile.c_str() );
	if ( !ServerLoadConfiguration( zCertFile.c_str(), zKeyFile.c_str() ) ) return;

	//
	// Create/allocate a new listener object.
	//
	if ( QUIC_FAILED( m_ListenerStatus = GetMsQuic()->ListenerOpen( GetRegistration(), Quic::S_ServerListenerCallback, nullptr, &Listener ) ) )
	{
		printf( "ListenerOpen failed, 0x%x!\n", m_ListenerStatus );
		CloseListener( Listener );
		Fini();
		return;
	}

	//
	// Starts listening for incoming connections.
	//
	if ( QUIC_FAILED( m_ListenerStatus = GetMsQuic()->ListenerStart( Listener, &m_Alpn, 1, &Address ) ) )
	{
		printf( "ListenerStart failed, 0x%x!\n", m_ListenerStatus );
		CloseListener( Listener );
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
	auto SendBufferRaw = malloc( sizeof( QUIC_BUFFER ) + m_SendBufferLength );
	if ( SendBufferRaw == nullptr )
	{
		printf( "SendBuffer allocation failed!\n" );
		m_MsQuic->StreamShutdown( Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0 );
		return;
	}
	auto SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	SendBuffer->Buffer = (uint8_t*)SendBufferRaw + sizeof( QUIC_BUFFER );
	SendBuffer->Length = m_SendBufferLength;

	printf( "[strm][%p] Sending data...\n", Stream );

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	QUIC_STATUS Status;
	if ( QUIC_FAILED( Status = m_MsQuic->StreamSend( Stream, SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer ) ) )
	{
		printf( "StreamSend failed, 0x%x!\n", Status );
		free( SendBufferRaw );
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

	HQUIC clientConnection;

	//
	// Allocate a new connection object.
	//
	if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->ConnectionOpen( m_Registration, Quic::S_ClientConnectionCallback, nullptr, &clientConnection ) ) )
	{
		printf( "ConnectionOpen failed, 0x%x!\n", m_ClientStatus );
		CloseClientConnection( clientConnection );
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
		if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->SetParam( clientConnection, QUIC_PARAM_CONN_RESUMPTION_TICKET, TicketLength, ResumptionTicket ) ) )
		{
			printf( "SetParam(QUIC_PARAM_CONN_RESUMPTION_TICKET) failed, 0x%x!\n", m_ClientStatus );
			CloseClientConnection( clientConnection );
			return;
		}
	}

	//
	// Get the target / server name or IP from the command line.
	//
	if ( pTargetAddress == nullptr )
	{
		m_ClientStatus = QUIC_STATUS_INVALID_PARAMETER;
		CloseClientConnection( clientConnection );
		return;
	}

	printf( "[conn][%p] Connecting...\n", clientConnection );

	//
	// Start the connection to the server.
	//
	if ( QUIC_FAILED( m_ClientStatus = m_MsQuic->ConnectionStart( clientConnection, m_ClientConfiguration, QUIC_ADDRESS_FAMILY_UNSPEC, pTargetAddress, iPort ) ) )
	{
		printf( "ConnectionStart failed, 0x%x!\n", m_ClientStatus );
		CloseClientConnection( clientConnection );
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
	uint8_t* SendBufferRaw;
	QUIC_BUFFER* SendBuffer;

	//
	// Create/allocate a new bidirectional stream. The stream is just allocated
	// and no QUIC stream identifier is assigned until it's started.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamOpen( Connection, QUIC_STREAM_OPEN_FLAG_NONE, Quic::S_ClientStreamCallback, nullptr, &Stream ) ) )
	{
		printf( "StreamOpen failed, 0x%x!\n", Status );
		ShutdownClientConnection( Connection );
		return;
	}

	printf( "[strm][%p] Starting...\n", Stream );

	//
	// Starts the bidirectional stream. By default, the peer is not notified of
	// the stream being started until data is sent on the stream.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamStart( Stream, QUIC_STREAM_START_FLAG_NONE ) ) )
	{
		printf( "StreamStart failed, 0x%x!\n", Status );
		m_MsQuic->StreamClose( Stream );
		ShutdownClientConnection( Connection );
		return;
	}

	//
	// Allocates and builds the buffer to send over the stream.
	//
	SendBufferRaw = (uint8_t*)malloc( sizeof( QUIC_BUFFER ) + m_SendBufferLength );
	if ( SendBufferRaw == nullptr )
	{
		printf( "SendBuffer allocation failed!\n" );
		Status = QUIC_STATUS_OUT_OF_MEMORY;
		ShutdownClientConnection( Connection );
		return;
	}
	SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
	SendBuffer->Buffer = SendBufferRaw + sizeof( QUIC_BUFFER );
	SendBuffer->Length = m_SendBufferLength;

	printf( "[strm][%p] Sending data...\n", Stream );

	//
	// Sends the buffer over the stream. Note the FIN flag is passed along with
	// the buffer. This indicates this is the last buffer on the stream and the
	// the stream is shut down (in the send direction) immediately after.
	//
	if ( QUIC_FAILED( Status = m_MsQuic->StreamSend( Stream, SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer ) ) )
	{
		printf( "StreamSend failed, 0x%x!\n", Status );
		free( SendBufferRaw );
		ShutdownClientConnection( Connection );
		return;
	}
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
