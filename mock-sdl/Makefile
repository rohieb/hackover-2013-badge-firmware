#!/usr/bin/make -f

CC            = emcc
CPPFLAGS      = -I. -I ../badge
CFLAGS        = -Wall -Wextra -pedantic -std=c99 -Werror -O0 -g
LDLIBS        = -SDL -SDLmain

BADGE         = badge.html
LEVELLIST     = levels.txt

GLADEFILES    = lcd-display.glade

BADGE_CSRCS   = main_loop.c \
                mock/display.c \
                mock/event.c

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

BADGE_COBJS   = $(BADGE_CSRCS:%.c=%.o)
BADGE_OBJS    = $(BADGE_COBJS) $(BADGE_FAROBJS)

CDEPS      = $(BADGE_CSRCS:%.c=%.dep)
FARDEPS    = $(BADGE_FAROBJS:%.o=%.dep)
DEPS       = $(CDEPS) $(FARDEPS)

RM         = rm -f

.PHONY: all dep clean distclean play

all: dep $(BADGE)


$(BADGE_COBJS) : %.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BADGE_FAROBJS) : %.o : ../badge/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BADGE): $(BADGE_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

dep: $(DEPS)

$(CDEPS): %.dep : %.c
	$(CC)  $(CPPFLAGS) $(CFLAGS)   -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

$(FARDEPS): %.dep : ../badge/%.c
	$(CC)  $(CPPFLAGS) $(CFLAGS)   -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

sinclude $(DEPS)

clean:
	$(RM) $(BADGE_OBJS) $(BADGE)

distclean: clean
	$(RM) $(DEPS) $(wildcard *~)

CONVERTER = ../badge/tools/level-converter
LEVELS    = ../badge/jumpnrun/levels/*.lv

play: all
	$(MAKE) -C ../badge/tools
	echo > $(LEVELLIST)
	ls $(LEVELS) | xargs -n1 basename | cut -d . -f 1 > $(LEVELLIST)
	$(CONVERTER) $(LEVELS)
	./$(BADGE)
