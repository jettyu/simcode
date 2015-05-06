TOP_DIR=../../../../build
INC_DIR=-I$(TOP_DIR)/

CPP=g++
FLAGS=-o 
CPPFLAGS= -g -O0 -std=c++11 $(INC_DIR)
LIBS=  -lpthread \
       -lboost_system -lboost_thread \
       -lprotoc -lprotobuf -lprotobuf-lite \
       -lhiredis \
       -L$(TOP_DIR)/lib -lsimcode_net -lsimcode_thread

SOURCES=$(shell echo *.cpp $(ADD_SRC))
HEADERS=$(shell echo *.h)
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=client

target:
	make all
all: $(TARGET)
$(TARGET) : $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(FLAGS) $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)
.PHONY: clean
clean:
	rm -rf *.o *.pb.* $(TARGET)
