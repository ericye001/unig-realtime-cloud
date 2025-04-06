CC=g++ -o3 -fpermissive -I../include  -Wall -g
SRC = GlobalArgs.cpp LoginServerListener.cpp  ILMAction.cpp ProxyAction.cpp ProxyManager.cpp Mgr.cpp stacktrace.c attach.c

OBJ = $(SRC:.cpp=.o)
LIB = ../message/lib_message.a ../common/lib_common.a -lpthread
TARGET = ProxyManger



all: $(OBJ) $(TARGET)


%.o : %.cpp
	$(CC) -c $< -o $@

ProxyManger: $(OBJ) $(LIB)
	$(CC) $(OBJ) $(LIB)   -o $(TARGET)


clean:
	@rm -rf *.o $(TARGET)
