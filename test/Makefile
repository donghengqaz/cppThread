ifneq ($(ESTD_SDK), freertos)

BD_DIR:=build_posix

.PHONY: all
all:
	@mkdir $(BD_DIR) -p
	@cd $(BD_DIR) && cmake .. && make

.PHONY: clean
clean:
	@rm -rf $(BD_DIR)

else
PROJECT_NAME := test

DEFS = -DCONFIG_USING_FREERTOS
CPPFLAGS += $(DEFS)

include $(IDF_PATH)/make/project.mk
endif
