
CC := g++
SRC_DIR := main ../src
CPP_SRCS := $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.cpp) $(wildcard $(dir)/*.cc)) 
TARGET := test

INCLUDE += -I../include

DEFS = -DCONFIG_USING_POSIX
CPPFLAGS += --std=c++11 $(DEFS)

LIBS += -lpthread -lrt

$(TARGET):$(CPP_SRCS)
	@$(CC) $(CPPFLAGS) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

info:
	@echo $(OBJS)

.PHONY: clean
clean:
	@rm -rf $(TARGET) $(OBJS)
