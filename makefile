CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20
TARGET = mycc
SRCS = main.cpp
# Automatically generate object file names (.o) from source file names
OBJS = $(SRCS:.cpp=.o)

ifdef OS
   # Settings for Windows
   RM = del /Q /F
   EXEC = $(TARGET).exe
else
   # Settings for Linux / Unix
   ifeq ($(shell uname), Linux)
      RM = rm -f
      EXEC = $(TARGET)
   endif
endif

all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Pattern rule to compile each .cpp file into a .o object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(EXEC) $(OBJS)