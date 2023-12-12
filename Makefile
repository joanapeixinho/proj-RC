CC = g++
CXX = g++
LD = g++

INCLUDE_DIRS := src/Client src/Server src/
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

TARGETS = src/Client/User src/Server/server
TARGET_EXECS = user AS

CLIENT_SOURCES := $(wildcard src/Client/*.cpp)
COMMON_SOURCES := $(wildcard src/common/*.cpp)
SERVER_SOURCES := $(wildcard src/Server/*.cpp)
SOURCES := $(CLIENT_SOURCES) $(COMMON_SOURCES) $(SERVER_SOURCES)

CLIENT_HEADERS := $(wildcard src/Client/*.hpp)
COMMON_HEADERS := $(wildcard src/common/*.hpp)
SERVER_HEADERS := $(wildcard src/Server/*.hpp)
HEADERS := $(CLIENT_HEADERS) $(COMMON_HEADERS) $(SERVER_HEADERS)

CLIENT_OBJECTS := $(CLIENT_SOURCES:.cpp=.o)
COMMON_OBJECTS := $(COMMON_SOURCES:.cpp=.o)
SERVER_OBJECTS := $(SERVER_SOURCES:.cpp=.o)
OBJECTS := $(CLIENT_OBJECTS) $(COMMON_OBJECTS) $(SERVER_OBJECTS)

CXXFLAGS = -std=c++17
LDFLAGS = -std=c++17

CXXFLAGS += $(INCLUDES)
LDFLAGS += $(INCLUDES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

vpath # clears VPATH
vpath %.hpp $(INCLUDE_DIRS)

# Run `make OPTIM=no` to disable -O3
ifeq ($(strip $(OPTIM)), no)
	CXXFLAGS += -O0
else
	CXXFLAGS += -O3
endif

# Run `make DEBUG=true` to run with debug symbols
ifeq ($(strip $(DEBUG)), yes)
	CXXFLAGS += -g
endif

CXXFLAGS += -fdiagnostics-color=always
CXXFLAGS += -Wall
CXXFLAGS += -Werror
CXXFLAGS += -Wextra
CXXFLAGS += -Wcast-align
CXXFLAGS += -Wconversion
CXXFLAGS += -Wfloat-equal
CXXFLAGS += -Wformat=2
CXXFLAGS += -Wnull-dereference
CXXFLAGS += -Wshadow
CXXFLAGS += -Wsign-conversion
CXXFLAGS += -Wswitch-default
CXXFLAGS += -Wswitch-enum
CXXFLAGS += -Wundef
CXXFLAGS += -Wunreachable-code
CXXFLAGS += -Wunused
LDFLAGS += -pthread

.PHONY: all clean fmt fmt-check package

all: $(TARGET_EXECS)

fmt: $(SOURCES) $(HEADERS)
	clang-format -i $^

fmt-check: $(SOURCES) $(HEADERS)
	clang-format -n --Werror $^

src/Server/server: $(SERVER_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

src/Client/User: $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

AS: src/Server/server
	cp $< $@

user: src/Client/User
	cp $< $@

clean:
	rm -f $(OBJECTS) $(TARGETS) $(TARGET_EXECS) project.zip

package:
	cp readme.txt
	zip project.zip $(SOURCES) $(HEADERS) Makefile .clang-format readme.txt *.xlsx
