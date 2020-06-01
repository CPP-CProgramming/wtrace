// Utility to log Windows API calls made by an executable (similar to Unix strace)
// Based on samples bundled with "Detours Express 2.1"


//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (syelog.h of syelog.lib)
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
#pragma once
#ifndef _SYELOGD_H_
#define _SYELOGD_H_
#include <stdarg.h>

#define SYELOG_FILE_NAME       L"wtrace.log"

// Facility Codes.
//
#define SYELOG_FACILITY_APPLICATION     0x50            // User-mode application

// Severity Codes.
//
#define SYELOG_SEVERITY_FATAL           0x00            // System is dead.
#define SYELOG_SEVERITY_NOTICE          0x50            // Significant condition.
#define SYELOG_SEVERITY_INFORMATION     0x60            // Informational

// Logging Functions.
//
VOID SyelogOpen(PCSTR pszIdentifier, BYTE nFacility);
VOID Syelog(BYTE nSeverity, PCSTR pszMsgf, ...);
VOID SyelogV(BYTE nSeverity, PCSTR pszMsgf, va_list args);
VOID SyelogClose(BOOL fTerminate);

#endif //  _SYELOGD_H_
//
///////////////////////////////////////////////////////////////// End of File.
