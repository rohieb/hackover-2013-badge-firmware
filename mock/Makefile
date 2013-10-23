#!/usr/bin/make -f

CPPFLAGS      = $$(pkg-config --cflags gtkmm-3.0) -I. -I ../badge
CFLAGS        = -Wall -Wextra -pedantic -std=c99   -Werror -O0 -g -fstack-protector-all
CXXFLAGS      = -Wall -Wextra -pedantic -std=c++0x -Werror -O0 -g -fstack-protector-all
LDLIBS        = $$(pkg-config --libs gtkmm-3.0)

BADGE         = badge
CONVERTER     = level-converter
LEVELLIST     = levels.txt

GLADEFILES    = lcd-display.glade

BADGE_CXXSRCS = mock-main.cc \
                mock/badge_worker.cc \
                mock/display.cc \
                mock/event.cc \
                mock/lcd-window.cc
BADGE_CSRCS   = badge_main_loop.c

BADGE_FAROBJS = ui/font.o \
                ui/menu.o \
                ui/sprite.o \
                jumpnrun/collision.o \
                jumpnrun/enemies.o \
                jumpnrun/game_state.o \
                jumpnrun/items.o \
                jumpnrun/jumpnrun.o \
                jumpnrun/level_load.o \
                jumpnrun/player.o \
                jumpnrun/render.o \
                jumpnrun/shots.o \
                jumpnrun/stats.o \
                jumpnrun/tiles.o

BADGE_CXXOBJS = $(BADGE_CXXSRCS:%.cc=%.o)
BADGE_COBJS   = $(BADGE_CSRCS:%.c=%.o)
BADGE_OBJS    = $(BADGE_CXXOBJS) $(BADGE_COBJS) $(BADGE_FAROBJS)

CONVERTER_SRCS = tools/level-converter.cc
CONVERTER_OBJS = $(CONVERTER_SRCS:%.cc=%.o)

CDEPS      = $(BADGE_CSRCS:%.c=%.dep)
CXXDEPS    = $(BADGE_CXXSRCS:%.cc=%.dep) $(CONVERTER_SRCS:%.cc=%.dep)
FARDEPS    = $(BADGE_FAROBJS:%.o=%.dep)
DEPS       = $(CDEPS) $(CXXDEPS) $(FARDEPS)

RM         = rm -f

.PHONY: all dep clean distclean play

all: dep $(BADGE) $(CONVERTER)


$(BADGE_COBJS) : %.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BADGE_CXXOBJS) : %.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BADGE_FAROBJS) : %.o : ../badge/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BADGE): $(BADGE_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

dep: $(DEPS)

$(CDEPS): %.dep : %.c
	$(CC)  $(CPPFLAGS) $(CFLAGS)   -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

$(CXXDEPS): %.dep : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

$(FARDEPS): %.dep : ../badge/%.c
	$(CC)  $(CPPFLAGS) $(CFLAGS)   -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

sinclude $(DEPS)

clean:
	$(RM) $(BADGE_OBJS) $(CONVERTER_OBJS) $(BADGE) $(CONVERTER)

distclean: clean
	$(RM) $(DEPS) $(wildcard *~)

$(CONVERTER): $(CONVERTER_OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

play: all
	echo > $(LEVELLIST)
	ls ../badge/jumpnrun/levels/*.lv | xargs -n1 basename | cut -d . -f 1 > $(LEVELLIST)
	./$(CONVERTER)
	./$(BADGE)
