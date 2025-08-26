# Compiladores
CXX := g++
ASM := nasm

# Flags
CXXFLAGS := -Wall -Wextra -O3
ASMFLAGS := -f elf64
LDFLAGS := -lm -ldl -lpthread -lz -lsndfile

# Nombre de salida (sin ruta, sólo nombre de archivo)
OUTNAME := doginec

# Carpetas
SRC_DIR := src
BUILD_DIR := out
BIN_DIR := bin

# Archivos fuente
CPP_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
ASM_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.s")
SOURCES := $(CPP_SOURCES) $(ASM_SOURCES)

# Archivos objeto (con estructura de subdirectorios)
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(CPP_SOURCES:.cpp=.o))
OBJECTS += $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(ASM_SOURCES:.asm=.o))

# Reglas
.PHONY: all clean run

all: $(BIN_DIR)/$(OUTNAME)

$(BIN_DIR)/$(OUTNAME): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compilar C++
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar ASM
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

# Crear carpeta bin
$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(OUTNAME)

run: all
	LD_LIBRARY_PATH=$(BIN_DIR) ./$(BIN_DIR)/$(OUTNAME)
