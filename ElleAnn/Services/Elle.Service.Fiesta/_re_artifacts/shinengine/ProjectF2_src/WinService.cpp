// WinService.cpp
// CWinService implementation.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\winservice\winservice.cpp

#include "stdafx.h"
#include "WinService.h"
#include <stdio.h>

CWinService* CWinService::s_pInstance = NULL;

CWinService::CWinService( const char* pszName, const char* pszDesc )
	: m_hStatus(NULL), m_bInteractive(false)
{
	strncpy_s( m_szName, sizeof(m_szName), pszName, _TRUNCATE );
	strncpy_s( m_szDesc, sizeof(m_szDesc), pszDesc ? pszDesc : "", _TRUNCATE );

	ZeroMemory( &m_status, sizeof(m_status) );
	m_status.dwServiceType      = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	m_status.dwCurrentState     = SERVICE_STOPPED;

	s_pInstance = this;
}

CWinService::~CWinService()
{
	s_pInstance = NULL;
}

int CWinService::Start( int argc, char** argv )
{
	// Check for command-line install / uninstall
	if( argc >= 2 )
	{
		if( _stricmp( argv[1], "/install" ) == 0 )
		{
			Install();
			return 0;
		}
		if( _stricmp( argv[1], "/uninstall" ) == 0 )
		{
			Uninstall();
			return 0;
		}
	}

	// Detect interactive (double-click) vs SCM launch
	// If no SCM environment, show dialog and exit.
	if( !IsDebuggerPresent() )
	{
		DWORD dwType = 0;
		// A process started by SCM has a service type set;
		// detect by attempting to open the SCM.
		SC_HANDLE hSCM = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
		if( hSCM )
		{
			SC_HANDLE hSvc = OpenServiceA( hSCM, m_szName, SERVICE_QUERY_STATUS );
			if( !hSvc )
			{
				// Not registered or not started by SCM -- interactive launch
				CloseServiceHandle( hSCM );
				MessageBoxA( NULL,
				             "<SERVICE UPLOAD ONLY OK>",
				             m_szName,
				             MB_OK | MB_ICONINFORMATION );
				return 0;
			}
			CloseServiceHandle( hSvc );
			CloseServiceHandle( hSCM );
		}
	}

	SERVICE_TABLE_ENTRYA st[] =
	{
		{ m_szName, ServiceMain },
		{ NULL, NULL }
	};

	if( !StartServiceCtrlDispatcherA( st ) )
	{
		DWORD dwErr = GetLastError();
		if( dwErr == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT )
		{
			// Running interactively (e.g. debugger)
			m_bInteractive = true;
			SetStatus( SERVICE_RUNNING );
			Run();
			SetStatus( SERVICE_STOPPED );
		}
		return (int)dwErr;
	}

	return 0;
}

void WINAPI CWinService::ServiceMain( DWORD argc, LPSTR* argv )
{
	if( !s_pInstance ) return;
	CWinService* pSvc = s_pInstance;

	pSvc->m_hStatus = RegisterServiceCtrlHandlerA(
	    pSvc->m_szName, ServiceCtrlHandler );

	if( !pSvc->m_hStatus ) return;

	pSvc->SetStatus( SERVICE_START_PENDING );
	pSvc->SetStatus( SERVICE_RUNNING );
	pSvc->Run();
	pSvc->SetStatus( SERVICE_STOPPED );
}

void WINAPI CWinService::ServiceCtrlHandler( DWORD dwCtrl )
{
	if( !s_pInstance ) return;
	CWinService* pSvc = s_pInstance;

	switch( dwCtrl )
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		pSvc->SetStatus( SERVICE_STOP_PENDING );
		pSvc->OnStop();
		break;

	case SERVICE_CONTROL_PAUSE:
		pSvc->OnPause();
		break;

	case SERVICE_CONTROL_CONTINUE:
		pSvc->OnResume();
		break;

	case SERVICE_CONTROL_INTERROGATE:
		pSvc->SetStatus( pSvc->m_status.dwCurrentState );
		break;

	default:
		break;
	}
}

void CWinService::SetStatus( DWORD dwState, DWORD dwExitCode )
{
	m_status.dwCurrentState  = dwState;
	m_status.dwWin32ExitCode = dwExitCode;

	if( m_hStatus )
		SetServiceStatus( m_hStatus, &m_status );
}

void CWinService::OnStop()
{
	// Default: nothing. Subclass signals main loop to exit.
}

bool CWinService::Install()
{
	char szPath[MAX_PATH];
	GetModuleFileNameA( NULL, szPath, MAX_PATH );

	SC_HANDLE hSCM = OpenSCManager( NULL, NULL,
	                                SC_MANAGER_CREATE_SERVICE );
	if( !hSCM ) return false;

	SC_HANDLE hSvc = CreateServiceA(
		hSCM,
		m_szName,
		m_szDesc,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szPath,
		NULL, NULL, NULL, NULL, NULL );

	bool bOK = ( hSvc != NULL );
	if( hSvc ) CloseServiceHandle( hSvc );
	CloseServiceHandle( hSCM );
	return bOK;
}

bool CWinService::Uninstall()
{
	SC_HANDLE hSCM = OpenSCManager( NULL, NULL,
	                                SC_MANAGER_CONNECT );
	if( !hSCM ) return false;

	SC_HANDLE hSvc = OpenServiceA( hSCM, m_szName, DELETE );
	bool bOK = false;
	if( hSvc )
	{
		bOK = ( DeleteService( hSvc ) != FALSE );
		CloseServiceHandle( hSvc );
	}
	CloseServiceHandle( hSCM );
	return bOK;
}
