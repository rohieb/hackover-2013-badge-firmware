#!/usr/bin/make -f

CPPFLAGS      = $$(pkg-config --cflags gtkmm-3.0) -I. -I ../badge
CFLAGS        = -Wall -Wextra -pedantic -std=c99   -Werror -O0 -g
CXXFLAGS      = -Wall -Wextra -pedantic -std=c++11 -Werror -O0 -g

BADGE         = badge
CONVERTER     = level-converter

GLADEFILES    = lcd-display.glade

VPATH         = ../badge

BADGE_CXXSRCS = mock-main.cc \
                mock/badge_worker.cc \
                mock/display.cc \
                mock/event.cc \
                mock/lcd-window.cc
BADGE_CSRCS   = badge_main_loop.c \
                ui/font.c \
                ui/sprite.c \
                jumpnrun/collision.c \
                jumpnrun/enemies.c \
                jumpnrun/items.c \
                jumpnrun/jumpnrun.c \
                jumpnrun/tiles.c \
                jumpnrun/level_load.c

BADGE_CXXOBJS = $(BADGE_CXXSRCS:%.cc=%.o)
BADGE_COBJS   = $(BADGE_CSRCS:%.c=%.o)
BADGE_OBJS    = $(BADGE_CXXOBJS) $(BADGE_COBJS)

CONVERTER_SRCS = tools/level-converter.cc
CONVERTER_OBJS = $(CONVERTER_SRCS:%.cc=%.o)

CDEPS      = $(BADGE_CSRCS:%.c=%.dep)
CXXDEPS    = $(BADGE_CXXSRCS:%.cc=%.dep) $(CONVERTER_SRCS:%.cc=%.dep)
DEPS       = $(CDEPS) $(CXXDEPS)

LDLIBS     = $$(pkg-config --libs gtkmm-3.0)
RM         = rm -f

.PHONY: all dep clean distclean play

all: dep $(BADGE) $(CONVERTER)


$(BADGE_COBJS) : %.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -c -o $(@:../badge/%=%) $< $(LDLIBS)

$(BADGE_CXXOBJS) : %.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $< $(LDLIBS)

$(BADGE): $(BADGE_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(+:../badge/%=%) $(LDLIBS)

dep: $(DEPS)

$(CDEPS): %.dep : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< -MT $(<:%.c=%.o) -MF $@

$(CXXDEPS): %.dep : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< -MT $(<:%.cc=%.o) -MF $@

sinclude $(DEPS)

clean:
	$(RM) $(BADGE_OBJS) $(CONVERTER_OBJS) $(BADGE) $(CONVERTER)

distclean: clean
	$(RM) $(DEPS) $(wildcard *~)

$(CONVERTER): $(CONVERTER_OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

play: all
	./$(CONVERTER)
	./$(BADGE)
