#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath> 
#include "include/raylib.h"
#include "include/raymath.h" // Adicionado para operações de matrizes (MatrixRotateY, MatrixMultiply)
#include "include/json.hpp"

using json = nlohmann::json;

// ==============================================================================
// ESTRUTURAS DE DADOS
// ==============================================================================
struct Posicao2D {
    float x;
    float y;
};

struct ObjetoArena {
    int id;
    Posicao2D posicao;
    float dimensao; 
};

// Variáveis Globais de Controle de Dados
Posicao2D pos_robo = { 0.0f, 0.0f };
float angulo_robo = 0.0f; 
std::vector<ObjetoArena> lista_objetos;
Camera3D camera = {}; 

// Recursos reutilizáveis para o Robô (Evita recriar a malha todo frame)
Mesh malha_robo;
Model modelo_robo;

float lado_da_arena = 90.0f;
std::string arquivo_dados = "./modelador/arena.json";
float tamanho_robo = 20.0f; 
float altura_objeto = 10.0f;
float velocidade_camera = 0.8f;

bool json_encontrado = false;

// ==============================================================================
// FUNÇÕES DO RENDERIZADOR
// ==============================================================================

void ConfigurarCenario(float lado_da_arena) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Mapeamento da Arena");

    int monitor = GetCurrentMonitor();
    int largura_tela = GetMonitorWidth(monitor);
    int altura_tela = GetMonitorHeight(monitor);

    SetWindowSize(largura_tela, altura_tela);
    SetTargetFPS(60);

    camera.position = (Vector3){ -20.0f, 45.0f, -20.0f }; 
    camera.target = (Vector3){ lado_da_arena / 2.0f, 0.0f, lado_da_arena / 2.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };                                    
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Inicializa a malha e o modelo do robô nas dimensões corretas (20x20x20)
    malha_robo = GenMeshCube(tamanho_robo, tamanho_robo, tamanho_robo);
    modelo_robo = LoadModelFromMesh(malha_robo);
    // Define a cor base do material do robô para Vermelho
    modelo_robo.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = RED;
}

void DesenharChaoDaArena(float tamanho, float espacamento) {
    for (float x = 0; x <= tamanho; x += espacamento) {
        DrawLine3D((Vector3){ x, 0.0f, 0.0f }, (Vector3){ x, 0.0f, tamanho }, LIGHTGRAY);
    }
    for (float z = 0; z <= tamanho; z += espacamento) {
        DrawLine3D((Vector3){ 0.0f, 0.0f, z }, (Vector3){ tamanho, 0.0f, z }, LIGHTGRAY);
    }
}

