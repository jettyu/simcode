TOP_DIR=../../../../build
INC_DIR=-I$(TOP_DIR)/
VPATH=$(TOP_DIR)/simcode/base:$(TOP_DIR)/simcode/net:$(TOP_DIR)/simcode/thread
ADD_SRC=


CPP=g++
FLAGS=-o 
CPPFLAGS= -g -O0 -std=c++11 $(INC_DIR)
LIBS=  -lpthread \
       -L$(TOP_DIR)/lib -lsimcode_net -lsimcode_thread -lsimcode \

SOURCES=$(shell echo *.cpp $(ADD_SRC))
HEADERS=$(shell echo *.h)
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=server

target:
	make all
all: $(TARGET)
$(TARGET) : $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(FLAGS) $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)
.PHONY: clean
clean:
	rm -rf *.o *.pb.* $(TARGET)
