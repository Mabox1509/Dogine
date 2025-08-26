# Compiladores
CXX := g++
CC := gcc

# Flags
CXXFLAGS := -Iinc -Wall -Wextra -O3 -fPIC
CFLAGS := -Iinc -Wall -Wextra -O3 -fPIC

# Nombre de salida (solo archivo)
OUTNAME := libdogine.a

# Carpetas
SRC_DIR := src
BUILD_DIR := out
BIN_DIR := bin

# Archivos fuente
CPP_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
C_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.c")
SOURCES := $(CPP_SOURCES) $(C_SOURCES)

# Archivos objeto (manteniendo estructura de subdirectorios)
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(CPP_SOURCES:.cpp=.o))
OBJECTS += $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(C_SOURCES:.c=.o))

# Reglas
.PHONY: all clean

all: $(BIN_DIR)/$(OUTNAME)

# Crear librería estática
$(BIN_DIR)/$(OUTNAME): $(OBJECTS) | $(BIN_DIR)
	ar rcs $@ $(OBJECTS)

# Compilar C++
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar C
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear carpeta bin
$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(OUTNAME)
