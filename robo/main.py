from movimentador import Movimentador
from enviador import Enviador

enviador = Enviador()

movimentador = Movimentador(enviador)

if __name__ == "__main__":
    while True:
        