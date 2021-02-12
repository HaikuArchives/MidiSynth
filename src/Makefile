#-------------------------------------------------------
#
#	Generic Cross-platform Makefile Base
#	R4 x86 and ppc binaries
#
#  after:
#		Be Inc. Generic R4 makefile
#
#	Original Copyright 1998 Be, Inc.  All Rights Reserved
#
#
#	Fill out the Project Specific Information section
#	Use a \ to continue information on different lines
#	example: SRCS = source1.cpp \
#					source2.cpp
#
#	When done simply cd to the directory in the terminal
#	and type make.  The objects and binary will appear
#	in an objx86 or objppc folder.
#
#--------------------------------------------------------

#--------------------------------------------------------
#	Project Specific Information
#--------------------------------------------------------

#	specify the name of the project -- FILL THIS IN!!
	NAME=MidiSynth
	
#	specify the type of image to create
#		APP:	Application
#		SHARED:	Shared Library
#		STATIC:	Static Library
	TYPE=APP
	
#	specify the list of source files
	## SRCS= 	

include Makefile.SRCS

#	specify the list of resource files
#	Only resources for the host platform are needed
	## RSRCS= 

include Makefile.RSRCS
	
#	specify additional libraries to link against
#	there are two acceptable forms of library specifications
#	-	if your library follows the naming pattern of:
#		libXXX.so or libXXX.a you can simply specify XXX
#		library: libbe.so entry: be
#	  (Note default entry!)
#		
#	- 	if your library does not follow the standard library
#		naming scheme you need to specify the path to the library
#		and it's name
#		library: my_lib.a entry: my_lib.a or path/my_lib.a
LIBS= midi midi2 GL be tracker
		
#	specify additional paths to directories following the standard
#	libXXX.so or libXXX.a naming scheme.  You can specify full paths
#	or paths relative to the makefile.  The paths included may not
#	be recursive, so include all of the paths where libraries can
#	be found.  Directories where source files are found are
#	automatically included.
LIBPATHS= 

#	additional paths to look for system headers
#	thes use the form: #include <header>
#	source file directories are NOT auto-included here
SYSTEM_INCLUDE_PATHS = 

#	additional paths to look for local headers
#	thes use the form: #include "header"
#	source file directories are automatically included
LOCAL_INCLUDE_PATHS = 

#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= 

#	specify any preprocessor symbols to be defined.  The symbols
#	will be set to a value of 1.  For example specify DEBUG if you want
#	DEBUG=1 to be set when compiling.
DEFINES= 

#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS = 

#	specify whether image symbols will be created
#	so that stack crawls in the debugger are meaningful
#	if TRUE symbols will be created
SYMBOLS = 

#	specify debug settings
#	if TRUE will allow application to be run from
#	a source-level debugger
DEBUGGER = 

#	specify additional compiler flags for all files
COMPILER_FLAGS =

#	specify additional linker flags
LINKER_FLAGS =


#####################################################################

## include the makefile-engine
include /boot/develop/etc/makefile-engine

cleanall::
	$(MAKE) CPU=ppc clean
	$(MAKE) CPU=x86 clean
	
#	remove just the application from the object folder
rmapp ::
	-rm -f $(TARGET)


