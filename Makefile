# Compilador e Flags
CXX = g++
CXXFLAGS = -O2 -std=c++17

INCLUDE_DIR = ./modelador/include

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = ./modelador/renderizador.cpp
TARGET = ./modelador/bin/renderizador

run:
	$(TARGET)

start:
	pybricksdev run ble --name "Chell" "/home/davizera/Dev/robótica/Feira-Garanca-2026/robo/main.py" | uv run python -m modelador.main

renderizador: modelador/renderizador.cpp
	$(CXX) $(CXXFLAGS) -isystem $(INCLUDE_DIR) $(SRC) $(LIBS) -o $(TARGET)