#include "Quic.h"
#include <stdio.h>
#include <stdlib.h>

Quic::Driver*			Quic::sm_pDriver = NULL;
std::string				Quic::sm_zTestCertFile = "mpicert.pem";
std::string				Quic::sm_zTestKeyFile = "key.pem";

void
Quic::S_SetDriver( Driver* pDriver )
{
	if ( NULL != sm_pDriver )
	{
		sm_pDriver->Fini();
		delete sm_pDriver;
	}

	sm_pDriver = pDriver;
}

void
Quic::S_RegisterDriver( Driver* pDriver )
{
	S_SetDriver( pDriver );

	if ( S_HasDriver() ) S_GetDriver()->Init();
}

void
Quic::S_DestroyDriver()
{
	S_SetDriver( NULL );

}

void
Quic::S_CreateListener( uint16_t iPort )
{
	if ( !S_HasDriver() ) return;
	S_GetDriver()->CreateListener( iPort );
}

void
Quic::S_CreateClient( const char* pTargetAddress, uint16_t iPort, bool bSecure )
{
	if ( !S_HasDriver() ) return;

	S_GetDriver()->CreateClient( pTargetAddress, iPort, bSecure, nullptr );
}

bool
Quic::S_IsListenerRunning()
{
	if ( !S_HasDriver() ) return false;

	return S_GetDriver()->IsListenerRunning();
}

bool
Quic::S_IsClientConnected()
{
	if ( !S_HasDriver() ) return false;

	return S_GetDriver()->IsClientConnected();
}

QUIC_STATUS QUIC_API
Quic::S_ServerStreamCallback( HQUIC Stream, void*, QUIC_STREAM_EVENT* Event )
{
	switch ( Event->Type )
	{
	case QUIC_STREAM_EVENT_SEND_COMPLETE:
		//
		// A previous StreamSend call has completed, and the context is being
		// returned back to the app.
		//
		free( Event->SEND_COMPLETE.ClientContext );
		printf( "[strm][%p] Data sent\n", Stream );
		break;
	case QUIC_STREAM_EVENT_RECEIVE:
		//
		// Data was received from the peer on the stream.
		//
		printf( "[strm][%p] Data received\n", Stream );
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
		//
		// The peer gracefully shut down its send direction of the stream.
		//
		printf( "[strm][%p] Peer shut down\n", Stream );
		S_GetDriver()->ServerSend( Stream );
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
		//
		// The peer aborted its send direction of the stream.
		//
		printf( "[strm][%p] Peer aborted\n", Stream );
		S_GetDriver()->GetMsQuic()->StreamShutdown( Stream, QUIC_STREAM_SHUTDOWN_FLAG_ABORT, 0 );
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		printf( "[strm][%p] All done\n", Stream );
		S_GetDriver()->GetMsQuic()->StreamClose( Stream );
		break;
	default:
		break;
	}
	return QUIC_STATUS_SUCCESS;
}

QUIC_STATUS QUIC_API
Quic::S_ServerConnectionCallback( HQUIC Connection, void*, QUIC_CONNECTION_EVENT* Event )
{
	if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
	switch ( Event->Type )
	{
	case QUIC_CONNECTION_EVENT_CONNECTED:
		//
		// The handshake has completed for the connection.
		//
		printf( "[conn][%p] Connected\n", Connection );
		S_GetDriver()->GetMsQuic()->ConnectionSendResumptionTicket( Connection, QUIC_SEND_RESUMPTION_FLAG_NONE, 0, nullptr );
		S_GetDriver()->SetListenerRunning( true );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		//
		// The connection has been shut down by the transport. Generally, this
		// is the expected way for the connection to shut down with this
		// protocol, since we let idle timeout kill the connection.
		//
		if ( Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE )
		{
			printf( "[conn][%p] Successfully shut down on idle.\n", Connection );
		}
		else
		{
			printf( "[conn][%p] Shut down by transport, 0x%x\n", Connection, Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status );
		}
		S_GetDriver()->SetListenerRunning( false );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		//
		// The connection was explicitly shut down by the peer.
		//
		printf( "[conn][%p] Shut down by peer, 0x%llu\n", Connection, ( unsigned long long )Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode );
		S_GetDriver()->SetListenerRunning( false );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
		//
		// The connection has completed the shutdown process and is ready to be
		// safely cleaned up.
		//
		printf( "[conn][%p] All done\n", Connection );
		S_GetDriver()->GetMsQuic()->ConnectionClose( Connection );
		break;
	case QUIC_CONNECTION_EVENT_PEER_STREAM_STARTED:
		if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
		//
		// The peer has started/created a new stream. The app MUST set the
		// callback handler before returning.
		//
		printf( "[strm][%p] Peer started\n", Event->PEER_STREAM_STARTED.Stream );
		S_GetDriver()->GetMsQuic()->SetCallbackHandler( Event->PEER_STREAM_STARTED.Stream, (void*)S_ServerStreamCallback, nullptr );
		break;
	case QUIC_CONNECTION_EVENT_RESUMED:
		//
		// The connection succeeded in doing a TLS resumption of a previous
		// connection's session.
		//
		printf( "[conn][%p] Connection resumed!\n", Connection );
		S_GetDriver()->SetListenerRunning( true );
		break;
	default:
		break;
	}
	return QUIC_STATUS_SUCCESS;
}

