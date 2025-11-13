TARGET := prog
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	g++ -o $(TARGET) $(OBJ) -lrtmidi

%.o: %.cpp
	g++ -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)