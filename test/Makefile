ifneq ($(ESTD_SDK), freertos)
CC := g++
SRC_DIR := main
SRCS := $(SRC_DIR)/thread_test.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := test

INCLUDE += -I../include

DEFS = -DCONFIG_USING_POSIX
CPPFLAGS += --std=c++11 $(DEFS)

LIBS += -lpthread -lrt

$(TARGET):$(OBJS)
	@$(CC) $(CPPFLAGS) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	@rm -rf $(TARGET) $(OBJS)

%.o:%.cpp
	@$(CC) $(CPPFLAGS) $(INCLUDE) -o $@ -c $< $(LIBS)
else
PROJECT_NAME := test

DEFS = -DCONFIG_USING_FREERTOS
CPPFLAGS += $(DEFS)

include $(IDF_PATH)/make/project.mk
endif
