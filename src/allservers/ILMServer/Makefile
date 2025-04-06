CC=g++ -o3 -fpermissive -I../include -Wall -g

SRC= RoleServer.cpp stacktrace.cpp attach.cpp Util.cpp Config.cpp ZoneMgr.cpp Zone.cpp  PlanMgr.cpp  GameServerListener.cpp ProxyMgrServerListener.cpp GlobalArgs.cpp  ConsoleListener.cpp ServerManager.cpp ILMServer.cpp

OBJ = $(SRC:.cpp=.o)

TARGET = ILMServer
LIB = ../message/lib_message.a ../common/lib_common.a -lpthread

all: $(OBJ) $(TARGET)

%.o : %.cpp
	$(CC) -c $< -o $@

ILMServer: $(OBJ) $(LIB)
	$(CC) $(OBJ) $(LIB)   -o $(TARGET)
	

clean:
	@rm -rf *.o $(TARGET)
