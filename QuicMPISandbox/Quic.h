#pragma once

#include "msquic.h"
#include <string>

#include "DataPacket.h"

#include "TimeKeeper.h"

typedef struct QUIC_CREDENTIAL_CONFIG_HELPER
{
    QUIC_CREDENTIAL_CONFIG CredConfig;
    union
    {
        QUIC_CERTIFICATE_HASH CertHash;
        QUIC_CERTIFICATE_HASH_STORE CertHashStore;
        QUIC_CERTIFICATE_FILE CertFile;
        QUIC_CERTIFICATE_FILE_PROTECTED CertFileProtected;
    };
} QUIC_CREDENTIAL_CONFIG_HELPER;

class Quic
{
public:
	class Driver
	{
	public:
		Driver() {};
		virtual ~Driver() {};

		virtual void		Init() = 0;
		virtual void		Fini() = 0;

		virtual void		CreateListener( uint16_t iPort ) = 0;
		virtual bool		ServerLoadConfiguration( const char* pCertHash ) = 0;
		virtual bool		ServerLoadConfiguration( const char* pCertFile, const char* pKeyFile, const char* pPassword = nullptr ) = 0;
		virtual void		ServerSend( HQUIC Stream ) = 0;

		virtual void		CreateClient( const char* pTargetAddress, uint16_t iPort, bool bUnsecure, const char* pResumptionTicket ) = 0;
		virtual bool		ClientLoadConfiguration( bool bUnsecure ) = 0;
		virtual void		ClientSend( HQUIC Connection ) = 0;
		virtual void		ClientSendData( const std::string zDataBuffer ) = 0;
		//virtual void		ClientSendData( HQUIC Stream, const std::string zDataBuffer ) = 0;

		virtual HQUIC		CreateStream() = 0;
		virtual bool		CloseStream( HQUIC Stream ) = 0;

		virtual HQUIC		GetRegistration() = 0;
		virtual HQUIC		GetListenerConfiguration() = 0;
		virtual const		QUIC_API_TABLE* GetMsQuic() = 0;
		virtual const		QUIC_BUFFER GetAlpn() = 0;

		virtual void		SetListenerRunning( bool bIsRunning ) = 0;
		virtual bool		IsListenerRunning() = 0;

		virtual void		SetClientConnected( bool bIsConnected ) = 0;
		virtual bool		IsClientConnected() = 0;
		virtual void		SetClientStreamReady( bool bIsReady ) = 0;
		virtual bool		IsClientStreamReady() = 0;

		virtual void		IncrementOpenStreamCount() = 0;
		virtual void		DecrementOpenStreamCount() = 0;
		virtual bool		HasOpenStreams() = 0;

		virtual void		StoreStreamData( HQUIC Stream, int iBufferCount, const QUIC_BUFFER* pIncBuffers ) = 0;
		virtual void		ProcessData( HQUIC Stream ) = 0;
	};

public:
	static void				S_RegisterDriver( Driver* pDriver );
	static bool				S_HasDriver() { return NULL != sm_pDriver; };
	static Driver*			S_GetDriver() { return sm_pDriver; };
	static void				S_DestroyDriver();

	static void				S_CreateListener( uint16_t iPort );
	static void				S_CreateClient( const char* pTargetAddress, uint16_t iPort, bool bSecure = true );

	static bool				S_IsListenerRunning();
	static bool				S_IsClientConnected();
	static bool				S_HasOpenStreams();

	static std::string		S_GetTestCertFileName() { return sm_zTestCertFile; }
	static std::string		S_GetTestKeyFileName() { return sm_zTestKeyFile; }
	static std::string		S_GetTestPassPhrase() { return sm_zTestPassPhrase; }


	static void				S_StoreTime( std::string zName, double dElapsedMs );
	static void				S_SaveStoredTimesToFile( std::string zFileName );


	//
	// The server's callback for stream events from MsQuic.
	//
	static
	_IRQL_requires_max_( DISPATCH_LEVEL )
	_Function_class_( QUIC_STREAM_CALLBACK )
	QUIC_STATUS
	QUIC_API
	S_ServerStreamCallback( HQUIC Stream, void*, QUIC_STREAM_EVENT* Event );

	//
	// The server's callback for connection events from MsQuic.
	//
	static
	_IRQL_requires_max_( DISPATCH_LEVEL )
	_Function_class_( QUIC_CONNECTION_CALLBACK )
	QUIC_STATUS
	QUIC_API
	S_ServerConnectionCallback(
		_In_ HQUIC Connection,
		_In_opt_ void* /*Context*/,
		_Inout_ QUIC_CONNECTION_EVENT* Event
	);

	//
	// The server's callback for listener events from MsQuic.
	//
	static
	_IRQL_requires_max_( PASSIVE_LEVEL )
	_Function_class_( QUIC_LISTENER_CALLBACK )
	QUIC_STATUS
	QUIC_API
	S_ServerListenerCallback(
		_In_ HQUIC /*Listener*/,
		_In_opt_ void* /*Context*/,
		_Inout_ QUIC_LISTENER_EVENT* Event );

	//
	// The clients's callback for stream events from MsQuic.
	//
	static
	_IRQL_requires_max_( DISPATCH_LEVEL )
	_Function_class_( QUIC_STREAM_CALLBACK )
	QUIC_STATUS
	QUIC_API
	S_ClientStreamCallback(
		_In_ HQUIC Stream,
		_In_opt_ void* /*Context*/,
		_Inout_ QUIC_STREAM_EVENT* Event
	);

	//
	// The clients's callback for connection events from MsQuic.
	//
	static
	_IRQL_requires_max_( DISPATCH_LEVEL )
	_Function_class_( QUIC_CONNECTION_CALLBACK )
	QUIC_STATUS
	QUIC_API
	S_ClientConnectionCallback(
		_In_ HQUIC Connection,
		_In_opt_ void* /*Context*/,
		_Inout_ QUIC_CONNECTION_EVENT* Event
	);

private:
	static void				S_SetDriver( Driver* pDriver );

private:
	static Driver*			sm_pDriver;
	static std::string		sm_zTestCertFile;
	static std::string		sm_zTestKeyFile;
	static std::string		sm_zTestPassPhrase;

	static TimeKeeper		sm_TimeKeeper;
};
