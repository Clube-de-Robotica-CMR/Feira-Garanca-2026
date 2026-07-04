import time
import subprocess
import sys
import os
from modelador.recebidor import Recebidor

def main():
    CAMINHO_ARENA = "./modelador/arena.json"
    CAMINHO_EXECUTAVEL = "./modelador/bin/renderizador"
    
    if not os.path.exists(CAMINHO_EXECUTAVEL):
        print(f"[PC ERRO] Executável '{CAMINHO_EXECUTAVEL}' não encontrado!", file=sys.stderr)
        sys.exit(1)

    print("[PC] Inicializando o renderizador gráfico...", file=sys.stderr)
    processo_renderizador = subprocess.Popen(
        [CAMINHO_EXECUTAVEL],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL
    )
    
    recebidor = Recebidor(caminho=CAMINHO_ARENA)
    
    try:
        # O loop roda enquanto o pipe estiver aberto recebendo dados
        while recebidor.ouvir():
            # Só salva se a string atual for o JSON válido filtrado pelo ouvir
            if recebidor.texto_limpo.startswith('['):
                recebidor.salvar_json()
            time.sleep(0.01)
            
    except KeyboardInterrupt:
        print("\n[PC] Interrupção detectada.", file=sys.stderr)
    finally:
        if processo_renderizador.poll() is None:
            print("[PC] Encerrando o renderizador gráfico...", file=sys.stderr)
            processo_renderizador.terminate()

if __name__ == "__main__":
    main()