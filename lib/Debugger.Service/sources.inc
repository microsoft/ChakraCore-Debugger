LIBLETNAME = ChakraCore.Debugger.Service
LIBLET_NO_PRECOMP = 1
#PRECOMPILED_INCLUDE = ..\stdafx.h

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Protocol; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.ProtocolHandler; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Service; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\deps\websocketpp; \
	$(PKGBOOST)\lib\native\include; \
	$(WAREHOUSEIMPORTDIR)\microsoft_chakracore\x-none\inc; \

!LISTFILES -recursive BOOST_HEADERS = $(PKGBOOST) *

HEADERS = $(HEADERS) \
	$(BOOST_HEADERS) \

SOURCES_SHARED = \
	..\stdafx.cpp \
	..\ChakraDebugService.cpp \
	..\Service.cpp \
	..\ServiceHandler.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

C_DEFINES = $(C_DEFINES) -D _HAS_AUTO_PTR_ETC -D BOOST_ASIO_DISABLE_SIGNAL

C_FLAGS = $(C_FLAGS) /W3 /wd4127