void DesenharRobo(Posicao2D pos, float angulo) {
    // 1. Cria a matriz de translação para posicionar o robô na arena
    Matrix transformacao = MatrixTranslate(pos.x, tamanho_robo / 2.0f, pos.y);
    
    // 2. Cria a matriz de rotação no eixo Y (convertendo o ângulo para radianos)
    Matrix rotacao = MatrixRotateY(angulo * DEG2RAD);
    
    // 3. Combina as transformações (A rotação deve ser aplicada ANTES da translação)
    modelo_robo.transform = MatrixMultiply(rotacao, transformacao);
    
    // 4. Desenha o modelo preenchido
    DrawModel(modelo_robo, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
    
    // 5. Como a Raylib não faz o "Wires" de um modelo rotacionado facilmente, 
    // aplicamos a mesma transformação diretamente na malha para desenhar as linhas de contorno (maroon)
    DrawMesh(malha_robo, modelo_robo.materials[0], modelo_robo.transform);
}

void DesenharObjeto(const ObjetoArena& obj) {
    Vector3 posicao_3d = { obj.posicao.x, altura_objeto / 2.0f, obj.posicao.y };
    
    DrawCube(posicao_3d, obj.dimensao, altura_objeto, obj.dimensao, BLUE);
    DrawCubeWires(posicao_3d, obj.dimensao, altura_objeto, obj.dimensao, DARKBLUE);
}

void Atualizar(const std::string& caminho_json) {
    std::ifstream arquivo(caminho_json);
    if (!arquivo.is_open()) {
        json_encontrado = false; 
        return;
    }
    json_encontrado = true;

    try {
        json j;
        arquivo >> j;
        arquivo.close();

        if (!j.empty()) {
            pos_robo.x = j["robo"]["posicao"]["x"];
            pos_robo.y = j["robo"]["posicao"]["y"];
            
            if (j["robo"].contains("angulacao")) {
                angulo_robo = j["robo"]["angulacao"];
            } else {
                angulo_robo = 0.0f;
            }

            lista_objetos.clear();
            for (const auto& item : j["objetos"]) {
                ObjetoArena obj;
                obj.id = item["id"];
                obj.posicao.x = item["posicao"]["x"];
                obj.posicao.y = item["posicao"]["y"];
                obj.dimensao = item["dimensao"];
                
                lista_objetos.push_back(obj);
            }
        }
    } catch (const json::parse_error& e) {
        std::cout << "[JSON ERRO DE SINTAXE]: " << e.what() << std::endl;
        if (arquivo.is_open()) arquivo.close();
    } catch (const json::type_error& e) {
        std::cout << "[JSON ERRO DE TIPO]: " << e.what() << std::endl;
        if (arquivo.is_open()) arquivo.close();
    } catch (...) {
        std::cout << "[JSON ERRO DESCONHECIDO]" << std::endl;
        if (arquivo.is_open()) arquivo.close();
    }
}

// ==============================================================================
// LOOP PRINCIPAL (MAIN DO C++)
// ==============================================================================
int main() {
    ConfigurarCenario(lado_da_arena);

    while (!WindowShouldClose()) {
        
        Atualizar(arquivo_dados);

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        Vector3 direcao = {
            camera.target.x - camera.position.x,
            0.0f, 
            camera.target.z - camera.position.z
        };
        
        float comprimento = sqrtf(direcao.x * direcao.x + direcao.z * direcao.z);
        
        if (comprimento < 0.001f) {
            direcao.x = 0.0f;
            direcao.z = 1.0f;
        } else {
            direcao.x /= comprimento;
            direcao.z /= comprimento;
        }

        Vector3 direita = { -direcao.z, 0.0f, direcao.x };

        if (IsKeyDown(KEY_W)) {
            camera.position.x += direcao.x * velocidade_camera;
            camera.position.z += direcao.z * velocidade_camera;
            camera.target.x += direcao.x * velocidade_camera;
            camera.target.z += direcao.z * velocidade_camera;
        }
        if (IsKeyDown(KEY_S)) {
            camera.position.x -= direcao.x * velocidade_camera;
            camera.position.z -= direcao.z * velocidade_camera;
            camera.target.x -= direcao.x * velocidade_camera;
            camera.target.z -= direcao.z * velocidade_camera;
        }
        if (IsKeyDown(KEY_A)) {
            camera.position.x -= direita.x * velocidade_camera;
            camera.position.z -= direita.z * velocidade_camera;
            camera.target.x -= direita.x * velocidade_camera;
            camera.target.z -= direita.z * velocidade_camera;
        }
        if (IsKeyDown(KEY_D)) {
            camera.position.x += direita.x * velocidade_camera;
            camera.position.z += direita.z * velocidade_camera;
            camera.target.x += direita.x * velocidade_camera;
            camera.target.z += direita.z * velocidade_camera;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                
                DesenharChaoDaArena(lado_da_arena, 5.0f);

                DrawLine3D((Vector3){ 0.0f, 0.1f, 0.0f }, (Vector3){ lado_da_arena, 0.1f, 0.0f }, RED);
                DrawLine3D((Vector3){ 0.0f, 0.1f, 0.0f }, (Vector3){ 0.0f, 0.1f, lado_da_arena }, BLUE);

                DesenharRobo(pos_robo, angulo_robo);

                for (const auto& obj : lista_objetos) {
                    DesenharObjeto(obj);
                }

            EndMode3D();

            DrawText("Controles: WASD para andar | Botao direito do mouse + arrastar para olhar", 10, 10, 20, DARKGRAY);
            DrawFPS(10, 40);
            if (!json_encontrado) {
                DrawText("ERRO: Arquivo 'arena.json' nao encontrado!", 10, 70, 20, RED);
            }

        EndDrawing();
    }

    // Desaloca os recursos da GPU antes de fechar a janela
    UnloadModel(modelo_robo);
    CloseWindow();
    return 0;
}