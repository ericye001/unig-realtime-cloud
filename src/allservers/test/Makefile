CC = gcc -o3  -Wall
SRC =do.c

OBJ = $(SRC:.cpp=.o)

LIB =  -lpthread

TARGET = test 
all: $(TARGET) $(OBJ)

%.o : %.cpp 
	$(CC) -c $< -o $@

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(OBJ) $(LIB) -o $(TARGET)


clean:
	rm -rf *.o *.core $(TARGET)
