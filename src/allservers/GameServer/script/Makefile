TARGET := libgs.a
TARGETPATH := ../lib/release/
#LIBS := cm 
#LIBPATHS := ../../../../baselib/lib/
INCS := ../include
INCS += ../../../../baselib/include/


CC := gcc
CXX := g++

CHOST = i686-pc-linux-gnu 
CFLAGS := -g -Wall -fpermissive -Dlinux -DHAVE_CONFIG_H -DNDEBUG -DZT_POSIX -DRLOG_COMPONENT="gmsv"
CFLAGS += -march=pentium4 -O2 -pipe -fomit-frame-pointer
CXXFLAGS := $(CFLAGS) 



AR=ar -rsv

#LD=ld -shared

SOURCE := $(wildcard ../src/*.c) $(wildcard ../src/*.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

.PHONY : all objs clean 

#OBJ1=$(SRC:.cxx=.o)
#OBJ2=$(SRC:.c=.o)

all : $(TARGETPATH)$(TARGET)

objs : $(OBJS)

%.o : %.cpp
	$(CC) $(CXXFLAGS) $(addprefix -I,$(INCS)) -c $< -o $@
%.o : %.c
	$(C)  $(CFLAGS) $(addprefix -I,$(INCS)) -c $< -o $@

#	#echo $(OBJ2)
$(TARGETPATH)$(TARGET): $(OBJS) 
	$(AR)  $(TARGETPATH)$(TARGET) $(OBJS) 
#mv $(TARGET) ../lib/release
#$(TARGET_SO):
#	$(LD) -soname $(TARGET_SO) -o $(TARGET_SO) $(OBJ)

#test:
#	echo $(SOURCE)
#	echo $(OBJS)
install:
#	@cp -f $(TARGET) /usr/lib/
#	@cp -f $(TARGET_SO) /usr/lib/

clean:
	@rm -rf ../src/*.o ../lib/release/$(TARGET)
