cmake_minimum_required(VERSION 3.15)
project(Dogine LANGUAGES C CXX)

# Configuración general
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Rutas
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/inc)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/out)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# Busca todos los .cpp y .c en src/
file(GLOB_RECURSE CPP_SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE C_SOURCES ${SRC_DIR}/*.c)
set(SOURCES ${CPP_SOURCES} ${C_SOURCES})

# Librería estática
add_library(dogine STATIC ${SOURCES})
target_include_directories(dogine PUBLIC ${INC_DIR})

# Salida personalizada (igual que tu Makefile)
set_target_properties(dogine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)
