#pragma once

#include "Quic.h"

class QuicDriver :		public Quic::Driver
{
public:
	QuicDriver();
	virtual ~QuicDriver();

	virtual void		Init() override;
	virtual void		Fini() override;

	virtual void		CreateListener( uint16_t iPort ) override;
	virtual bool		ServerLoadConfiguration( const char* pCertHash ) override;
	virtual bool		ServerLoadConfiguration( const char* pCertFile, const char* pKeyFile, const char* pPassword = nullptr ) override;
	virtual void		ServerSend( HQUIC Stream ) override;

	virtual void		CreateClient( const char* pTargetAddress, uint16_t iPort, bool bUnsecure, const char* pResumptionTicket ) override;
	virtual bool		ClientLoadConfiguration( bool bUnsecure ) override;
	virtual void		ClientSend( HQUIC Connection ) override;

	virtual HQUIC		GetRegistration() override { return m_Registration; }
	virtual HQUIC		GetListenerConfiguration() override { return m_ListenerConfiguration; }
	virtual const		QUIC_API_TABLE* GetMsQuic() override { return m_MsQuic; }
	virtual const		QUIC_BUFFER GetAlpn() override { return m_Alpn; }

	virtual void		SetListenerRunning( bool bIsRunning ) override { m_bListenerRunning = bIsRunning; }
	virtual bool		IsListenerRunning() override { return m_bListenerRunning; }

	virtual void		SetClientConnected( bool bIsConnected ) override { m_bClientConnected = bIsConnected; }
	virtual bool		IsClientConnected() override { return m_bClientConnected; }

private:
	void				CloseListener( HQUIC Listener );
	void				CloseClientConnection( HQUIC Connection );
	void				ShutdownClientConnection( HQUIC Connection );
	bool				SetupCredentialCertHash( const char* pCertHash, QUIC_CREDENTIAL_CONFIG_HELPER& config );
	bool				SetupCredentialCertFile( const char* pCertFile, const char* pKeyFile, const char* pPassword, QUIC_CREDENTIAL_CONFIG_HELPER& config );

	//
	// Helper function to convert a string of hex characters to a byte buffer.
	//
	uint32_t			DecodeHexBuffer( _In_z_ const char* HexBuffer, _In_ uint32_t OutBufferLen, _Out_writes_to_( OutBufferLen, return ) uint8_t* OutBuffer );
	//
	// Helper function to convert a hex character to its decimal value.
	//
	uint8_t				DecodeHexChar( char c );

private:
	//
	// The (optional) registration configuration for the app. This sets a name for
	// the app (used for persistent storage and for debugging). It also configures
	// the execution profile, using the default "low latency" profile.
	//
	const QUIC_REGISTRATION_CONFIG				m_RegConfig = { "QuicMpiSandbox", QUIC_EXECUTION_PROFILE_LOW_LATENCY };

	//
	// The protocol name used in the Application Layer Protocol Negotiation (ALPN).
	//
	const QUIC_BUFFER							m_Alpn = { sizeof( "sample" ) - 1, (uint8_t*)"sample" };

	//
	// The default idle timeout period (1 second) used for the protocol.
	//
	const uint64_t								m_IdleTimeoutMs = 1000;

	//
	// The length of buffer sent over the streams in the protocol.
	//
	const uint32_t								m_SendBufferLength = 100;

	//
	// The QUIC API/function table returned from MsQuicOpen2. It contains all the
	// functions called by the app to interact with MsQuic.
	//
	const QUIC_API_TABLE*						m_MsQuic;

	//
	// The QUIC handle to the registration object. This is the top level API object
	// that represents the execution context for all work done by MsQuic on behalf
	// of the app.
	//
	HQUIC										m_Registration;

	//
	// The QUIC handle to the configuration object. This object abstracts the
	// connection configuration. This includes TLS configuration and any other
	// QUIC layer settings.
	//
	HQUIC										m_ListenerConfiguration;
	HQUIC										m_ClientConfiguration;

	HRESULT										m_ListenerStatus;
	HRESULT										m_ClientStatus;

	bool										m_bListenerRunning;
	bool										m_bClientConnected;
};
