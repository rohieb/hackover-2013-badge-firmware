#!/usr/bin/make -f

CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O0 -g

TARGET   = level-converter
SRCS     = level-converter.cc
OBJS     = $(SRCS:%.cc=%.o)
DEPS     = $(OBJS:%.o=%.dep)
CPPFLAGS = -I ..

all: $(TARGET)

$(DEPS) : %.dep : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< -MT $@ -MT $(@:%.dep=%.o) -MF $@

sinclude $(DEPS)

$(OBJS): %.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $+ $(LDLIBS)

clean:
	rm -f $(TARGET) $(OBJS)

distclean: clean
	rm -f $(DEPS)

.PHONY: clean distclean
