# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++11

# Source files
SRCS = project.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = project

# Rule to compile everything
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Rule to compile source files
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean object files and executable
clean:
	rm -f $(OBJS) $(EXEC)
