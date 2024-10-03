# Variables
CXX = g++
CXXFLAGS = -I"C:\PROGRAMMING\BIG PROJECT\DrawPixel with C++\include"
LDFLAGS = -L"C:\PROGRAMMING\BIG PROJECT\DrawPixel with C++\lib" -lmingw32 -lSDL2main -lSDL2 -mconsole
TARGET = myapp
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	del *.o $(TARGET)
