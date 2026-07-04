import time
from modelador.recebidor import Recebidor # Ajuste o caminho do import se necessário

def main():
    # Caminho do JSON que o seu renderizador.cpp em Raylib está escutando
    CAMINHO_ARENA = "./modelador/arena.json"
    
    recebidor = Recebidor(caminho=CAMINHO_ARENA)
    
    # O loop roda enquanto o pybricksdev estiver enviando dados pelo pipe
    while recebidor.ouvir():
        recebidor.salvar_json()
        
        # Um delay opcional mínimo apenas para aliviar o processamento do processador
        time.sleep(0.01)

if __name__ == "__main__":
    main()
