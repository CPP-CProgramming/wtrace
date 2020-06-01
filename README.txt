This software was originally distributed as
wtrace.exe
[downloaded from http://www.ovpworld.org]

However, this version has been modified to run over the latest version of Detour package released on Dec. 10th, 2019.
This software is built over the visual studio 2019 and run without any issue at Windows 10.
If any of you has found a bug, send me a feedback to neogeoss1@gmail.com


SUMMARY:

This directory contains a utility to log Windows API calls made by a Windows
application much as the "strace" utility logs system calls for Linux programs.

The tool is based on samples bundled with Microsoft Detours Package 4.0.
(see withdll, traceapi and syelog samples).


LICENSING:

THIS SOFTWARE IS PROVIDED "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF 
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST 
INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE 
OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL 
ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS DISCLAIMER
ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.

This software is based heavily on Microsoft Detours and as such its use
indicates acceptance of the Microsoft Research Shared Source License Agreement
(Non-commercial Use Only). See the included file "Microsoft_Detours_EULA.rtf"
for more information.


USAGE:

  Usage: bin\wtrace.exe [command line]

The utility should be run in a Windows command-prompt window. The command line
given contains the executable name and any arguments of the application to be
monitored.

The API call log is written to "wtrace.log"

The log file has two lines for each API call:

<thread-number>  <API-function> ( <arguments> )
...
<thread-number>  <API-function> ( ) -> <result>

Any nested/internal API calls are shown indented between the two lines for a call
made by the application. For example:

001 SystemParametersInfoW(42,c,7f7bc,0)
001   LocalAlloc(40,100)
001   LocalAlloc(,) -> b7418
001 SystemParametersInfoW(,,,) -> 1


FILES:

The package containing this README contains the executable harness as well
as the source needed to rebuild the it.

The harness was built in a command prompt window using Microsoft's NMAKE 
and Visual Studio tools. See the comments in the Makefile for information
on rebuilding.
