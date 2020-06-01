// Utility to log Windows API calls made by an executable (similar to Unix strace)
// Based on samples bundled with "Detours Express 2.1"

//////////////////////////////////////////////////////////////////////////////
//
//  Test DetourCreateProcessWithDll function (withdll.cpp).
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <detours.h>

#define arrayof(x)      (sizeof(x)/sizeof(x[0]))

#define APPNAME "wtrace.exe"

//////////////////////////////////////////////////////////////////////////////
//
void PrintUsage(void)
{
    printf("Usage:\n"
           "    " APPNAME " [command line]\n"
           "Log Windows API calls executing [command line] to \"wtrace.log\".\n"
    );
}

//////////////////////////////////////////////////////////////////////////////
//
//  This code verifies that the named DLL has been configured correctly
//  to be imported into the target process.  DLLs must export a function with
//  ordinal #1 so that the import table touch-up magic works.
//

struct ExportContext
{
    BOOL    fHasOrdinal1;
    ULONG   nExports;
};


static BOOL CALLBACK ExportCallback(_In_opt_ PVOID pContext,
                                    _In_ ULONG nOrdinal,
                                    _In_opt_ LPCSTR pszSymbol,
                                    _In_opt_ PVOID pbTarget)
{
    (void)pContext;
    (void)pbTarget;
    (void)pszSymbol;

    ExportContext *pec = (ExportContext *)pContext;

    if (nOrdinal == 1) {
        pec->fHasOrdinal1 = TRUE;
    }
    pec->nExports++;

    return TRUE;
}


BOOL DoesDllExportOrdinal1(PCHAR pszDllPath)
{
    HMODULE hDll = LoadLibraryEx(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hDll == NULL) {
        printf(APPNAME ": LoadLibraryEx(%s) failed with error %d.\n",
               pszDllPath,
               GetLastError());
        return FALSE;
    }

    BOOL validFlag = FALSE;
    ExportContext ec;
    ec.fHasOrdinal1 = FALSE;
    ec.nExports = 0;
    DetourEnumerateExports(hDll, &ec, ExportCallback);
    FreeLibrary(hDll);
    return ec.fHasOrdinal1;
}

//////////////////////////////////////////////////////////////////////// main.
//
int CDECL main(int argc, char **argv)
{
    BOOLEAN fNeedHelp = FALSE;

    int arg = 1;
    for (; arg < argc && (argv[arg][0] == '-' || argv[arg][0] == '/'); arg++) {

        CHAR *argn = argv[arg] + 1;
        CHAR *argp = argn;
        while (*argp && *argp != ':')
            argp++;
        if (*argp == ':')
            *argp++ = '\0';

        switch (argn[0]) {
          case '?':                                     // Help
            fNeedHelp = TRUE;
            break;

          default:
            fNeedHelp = TRUE;
            printf(APPNAME ": Bad argument: %s\n", argv[arg]);
            break;
        }
    }

    if (arg >= argc) {
        fNeedHelp = TRUE;
    }

    if (fNeedHelp) {
        PrintUsage();
        return 9001;
    }

    /////////////////////////////////////////////////////////// Validate DLLs.
    //
    CHAR szDllPath[1024];
    CHAR szDetouredDllPath[1024];
    PCHAR pszFilePart = NULL;

    if (!SearchPath(NULL, "traceapi.dll", NULL,
                    arrayof(szDllPath),
                    szDllPath,
                    &pszFilePart)) {
        printf(APPNAME ": Couldn't find traceapi.dll.\n");
        return 9002;
    }

#if 0    
    PCHAR pszDllPath = "traceapi.dll";
    if (!GetFullPathName(pszDllPath, arrayof(szDllPath), szDllPath, &pszFilePart)) {
        printf(APPNAME ": Error: %s is not a valid path name..\n",
               pszDllPath);
        return 9002;
    }
#endif
    if (!DoesDllExportOrdinal1(szDllPath)) {
        printf(APPNAME ": Error: %s does not export function with ordinal #1.\n",
               szDllPath);
        return 9003;
    }

    // HMODULE hDetouredDll = DetourGetDetouredMarker();
    // GetModuleFileName(hDetouredDll,
    //                   szDetouredDllPath, arrayof(szDetouredDllPath));
// #if 0
//     if (!SearchPath(NULL, "detoured.dll", NULL,
//                     arrayof(szDetouredDllPath),
//                     szDetouredDllPath,
//                     &pszFilePart)) {
//         printf(APPNAME ": Couldn't find Detoured.DLL.\n");
//         return 9006;
//     }
// #endif

    //////////////////////////////////////////////////////////////////////////
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    CHAR szCommand[2048];
    CHAR szExe[1024];
    CHAR szFullExe[1024] = "\0";
    PCHAR pszFileExe = NULL;
    LPCSTR rpszDllsOut[256];
    DWORD nDlls = 1;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    szCommand[0] = L'\0';

#ifdef _CRT_INSECURE_DEPRECATE
    strcpy_s(szExe, sizeof(szExe), argv[arg]);
#else
    strcpy(szExe, argv[arg]);
#endif
    for (; arg < argc; arg++) {
        if (strchr(argv[arg], ' ') != NULL || strchr(argv[arg], '\t') != NULL) {
#ifdef _CRT_INSECURE_DEPRECATE
            strcat_s(szCommand, sizeof(szCommand), "\"");
            strcat_s(szCommand, sizeof(szCommand), argv[arg]);
            strcat_s(szCommand, sizeof(szCommand), "\"");
#else
            strcat(szCommand, "\"");
            strcat(szCommand, argv[arg]);
            strcat(szCommand, "\"");
#endif
        }
        else {
#ifdef _CRT_INSECURE_DEPRECATE
            strcat_s(szCommand, sizeof(szCommand), argv[arg]);
#else
            strcat(szCommand, argv[arg]);
#endif
        }

        if (arg + 1 < argc) {
#ifdef _CRT_INSECURE_DEPRECATE
            strcat_s(szCommand, sizeof(szCommand), " ");
#else
            strcat(szCommand, " ");
#endif
        }
    }
    printf(APPNAME ": Starting: `%s'\n", szCommand);
    printf(APPNAME ":   with `%s'\n", szDllPath);
    // printf(APPNAME ":   marked by `%s'\n", szDetouredDllPath);
    fflush(stdout);
    // DWORD c = (DWORD)strlen(szDllPath) + 1;
    // PCHAR psz = new CHAR [c];
    // StringCchCopyA(psz, c, szDllPath);
    // rpszDllsOut[0] = psz;
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

    SetLastError(0);
    SearchPath(NULL, szExe, ".exe", arrayof(szFullExe), szFullExe, &pszFileExe);
    if (!DetourCreateProcessWithDll(szFullExe[0] ? szFullExe : NULL, szCommand,
                                    NULL, NULL, TRUE, dwFlags, NULL, NULL,
                                    &si, &pi, szDllPath, NULL)) {
        printf(APPNAME ": DetourCreateProcessWithDll failed: %d\n", GetLastError());
        ExitProcess(9009);
    }

    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
        printf(APPNAME ": GetExitCodeProcess failed: %d\n", GetLastError());
        return 9010;
    }

    return dwResult;
}
//
///////////////////////////////////////////////////////////////// End of File.
