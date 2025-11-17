TARGET := prog
SRC := $(wildcard *.cpp) # all cpp files
OBJDIR := obj
OBJ := $(SRC:%.cpp=$(OBJDIR)/%.o) # obj is all .o files, their names will be the source files but replace .cpp with .o


all: $(TARGET)

$(TARGET): $(OBJ)
	g++ -o $(TARGET) $(OBJ) -lrtmidi

$(OBJDIR)/%.o: %.cpp # % matches any file name, %< is the dependency, $@ is the target
	g++ -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)