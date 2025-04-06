TARGET = lib_message.a

CC=g++ -c -o3 -Wall -I../include -fpermissive
AR=ar -rsv

SRC=Message.cpp PM2PROXY.cpp Login2PM.cpp Proxy2PM.cpp  Server2ILM.cpp PM2ILM.cpp  Console2ILM.cpp
OBJ=$(SRC:.cpp=.o)

all: $(OBJ) $(TARGET)

%.o : %.cpp
	$(CC) $< -o $@
$(TARGET): $(OBJ)
	$(AR) $(TARGET) $(OBJ)

clean:
	@rm -rf *.o $(TARGET)
