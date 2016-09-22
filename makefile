BINARY=ccsh
OBJECTS=obj/ccsh_command.o obj/ccsh_utils.o obj/main.o
HEADERS=src/CWrapper.hpp src/ccsh.hpp

# compilation and linking
CXX=clang++
CXXFLAGS=-std=c++14
WRNFLAGS=-Wall -Wextra -Weffc++ -Werror -pedantic
OPTFLAGS=-O0 -g -ggdb
LDFLAGS=-g -ggdb -lboost_system

# for later use
CXXLIBS=

.PHONY: all clean

all: $(BINARY)
	@echo Compilation Successful

clean:
	rm -rf $(BINARY) obj

$(BINARY): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@ $(CXXLIBS)
	strip $(BINARY)

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) $(WRNFLAGS) $(OPTFLAGS) -c $< -o $@

