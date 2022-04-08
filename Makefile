TARGET = Srevice

BUILD_DIR = build

CC=g++
RES=windres
CFLAGS=-c -g3
LDFLAGS += -static
SOURC += \
	src/main.c \
	src/hello.c \
	src/modbusCrc.c \
	lwjson/src/lwjson/lwjson.c \
	lwjson/src/lwjson/lwjson_debug.c \

RESURSES += \
	src/icon.rc \

INC += \
	-Iinc \
	-Ilwjson/src/include \

DEFS += \
	-DLWJSON_IGNORE_USER_OPTS \
	-D_DEBUG \

CFLAGS += $(INC) ${DEFS}

OBJECTS=$(addprefix $(BUILD_DIR)/, $(notdir $(SOURC:.c=.o)))
vpath %.c $(sort $(dir $(SOURC)))
ICON=$(addprefix $(BUILD_DIR)/, $(notdir $(RESURSES:.rc=.o)))
vpath %.rc $(sort $(dir $(SOURC)))

all: $(TARGET)

$(BUILD_DIR):
	mkdir "$(BUILD_DIR)"

$(TARGET): $(OBJECTS) $(ICON) Makefile
	$(CC) $(LDFLAGS) $(OBJECTS) $(ICON) -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o:%.rc Makefile | $(BUILD_DIR)
	$(RES) $< -o $@

clear:
	rm -fr "$(BUILD_DIR)"
	rm -rf "$(TARGET)"
	rm -rf "ini.json"