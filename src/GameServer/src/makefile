EXEC := gmsv
EXEPATH := ../bin
LIBS := BaseLib
LIBPATHS := ../../../../baselib/lib/
INCS := ../include
INCS += ../../../../baselib/include/


CC := gcc
CXX := g++
CFLAGS := -g -Wall -fpermissive
CFLAGS += -march=pentium4 -O2 -pipe -fomit-frame-pointer
CHOST = i686-pc-linux-gnu 

CXXFLAGS := $(CFLAGS)


RM-F := rm -f


SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cpp,$(MISSING_DEPS)))
CPPFLAGS += -MD

.PHONY : everything deps objs clean veryclean rebuild

everything : $(EXEPATH)$(EXEC)

deps : $(DEPS)

objs : $(OBJS)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(addprefix -I,$(INCS)) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(addprefix -I,$(INCS)) -c $< -o $@



clean :
	@$(RM-F) *.o
	@$(RM-F) *.d
	@$(RM-F) *.*~

veryclean: clean
	@$(RM-F) $(exepath)$(exec)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(EXEPATH)$(EXEC) : $(OBJS)
	$(CXX) -o $(EXEPATH)$(EXEC) $(OBJS) $(addprefix -I,$(INCS)) $(addprefix -L,$(LIBPATHS)) $(addprefix -l,$(LIBS))
