# Microsoft Developer Studio Generated NMAKE File, Based on encode.dsp
!IF "$(CFG)" == ""
CFG=encode - Win32 Debug
!MESSAGE No configuration specified. Defaulting to encode - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "encode - Win32 Release" && "$(CFG)" != "encode - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "encode.mak" CFG="encode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "encode - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "encode - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "encode - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\encode.exe"


CLEAN :
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\encode.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\encode.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\encode.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib BugslayerUtil.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\encode.pdb" /machine:I386 /out:"$(OUTDIR)\encode.exe" 
LINK32_OBJS= \
	"$(INTDIR)\encode.obj"

"$(OUTDIR)\encode.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "encode - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\encode.exe" "$(OUTDIR)\encode.bsc"


CLEAN :
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\encode.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\encode.bsc"
	-@erase "$(OUTDIR)\encode.exe"
	-@erase "$(OUTDIR)\encode.ilk"
	-@erase "$(OUTDIR)\encode.map"
	-@erase "$(OUTDIR)\encode.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\encode.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\encode.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\encode.sbr"

"$(OUTDIR)\encode.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib BugslayerUtil.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\encode.pdb" /map:"$(INTDIR)\encode.map" /debug /machine:I386 /out:"$(OUTDIR)\encode.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\encode.obj"

"$(OUTDIR)\encode.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("encode.dep")
!INCLUDE "encode.dep"
!ELSE 
!MESSAGE Warning: cannot find "encode.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "encode - Win32 Release" || "$(CFG)" == "encode - Win32 Debug"
SOURCE=.\encode.cpp

!IF  "$(CFG)" == "encode - Win32 Release"


"$(INTDIR)\encode.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "encode - Win32 Debug"


"$(INTDIR)\encode.obj"	"$(INTDIR)\encode.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

