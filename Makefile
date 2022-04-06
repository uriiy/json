TARGET = hello

BUILD_DIR = build

CC=g++
RES=windres
CFLAGS=-c -g3
LDFLAGS += -static
SOURC += \
	src/main.c \
	src/hello.c \
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

OBJECTS=$(SOURC:.c=.o)
ICON=$(RESURSES:.rc=.o)


all: $(SOURC) $(TARGET)

$(TARGET): $(OBJECTS) $(ICON)
	$(CC) $(LDFLAGS) $(OBJECTS) $(ICON) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o:%.rc
	$(RES) $< -o $@