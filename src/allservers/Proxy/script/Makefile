CC = g++  -g -I../include -I../../../../baselib/include -Wall -fpermissive
SRC = ../src/Proxy.cpp ../src/ProxyObject.cpp ../src/ProxyObjectFactory.cpp 
OBJ = $(SRC:.cpp=.o)
LIB = ../../../../baselib/lib/release/libcm.a -lpthread
TARGET = proxy 
all: $(TARGET) $(OBJ)

%.o : %.cpp 
	$(CC) -c $< -o $@  

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(OBJ) $(LIB) -o $(TARGET)
	mv $(TARGET) ../bin
clean:
	rm -rf ../src/*.o  ../test/*.o  $(TARGET)
