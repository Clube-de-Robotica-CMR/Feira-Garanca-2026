from pybricks.hubs import PrimeHub
from pybricks.parameters import Port
from pybricks.pupdevices import UltrasonicSensor
from pybricks.tools import Matrix, wait

from enviador import Enviador
from movimentador import Movimentador


class Mapeador:
    def __init__(self, movimentador: Movimentador, enviador: Enviador) -> None:
        self.movimentador = movimentador
        self.enviador = enviador
        self.ultrassom = UltrasonicSensor(Port.C)
        self.hub = PrimeHub()

        self.dimensao_arena = 900 #mm
        self.dimensao_robo = 250 #mm

        self.distancia_de_medicao = 10 #cm

        self.objetos_qtd = 0

    def objeto_identificado(self) -> bool:
        distancia = self.ultrassom.distance()
        return distancia < (self.dimensao_arena - self.dimensao_robo)
    
    def iniciar(self):
        for i in range(4):
            matriz = [[100 for j in range(5)] for i in range(5)]
            matriz = Matrix(matriz)
            self.hub.display.icon(matriz)

            self.movimentador.curvar_graus(-90)
            if self.objeto_identificado():
                self.mapear(i)
            self.movimentador.curvar_graus(90)
            self.movimentador.mover_cm(20)
            wait(100)
        matriz = [[100 for j in range(5)] for i in range(5)]
        matriz = Matrix(matriz)
        self.hub.display.icon(matriz)

        self.movimentador.curvar_graus(-90)
        if self.objeto_identificado():
            self.mapear(5)
        self.movimentador.curvar_graus(90)    

        self.movimentador.mover_cm(-80)

    def mapear(self, iteracao: int):
        matriz = [[0 if j != 2 or i == 3 else 100 for j in range(5)] for i in range(5)]
        matriz = Matrix(matriz)
        self.hub.display.icon(matriz)

        distancia_cm = self.ultrassom.distance() / 10.0
        
        self.objetos_qtd += 1
        dimensao = 10
        x = 70 - distancia_cm
        y = 10 + iteracao * 15

        self.enviador.adicionar_objeto(
            id_obj=self.objetos_qtd,
            x=x,
            y=y,
            dimensao=dimensao
        )
        self.enviador.enviar()