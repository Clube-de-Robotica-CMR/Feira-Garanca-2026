#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath> // Adicionado para garantir o uso correto de sqrtf e operações matemáticas
#include "include/raylib.h"
#include "include/json.hpp"

using json = nlohmann::json;

// ==============================================================================
// ESTRUTURAS DE DADOS
// ==============================================================================
struct Posicao2D {
    float x;
    float y;
};

struct Dimensoes2D {
    float largura;
    float comprimento;
};

struct ObjetoArena {
    int id;
    Posicao2D posicao;
    Dimensoes2D dimensoes;
};

// Variáveis Globais de Controle de Dados
Posicao2D pos_robo = { 0.0f, 0.0f };
std::vector<ObjetoArena> lista_objetos;
Camera3D camera = {}; // Inicialização limpa compatível com o GCC do Fedora

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
    // 1. Configura a janela para aceitar redimensionamento antes de inicializá-la
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    // 2. Inicializa uma janela temporária para permitir que a Raylib converse com o driver de vídeo do Fedora
    InitWindow(800, 600, "Mapeamento da Arena");

    // 3. Detecta a resolução real do monitor atual onde a janela foi aberta
    int monitor = GetCurrentMonitor();
    int largura_tela = GetMonitorWidth(monitor);
    int altura_tela = GetMonitorHeight(monitor);

    // 4. Redimensiona a janela para ocupar a tela inteira detectada
    SetWindowSize(largura_tela, altura_tela);
    
    // Opcional: Se quiser que entre em TELA CHEIA REAL (estilo jogo, sumindo com as barras do sistema), 
    // desinale as duas linhas acima e descomente a linha abaixo:
    // ToggleFullscreen();

    SetTargetFPS(60);

    // 5. Afasta a câmera para termos uma visão panorâmica inicial da quina (0,0)
    camera.position = (Vector3){ -20.0f, 45.0f, -20.0f }; 
    camera.target = (Vector3){ lado_da_arena / 2.0f, 0.0f, lado_da_arena / 2.0f }; // Mira no centro
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };                                    
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void DesenharChaoDaArena(float tamanho, float espacamento) {
    // Desenha as linhas paralelas ao eixo Z (comprimento)
    for (float x = 0; x <= tamanho; x += espacamento) {
        DrawLine3D((Vector3){ x, 0.0f, 0.0f }, (Vector3){ x, 0.0f, tamanho }, LIGHTGRAY);
    }
    // Desenha as linhas paralelas ao eixo X (largura)
    for (float z = 0; z <= tamanho; z += espacamento) {
        DrawLine3D((Vector3){ 0.0f, 0.0f, z }, (Vector3){ tamanho, 0.0f, z }, LIGHTGRAY);
    }
}

void DesenharRobo(Posicao2D pos) {
    // Mapeia o Y do plano 2D para o eixo Z do ambiente tridimensional da Raylib
    Vector3 posicao_3d = { pos.x, tamanho_robo / 2.0f, pos.y };
    
    DrawCube(posicao_3d, tamanho_robo, tamanho_robo, tamanho_robo, RED);
    DrawCubeWires(posicao_3d, tamanho_robo, tamanho_robo, tamanho_robo, MAROON); 
}

void DesenharObjeto(const ObjetoArena& obj) {
    Vector3 posicao_3d = { obj.posicao.x, altura_objeto / 2.0f, obj.posicao.y };
    
    DrawCube(posicao_3d, obj.dimensoes.largura, altura_objeto, obj.dimensoes.comprimento, BLUE);
    DrawCubeWires(posicao_3d, obj.dimensoes.largura, altura_objeto, obj.dimensoes.comprimento, DARKBLUE);
}

