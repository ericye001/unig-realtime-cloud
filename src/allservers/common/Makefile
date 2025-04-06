TARGET=lib_common.a
#TARGET_SO=libcrossfire_common.so
CC=g++ -c -Wall -o3 -I../include
AR=ar -rsv
#LD=ld -shared
SRC=EpollUtils.cpp EpollWaiter.cpp IniParser.cpp Socket.cpp SocketStream.cpp Thread.cpp Utils.cpp Buffer.cpp IOStream.cpp
OBJ=$(SRC:.cpp=.o)

all: $(OBJ) $(TARGET)

%.o : %.cpp
	$(CC) $< -o $@

$(TARGET): $(OBJ)
	$(AR) $(TARGET) $(OBJ)

#$(TARGET_SO):
#	$(LD) -soname $(TARGET_SO) -o $(TARGET_SO) $(OBJ)

install:
	@cp -f $(TARGET) /usr/lib/
#	@cp -f $(TARGET_SO) /usr/lib/

clean:
	@rm -rf *.o $(TARGET) $(TARGET_SO)