QUIC_STATUS QUIC_API
Quic::S_ServerListenerCallback( HQUIC, void*, QUIC_LISTENER_EVENT* Event )
{
	QUIC_STATUS Status = QUIC_STATUS_NOT_SUPPORTED;
	if ( !S_HasDriver() ) return Status;

	switch ( Event->Type )
	{
	case QUIC_LISTENER_EVENT_NEW_CONNECTION:
		//
		// A new connection is being attempted by a client. For the handshake to
		// proceed, the server must provide a configuration for QUIC to use. The
		// app MUST set the callback handler before returning.
		//
		S_GetDriver()->GetMsQuic()->SetCallbackHandler( Event->NEW_CONNECTION.Connection, (void*)S_ServerConnectionCallback, nullptr );
		Status = S_GetDriver()->GetMsQuic()->ConnectionSetConfiguration( Event->NEW_CONNECTION.Connection, S_GetDriver()->GetListenerConfiguration() );
		break;
	default:
		break;
	}
	return Status;
}

QUIC_STATUS QUIC_API
Quic::S_ClientStreamCallback( HQUIC Stream, void*, QUIC_STREAM_EVENT* Event )
{
	switch ( Event->Type )
	{
	case QUIC_STREAM_EVENT_SEND_COMPLETE:
		//
		// A previous StreamSend call has completed, and the context is being
		// returned back to the app.
		//
		free( Event->SEND_COMPLETE.ClientContext );
		printf( "[strm][%p] Data sent\n", Stream );
		break;
	case QUIC_STREAM_EVENT_RECEIVE:
		//
		// Data was received from the peer on the stream.
		//
		printf( "[strm][%p] Data received\n", Stream );
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
		//
		// The peer gracefully shut down its send direction of the stream.
		//
		printf( "[strm][%p] Peer aborted\n", Stream );
		break;
	case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
		//
		// The peer aborted its send direction of the stream.
		//
		printf( "[strm][%p] Peer shut down\n", Stream );
		break;
	case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
		//
		// Both directions of the stream have been shut down and MsQuic is done
		// with the stream. It can now be safely cleaned up.
		//
		printf( "[strm][%p] All done\n", Stream );
		if ( !Event->SHUTDOWN_COMPLETE.AppCloseInProgress )
		{
			S_GetDriver()->GetMsQuic()->StreamClose( Stream );
		}
		break;
	default:
		break;
	}
	return QUIC_STATUS_SUCCESS;
}

QUIC_STATUS QUIC_API
Quic::S_ClientConnectionCallback( HQUIC Connection, void*, QUIC_CONNECTION_EVENT* Event )
{
	if ( !S_HasDriver() ) return QUIC_STATUS_ABORTED;
	switch ( Event->Type )
	{
	case QUIC_CONNECTION_EVENT_CONNECTED:
		//
		// The handshake has completed for the connection.
		//
		printf( "[conn][%p] Connected\n", Connection );
		S_GetDriver()->ClientSend( Connection );
		S_GetDriver()->SetClientConnected( true );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
		//
		// The connection has been shut down by the transport. Generally, this
		// is the expected way for the connection to shut down with this
		// protocol, since we let idle timeout kill the connection.
		//
		if ( Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE )
		{
			printf( "[conn][%p] Successfully shut down on idle.\n", Connection );
		}
		else
		{
			printf( "[conn][%p] Shut down by transport, 0x%x\n", Connection, Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status );
		}
		S_GetDriver()->SetClientConnected( false );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
		//
		// The connection was explicitly shut down by the peer.
		//
		printf( "[conn][%p] Shut down by peer, 0x%llu\n", Connection, ( unsigned long long )Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode );
		S_GetDriver()->SetClientConnected( false );
		break;
	case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
		//
		// The connection has completed the shutdown process and is ready to be
		// safely cleaned up.
		//
		printf( "[conn][%p] All done\n", Connection );
		if ( !Event->SHUTDOWN_COMPLETE.AppCloseInProgress )
		{
			S_GetDriver()->GetMsQuic()->ConnectionClose( Connection );
		}
		S_GetDriver()->SetClientConnected( false );
		break;
	case QUIC_CONNECTION_EVENT_RESUMPTION_TICKET_RECEIVED:
		//
		// A resumption ticket (also called New Session Ticket or NST) was
		// received from the server.
		//
		printf( "[conn][%p] Resumption ticket received (%u bytes):\n", Connection, Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength );
		for ( uint32_t i = 0; i < Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength; i++ )
		{
			printf( "%.2X", (uint8_t)Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicket[i] );
		}
		printf( "\n" );
		S_GetDriver()->SetClientConnected( true );
		break;
	default:
		break;
	}
	return QUIC_STATUS_SUCCESS;
}
