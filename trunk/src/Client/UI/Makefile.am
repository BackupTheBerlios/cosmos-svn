INCLUDES = -I/usr/local/include/OGRE -I$(top_srcdir)/include $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = cosmosclientui

cosmosclientui_SOURCES = main.cpp CosmosGame.cpp CosmosGame_FrameListener.cpp
AM_CFLAGS = -Wall -fexceptions
AM_CXXFLAGS = $(CFLAGS)
noinst_HEADERS = CosmosGame_FrameListener.h CosmosGame.h
cosmosclientui_LDADD = -lOgreMain
