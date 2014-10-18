#!/usr/bin/make -f

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O0 -g
CPPFLAGS = -I .. -I ../..

LC_TARGET   = level-converter
LC_SRCS     = level-converter.cc
LC_OBJS     = $(LC_SRCS:%.cc=%.o)
LC_DEPS     = $(LC_OBJS:%.o=%.dep)

GLC_TARGET  = gladio_level_convert
GLC_SRCS    = gladio_level_convert.cc
GLC_OBJS    = $(GLC_SRCS:%.cc=%.o)
GLC_DEPS    = $(GLC_OBJS:%.o=%.dep)

MKTI_TARGET = mktitleimg
MKTI_SRCS   = mktitleimg.cc
MKTI_OBJS   = $(MKTI_SRCS:%.cc=%.o)
MKTI_DEPS   = $(MKTI_OBJS:%.o=%.dep)
MKTI_LIBS   = -lsfml-graphics

DEPS = $(LC_DEPS) $(MKTI_DEPS) $(GLC_DEPS)
OBJS = $(LC_OBJS) $(MKTI_OBJS) $(GLC_OBJS)

all: $(LC_TARGET) $(MKTI_TARGET) $(GLC_TARGET)

$(DEPS) : %.dep : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

sinclude $(DEPS)

$(OBJS): %.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(LC_TARGET): $(LC_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

$(MKTI_TARGET): $(MKTI_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(MKTI_LIBS) $(LDLIBS)

$(GLC_TARGET): $(GLC_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean:
	rm -f $(LC_TARGET) $(MKTI_TARGET) $(OBJS)

distclean: clean
	rm -f $(DEPS)

.PHONY: clean distclean
