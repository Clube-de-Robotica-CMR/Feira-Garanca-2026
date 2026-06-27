# Compilador e Flags
CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

INCLUDE_DIR = ./modelador/include

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = ./modelador/renderizador.cpp
TARGET = ./modelador/bin/renderizador

start:
	uv run python -m modelador.main

renderizador: modelador/renderizador.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) $(SRC) $(LIBS) -o $(TARGET)