LIBLETNAME = ChakraCore.Debugger.Protocol

LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Protocol; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ChakraCore-Debugger\lib\Debugger.Protocol\Generated; \

SOURCES_SHARED = \
	..\Common.cpp \
	..\String16.cpp \
	..\StringUtil.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

C_FLAGS = $(C_FLAGS) /wd4100 /wd4244

