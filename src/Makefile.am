AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++11 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear

bin_PROGRAMS = homegear-baselib-unittests
homegear_baselib_unittests_SOURCES = main.cpp BitReaderWriter.cpp HelperFunctions.cpp Http.cpp Json.cpp Math.cpp WebSocket.cpp
homegear_baselib_unittests_LDADD = -lhomegear-base -lpthread -lreadline -lgcrypt -lgnutls -lgpg-error

if BSDSYSTEM
else
homegear_baselib_unittests_LDADD += -ldl
endif
