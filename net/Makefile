TOP_DIR=..
INC_DIR=-I../..
ADD_SRC=
TARGET_ROOT_PATH=../build
ifeq ($(TARGET_PATH),)
else
    TARGET_ROOT_PATH=$(TARGET_PATH)
endif
TARGET_LIB_PATH = $(TARGET_ROOT_PATH)/lib
TARGET_INC_PATH = $(TARGET_ROOT_PATH)/simcode/net

CPP=g++



CPPFLAGS= -std=c++11 $(INC_DIR)
LDFLAGS += -static
#LIBS=-lpthread -lboost_system -lboost_thread

ifeq (y,$(DEBUG))
     CPPFLAGS += -g -O0
else
     CPPFLAGS += -g -O3
endif

export CFLAGS CPPFLAGS LDFLAGS

SOURCES=$(shell echo *.cpp $(ADD_SRC))
HEADERS=$(shell echo *.h)
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=libsimcode_net.a
all: $(TARGET)
$(TARGET) : $(OBJECTS)
	$(AR) -rc $(TARGET) $(OBJECTS)
	echo "target_path=$(TARGET_ROOT_PATH)"
	mkdir -p $(TARGET_LIB_PATH)
	cp $(TARGET) $(TARGET_LIB_PATH)
	mkdir -p $(TARGET_INC_PATH)
	cp *.h $(TARGET_INC_PATH)

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)
