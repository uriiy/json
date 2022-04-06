TARGET = hello

BUILD_DIR = build

CC=g++
CFLAGS=-c -g3
LDFLAGS += -static
SOURCES += \
  src/main.c \
  src/hello.c \
  lwjson/src/lwjson/lwjson.c \
  lwjson/src/lwjson/lwjson_debug.c \

INC += \
  -Iinc \
  -Ilwjson/src/include \

DEFS += \
	-DLWJSON_IGNORE_USER_OPTS \
	-D_DEBUG \

CFLAGS += $(INC) ${DEFS}

OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(TARGET)
	
$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
.c.o:
	$(CC) $(CFLAGS) $< -o $@