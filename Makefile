# nmake Makefile to build Windows wtrace utility
#
# Prepare environment for Visual Studio tools
#
#  C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat
#
# Add Windows SDK paths etc.
#
#  sdkvars.bat
#

DETOURS = detours

CFLAGS = /nologo /MT /Gm- /W4 /O1 /Gs /DDETOURS_X86=1 /D_X86_


CFLAGS    = $(CFLAGS) -I$(DETOURS)/include
LIBS      = $(DETOURS)\lib\detours.lib 
TRACELIBS = kernel32.lib gdi32.lib user32.lib shell32.lib advapi32.lib ole32.lib ws2_32.lib


all: bin\wtrace.exe bin\traceapi.dll 

bin\wtrace.exe: src\wtrace.cpp
	cl $(CFLAGS) /Fe$@ $** /link /subsystem:console /incremental:no $(LIBS)

bin\traceapi.dll: src\traceapi.cpp src\_win32.cpp src\syelogFake.cpp
	cl $(CFLAGS) /Fe$@ src\traceapi.cpp src\syelogFake.cpp /LD /link /subsystem:console /incremental:no /entry:_DllMainCRTStartup@12 /export:NullExport $(LIBS) $(TRACELIBS)

clean:
	del bin\traceapi.* bin\wtrace.* 