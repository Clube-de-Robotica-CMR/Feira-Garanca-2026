from pathlib import Path
import subprocess
import time

from modelador.recebidor import Recebidor

def main():
    raiz = Path(__file__).resolve().parent

    caminho_json = raiz / "modelador" / "arena.json"
    caminho_bin = raiz / "modelador" / "bin" / "renderizador"

    caminho_json_str = str(caminho_json)
    caminho_bin_str = str(caminho_bin)

    porta = "/dev/rfcomm0"
    velocidade = 115200

    recebidor = Recebidor(
        caminho=caminho_json_str,
        porta=porta,
        velocidade=velocidade
    )

    print("Iniciando renderizador...")

    renderizador = subprocess.Popen(
        [caminho_bin_str],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE
    )

    try:
        while True:
            if renderizador.poll() is not None:
                break

            if recebidor.ouvir():
                recebidor.salvar_json()

            time.sleep(0.001)

    except KeyboardInterrupt:
        print("\nFinalizando...")
    finally:
        if renderizador.poll() is None:
            renderizador.terminate()

if __name__ == "__main__":
    main()

