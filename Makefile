CC = gcc
CXX = g++

EDCFLAGS:= -O2 -std=gnu11 -I include/ -Wall $(CFLAGS)
EDCXXFLAGS:= -O2 -std=c++11 -Wall -I include/ $(CXXFLAGS)
EDLDFLAGS:= -lpthread -lm $(LDFLAGS)

LIBTARGET := libsgp4.a

CSRCS := $(wildcard src/*.c)
COBJS := $(patsubst %.c,%.o,$(CSRCS))
CDEPS := $(patsubst %.c,%.d,$(CSRCS))

CCSRCS := $(wildcard src/*.cc)
CCOBJS := $(patsubst %.cc,%.o,$(CCSRCS))
CCDEPS := $(patsubst %.cc,%.d,$(CCSRCS))

CXXSRCS := $(wildcard src/*.cpp)
CXXOBJS := $(patsubst %.cpp,%.o,$(CXXSRCS))
CXXDEPS := $(patsubst %.cpp,%.d,$(CXXSRCS))

all: $(LIBTARGET) test

$(LIBTARGET): $(CXXOBJS)
	ar -crs $(LIBTARGET) $(CXXOBJS)

test: $(LIBTARGET)
	$(CXX) $(EDCXXFLAGS) examples/passpredict.cpp $(LIBTARGET) -o examples/passpredict.out $(EDLDFLAGS)
	$(CXX) $(EDCXXFLAGS) examples/runtest.cpp $(LIBTARGET) -o examples/runtest.out $(EDLDFLAGS)
	$(CXX) $(EDCXXFLAGS) examples/sattrack.cpp $(LIBTARGET) -o examples/sattrack.out $(EDLDFLAGS)

-include $(CDEPS)

%.o: %.c Makefile
	$(CC) $(EDCFLAGS) -MMD -MP -o $@ -c $<

-include $(CXXDEPS)

%.o: %.cpp Makefile
	$(CXX) $(EDCXXFLAGS) -MMD -MP -o $@ -c $<

-include $(CCDEPS)

%.o: %.cc Makefile
	$(CXX) $(EDCXXFLAGS) -MMD -MP -o $@ -c $<

.PHONY: clean

clean:
	rm -vf $(CCOBJS)
	rm -vf $(CXXOBJS)
	rm -vf $(COBJS)
	rm -vf examples/*.out

spotless: clean
	rm -vf $(CCDEPS)
	rm -vf $(CXXDEPS)
	rm -vf $(CDEPS)
	rm -vf $(LIBTARGET)