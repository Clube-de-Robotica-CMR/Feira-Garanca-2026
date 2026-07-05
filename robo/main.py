from movimentador import Movimentador
from enviador import Enviador
from mapeador import Mapeador

enviador = Enviador()

movimentador = Movimentador()

mapeador = Mapeador(
    movimentador=movimentador, 
    enviador=enviador
    )

if __name__ == "__main__":
    mapeador.iniciar()