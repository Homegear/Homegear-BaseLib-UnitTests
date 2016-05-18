AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++11 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear

bin_PROGRAMS = libhomegear-base-unittests
libhomegear_base_unittests_SOURCES = main.cpp
libhomegear_base_unittests_LDADD = -lhomegear-base -lpthread -lreadline -lgcrypt -lgnutls -lgpg-error

if BSDSYSTEM
else
libhomegear_base_unittests_LDADD += -ldl
endif