void Atualizar(const std::string& caminho_json) {
    std::ifstream arquivo(caminho_json);
    if (!arquivo.is_open()) {
        json_encontrado = false; // Arquivo sumiu ou está no lugar errado
        return;
    }
    json_encontrado = true;

    try {
        json j;
        arquivo >> j;
        arquivo.close();

        if (!j.empty()) {
            pos_robo.x = j["posicao_robo"]["x"];
            pos_robo.y = j["posicao_robo"]["y"];

            lista_objetos.clear();
            for (const auto& item : j["objetos"]) {
                ObjetoArena obj;
                obj.id = item["id"];
                obj.posicao.x = item["posicao"]["x"];
                obj.posicao.y = item["posicao"]["y"];
                obj.dimensoes.largura = item["dimensoes"]["largura"];
                obj.dimensoes.comprimento = item["dimensoes"]["comprimento"];
                
                lista_objetos.push_back(obj);
            }
        }
    } catch (const json::parse_error& e) {
        // Erro se o arquivo estiver com sintaxe corrompida (ex: falta de vírgula)
        std::cout << "[JSON ERRO DE SINTAXE]: " << e.what() << std::endl;
        if (arquivo.is_open()) arquivo.close();
    } catch (const json::type_error& e) {
        // Erro se o tipo no JSON não bater com a variável do C++
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

        // Atualiza a rotação olhando com o mouse (segurando o botão direito)
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            // Se o botão direito estiver pressionado, a Raylib atualiza a rotação livremente
            UpdateCamera(&camera, CAMERA_FREE);
        } else {
            // Se NÃO estiver pressionado, passamos um modo inexistente ou mantemos estático.
            // No entanto, para a Raylib não acumular o movimento do mouse em background,
            // chamamos o UpdateCamera com uma "falsa câmera" ou simplesmente não atualizamos.
            // O jeito mais limpo na Raylib para travar a rotação mantendo o foco é:
            
            // Apenas atualiza a posição interna sem aplicar o delta do mouse do modo livre
            // (Isso evita que a câmera dê um "pulo" quando você clicar o botão direito novamente)
            std::vector<ObjetoArena> lista_velha = lista_objetos; // dummy para manter fluxo
        }

        // CONTROLE MANUAL COM W, A, S, D
        Vector3 direcao = {
            camera.target.x - camera.position.x,
            0.0f, 
            camera.target.z - camera.position.z
        };
        
        float comprimento = sqrtf(direcao.x * direcao.x + direcao.z * direcao.z);
        
        // PROTEÇÃO: Evita divisão por zero se a câmera e o alvo colidirem
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

        // RENDERIZAÇÃO FÍSICA
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                
                // 1. DESENHA A GRADE DA ARENA NO CHÃO
                DesenharChaoDaArena(lado_da_arena, 5.0f);

                // 2. BORDAS DE ORIENTAÇÃO (Eixos cartesianos partindo da quina 0,0)
                // Linha Vermelha indica a Largura (Eixo X)
                DrawLine3D((Vector3){ 0.0f, 0.1f, 0.0f }, (Vector3){ lado_da_arena, 0.1f, 0.0f }, RED);
                // Linha Azul indica o Comprimento (Eixo Z)
                DrawLine3D((Vector3){ 0.0f, 0.1f, 0.0f }, (Vector3){ 0.0f, 0.1f, lado_da_arena }, BLUE);

                // 3. DESENHA O ROBÔ (Bloco Vermelho)
                DesenharRobo(pos_robo);

                // 4. DESENHA OS OBSTÁCULOS DETECTADOS (Blocos Azuis)
                for (const auto& obj : lista_objetos) {
                    DesenharObjeto(obj);
                }

            EndMode3D();

            // Interface do Usuário em 2D
            DrawText("Controles: WASD para andar | Botao direito do mouse + arrastar para olhar", 10, 10, 20, DARKGRAY);
            DrawFPS(10, 40);
            if (!json_encontrado) {
                DrawText("ERRO: Arquivo 'arena.json' nao encontrado!", 10, 70, 20, RED);
                DrawText("Verifique se o arquivo está na mesma pasta onde voce abriu o terminal.", 10, 95, 16, MAROON);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}