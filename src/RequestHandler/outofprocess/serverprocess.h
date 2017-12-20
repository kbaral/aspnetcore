// Copyright (c) .NET Foundation. All rights reserved.
// Licensed under the MIT License. See License.txt in the project root for license information.

#pragma once

#define MIN_PORT                                    1025
#define MAX_PORT                                    48000
#define MAX_RETRY                                   10
#define MAX_ACTIVE_CHILD_PROCESSES                  16
#define LOCALHOST                                   "127.0.0.1"
#define ASPNETCORE_PORT_STR                         L"ASPNETCORE_PORT"
#define ASPNETCORE_PORT_ENV_STR                     L"ASPNETCORE_PORT="
#define ASPNETCORE_APP_PATH_ENV_STR                 L"ASPNETCORE_APPL_PATH="
#define ASPNETCORE_APP_TOKEN_ENV_STR                L"ASPNETCORE_TOKEN="
#define ASPNETCORE_APP_PATH_ENV_STR                 L"ASPNETCORE_APPL_PATH="
#define HOSTING_STARTUP_ASSEMBLIES_ENV_STR          L"ASPNETCORE_HOSTINGSTARTUPASSEMBLIES"
#define HOSTING_STARTUP_ASSEMBLIES_NAME             L"ASPNETCORE_HOSTINGSTARTUPASSEMBLIES="
#define HOSTING_STARTUP_ASSEMBLIES_VALUE            L"Microsoft.AspNetCore.Server.IISIntegration"
#define ASPNETCORE_IIS_AUTH_ENV_STR                 L"ASPNETCORE_IIS_HTTPAUTH="
#define ASPNETCORE_IIS_AUTH_WINDOWS                 L"windows;"
#define ASPNETCORE_IIS_AUTH_BASIC                   L"basic;"
#define ASPNETCORE_IIS_AUTH_ANONYMOUS               L"anonymous;"
#define ASPNETCORE_IIS_AUTH_NONE                    L"none"

class PROCESS_MANAGER;

class SERVER_PROCESS
{
public:
    SERVER_PROCESS();

    HRESULT
        Initialize(
        _In_ PROCESS_MANAGER      *pProcessManager,
        _In_ STRU                 *pszProcessExePath,
        _In_ STRU                 *pszArguments,
        _In_ DWORD                 dwStartupTimeLimitInMS,
        _In_ DWORD                 dwShtudownTimeLimitInMS,
        _In_ BOOL                  fWindowsAuthEnabled,
        _In_ BOOL                  fBasicAuthEnabled,
        _In_ BOOL                  fAnonymousAuthEnabled,
        _In_ ENVIRONMENT_VAR_HASH* pEnvironmentVariables,
        _In_ BOOL                  fStdoutLogEnabled,
        _In_ STRU                 *pstruStdoutLogFile,
        _In_ STRU                 *pszAppPhysicalPath,
        _In_ STRU                 *pszAppPath,
        _In_ STRU                 *pszAppVirtualPath
        );

    HRESULT
    StartProcess( VOID );

    HRESULT
    SetWindowsAuthToken(
        _In_ HANDLE hToken,
        _Out_ LPHANDLE pTargeTokenHandle
    );

    BOOL
    IsReady(
        VOID
    )
    {
        return m_fReady;
    }

    VOID
    StopProcess(
        VOID
    );

    DWORD
    GetPort()
    {
        return m_dwPort;
    }

    VOID
    ReferenceServerProcess(
        VOID
    )
    {
        InterlockedIncrement(&m_cRefs);
    }

    VOID
    DereferenceServerProcess(
        VOID
    )
    {
        _ASSERT(m_cRefs != 0 );
        if (InterlockedDecrement(&m_cRefs) == 0)
        {
            delete this;
        }
    }

    virtual 
    ~SERVER_PROCESS();

    static
    VOID
    CALLBACK
    ProcessHandleCallback(
        _In_ PVOID  pContext,
        _In_ BOOL
    );

    HRESULT
    HandleProcessExit(
        VOID
    );

    FORWARDER_CONNECTION*
    QueryWinHttpConnection(
        VOID
    )
    {
        return m_pForwarderConnection;
    }

    LPCSTR
    QueryGuid()
    {
        return m_straGuid.QueryStr();
    };

    VOID
    SendSignal( 
        VOID
    );

private:

