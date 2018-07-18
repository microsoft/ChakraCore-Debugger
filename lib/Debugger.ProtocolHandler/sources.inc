LIBLETNAME = ChakraCore.Debugger.ProtocolHandler

LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Protocol; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Protocol\Generated; \
	$(WAREHOUSEIMPORTDIR)\microsoft_chakracore\x-none\inc; \


SOURCES_SHARED = \
	..\ChakraDebugProtocolHandler.cpp \
	..\ConsoleImpl.cpp \
	..\Debugger.cpp \
	..\DebuggerBreak.cpp \
	..\DebuggerBreakpoint.cpp \
	..\DebuggerCallFrame.cpp \
	..\DebuggerContext.cpp \
	..\DebuggerImpl.cpp \
	..\DebuggerLocalScope.cpp \
	..\DebuggerObject.cpp \
	..\DebuggerRegExp.cpp \
	..\DebuggerScript.cpp \
	..\ErrorHelpers.cpp \
	..\JsPersistent.cpp \
	..\PropertyHelpers.cpp \
	..\ProtocolHandler.cpp \
	..\ProtocolHelpers.cpp \
	..\RuntimeImpl.cpp \
	..\SchemaImpl.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

C_FLAGS = $(C_FLAGS) /wd4100

