// WinService.h
// CWinService -- Windows SCM service wrapper.
// All ShineEngine server processes register as Win32 services.
// Double-clicking the exe shows the <SERVICE UPLOAD ONLY OK> dialog.
// Source: E:\ProjectF2\Server\1SharedCode\win2000\winservice\winservice.h

#pragma once
#include <windows.h>
#include "typedef.h"

#define WINSERVICE_NAME_LEN		64
#define WINSERVICE_DESC_LEN		256

//------------------------------------------------------------------
// CWinService
// Subclass and implement Run() with the server main loop.
// Call Start() from main() -- it blocks until service stops.
//------------------------------------------------------------------
class CWinService
{
public:
	CWinService( const char* pszName, const char* pszDesc );
	virtual ~CWinService();

	// Call from main(). If running interactively shows dialog
	// ("<SERVICE UPLOAD ONLY OK>") then exits.
	// If started by SCM, registers handler and enters loop.
	int		Start( int argc, char** argv );

	// Install / uninstall from SCM (used by installer).
	bool	Install();
	bool	Uninstall();

	const char*	GetName() const { return m_szName; }
	const char*	GetDesc() const { return m_szDesc; }

protected:
	// Subclass implements the server loop here.
	// Return when shutdown is requested.
	virtual void	Run() = 0;

	// Called when SCM sends STOP / SHUTDOWN.
	virtual void	OnStop();

	// Called on SERVICE_CONTROL_PAUSE / RESUME if supported.
	virtual void	OnPause()  {}
	virtual void	OnResume() {}

	void	SetStatus( DWORD dwState, DWORD dwExitCode = NO_ERROR );
	bool	IsInteractive() const { return m_bInteractive; }

private:
	static void WINAPI	ServiceMain( DWORD argc, LPSTR* argv );
	static void WINAPI	ServiceCtrlHandler( DWORD dwCtrl );

	static CWinService*			s_pInstance;

	char						m_szName[ WINSERVICE_NAME_LEN ];
	char						m_szDesc[ WINSERVICE_DESC_LEN ];
	SERVICE_STATUS				m_status;
	SERVICE_STATUS_HANDLE		m_hStatus;
	bool						m_bInteractive;
};