    BOOL 
    IsDebuggerIsAttached(
        VOID
    );

    HRESULT
    StopAllProcessesInJobObject(
        VOID
    );

    HRESULT
    SetupStdHandles(
        _Inout_ LPSTARTUPINFOW pStartupInfo
    );

    HRESULT
    CheckIfServerIsUp(
        _In_  DWORD       dwPort,
        _Out_ DWORD     * pdwProcessId,
        _Out_ BOOL      * pfReady
    );

    HRESULT 
    RegisterProcessWait(
        _In_ PHANDLE phWaitHandle,
        _In_ HANDLE  hProcessToWaitOn
    );

    HRESULT 
    GetChildProcessHandles(
        VOID
    );

    HRESULT
    SetupListenPort(
        ENVIRONMENT_VAR_HASH    *pEnvironmentVarTable
    );

    HRESULT
    SetupAppPath(
        ENVIRONMENT_VAR_HASH*   pEnvironmentVarTable
    );

    HRESULT
    SetupAppToken(
        ENVIRONMENT_VAR_HASH*   pEnvironmentVarTable
    );

    HRESULT
    InitEnvironmentVariablesTable(
        ENVIRONMENT_VAR_HASH**   pEnvironmentVarTable
    );

    HRESULT
    OutputEnvironmentVariables(
        MULTISZ*                pmszOutput,
        ENVIRONMENT_VAR_HASH*   pEnvironmentVarTable
    );

    HRESULT
    SetupCommandLine(
        STRU*    pstrCommandLine
    );

    HRESULT
    PostStartCheck(
        VOID
    );

    HRESULT
    GetRandomPort(
        DWORD*    pdwPickedPort,
        DWORD     dwExcludedPort
    );

    static
    VOID
    SendShutDownSignal(
        LPVOID lpParam
        );

    VOID
    SendShutDownSignalInternal(
        VOID
    );

    HRESULT
    SendShutdownHttpMessage(
        VOID
    );

    VOID
    TerminateBackendProcess(
        VOID
    );

    FORWARDER_CONNECTION   *m_pForwarderConnection;
    BOOL                    m_fStdoutLogEnabled;
    BOOL                    m_fWindowsAuthEnabled;
    BOOL                    m_fBasicAuthEnabled;
    BOOL                    m_fAnonymousAuthEnabled;

    STTIMER                 m_Timer;
    SOCKET                  m_socket;

    STRU                    m_struLogFile;
    STRU                    m_struFullLogFile;
    STRU                    m_ProcessPath;
    STRU                    m_Arguments;
    STRU                    m_struAppVirtualPath;  // e.g., '/' for site
    STRU                    m_struAppFullPath;     // e.g.,  /LM/W3SVC/4/ROOT/Inproc
    STRU                    m_struPhysicalPath;    // e.g., c:/test/mysite
    STRU                    m_struPort;
    STRU                    m_struCommandLine;

    volatile LONG           m_lStopping;
    volatile BOOL           m_fReady;
    mutable LONG            m_cRefs;

    DWORD                   m_dwPort;
    DWORD                   m_dwStartupTimeLimitInMS;
    DWORD                   m_dwShutdownTimeLimitInMS;
    DWORD                   m_cChildProcess;
    DWORD                   m_dwChildProcessIds[MAX_ACTIVE_CHILD_PROCESSES];
    DWORD                   m_dwProcessId;
    DWORD                   m_dwListeningProcessId;

    STRA                    m_straGuid;

    HANDLE                  m_hJobObject;
    HANDLE                  m_hStdoutHandle;
    //
    // m_hProcessHandle is the handle to process this object creates.
    //
    HANDLE                  m_hProcessHandle;
    HANDLE                  m_hListeningProcessHandle;
    HANDLE                  m_hProcessWaitHandle;
    HANDLE                  m_hShutdownHandle;
    //
    // m_hChildProcessHandle is the handle to process created by 
    // m_hProcessHandle process if it does.
    //
    HANDLE                  m_hChildProcessHandles[MAX_ACTIVE_CHILD_PROCESSES];
    HANDLE                  m_hChildProcessWaitHandles[MAX_ACTIVE_CHILD_PROCESSES];

    PROCESS_MANAGER         *m_pProcessManager;
    ENVIRONMENT_VAR_HASH    *m_pEnvironmentVarTable ;
};