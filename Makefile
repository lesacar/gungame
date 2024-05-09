CC = gcc
CFLAGS = -Wall -Wextra -pthread
RM = rm
WARN = 
LDFLAGS = -L./static -lraylib -lm
INCLUDE = -I./src/include
BUILD_DIR = build
TARGET = $(BUILD_DIR)/physim
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = dbg
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(WARN) $(INCLUDE) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

check: $(TARGET)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDE) $(WARN) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	cppcheck -q -I../raylib/include/ --enable=warning $(SRC)

clean:
	$(RM) -rf $(BUILD_DIR) $(OBJ_DIR) windows_obj

win: CC = x86_64-w64-mingw32-gcc -std=c11
win: CFLAGS += -march=native
win: LDFLAGS = -L /usr/lib/gcc/x86_64-w64-mingw32/10-posix/ -L ./static/windows -lraylib -lm -lopengl32 -lgdi32 -lwinmm -static
win: INCLUDE = -I./static/windows/include/
win: TARGET = $(BUILD_DIR)/main.exe
win: OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
win: $(TARGET)

winrel: CFLAGS += -O3 -D WINREL
# winrel: clean
winrel: win

debug: CFLAGS += -g -D UDEBUG
debug: $(TARGET)

release: CFLAGS += -O3 -D URELEASE
release: clean
release: $(TARGET)
