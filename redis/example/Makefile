TOP_DIR=../../..
SIMCODE_DIR=$(TOP_DIR)/simcode/build
INC_DIR=-I$(SIMCODE_DIR) 

CPP=g++
FLAGS=-o 
CPPFLAGS= -DDEBUGSTR -g -O0 -std=c++11 $(INC_DIR)
LIBS=  -L$(NETSVR_DIR)/lib \
       -L$(SIMCODE_DIR)/lib -lsimcode_net -lsimcode_thread -lsimcode \
       -lsimcode_redis -lhiredis \
       -lpthread #-levent

SOURCES=$(shell echo *.cpp $(ADD_SRC))
HEADERS=$(shell echo *.h)
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=entry

TARGET_ROOT_PATH=build
ifeq ($(TARGET_PATH),)
else
    TARGET_ROOT_PATH=$(TARGET_PATH)
endif
TARGET_BIN_PATH = $(TARGET_ROOT_PATH)/bin/$(TARGET)

target:
	make all
all: $(TARGET)
$(TARGET) : $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(FLAGS) $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)
.PHONY: clean
clean:
	rm -rf *.o *.pb.* $(TARGET)
