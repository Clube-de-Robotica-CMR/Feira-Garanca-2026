from enviador import Enviador
from movimentador import Movimentador
from pybricks.hubs import PrimeHub
from pybricks.parameters import Port
from pybricks.pupdevices import UltrasonicSensor
from pybricks.tools import Matrix, wait


class Mapeador:
    def __init__(self, movimentador: Movimentador, enviador: Enviador) -> None:
        self.movimentador = movimentador
        self.enviador = enviador
        self.ultrassom = UltrasonicSensor(Port.C)
        self.hub = PrimeHub()

        self.dimensao_arena = 900  # mm
        self.dimensao_robo = 300  # mm
        self.limiar_objeto = 500  # mm
        self.distancia_da_parede = 100  # mm
        self.toquinho = 150  # mm

        self.objetos_qtd = 0

    def objeto_identificado(self) -> bool:
        distancia = self.ultrassom.distance()
        return distancia < self.limiar_objeto

    def ir_para_mm(self, mm: float):
        if mm < self.distancia_da_parede:
            mm = self.distancia_da_parede

        distancia = self.ultrassom.distance()
        if distancia == mm:
            return

        fator = 1 if distancia > mm else -1

        self.movimentador.iniciar_movimento(fator)

        if distancia > mm:
            while self.ultrassom.distance() > mm:
                wait(10)

        else:
            while self.ultrassom.distance() < mm:
                wait(10)

        self.movimentador.parar()

    def iniciar(self):
        self.ir_para_mm(self.dimensao_arena - self.dimensao_robo)

        while self.ultrassom.distance() > self.distancia_da_parede:
            self.hub.display.number(self.objetos_qtd)

            distancia = self.ultrassom.distance() / 10

            self.movimentador.curvar_graus(-90)
            if self.objeto_identificado():
                self.mapear(distancia)
            self.movimentador.curvar_graus(90)
            self.ir_para_mm(self.ultrassom.distance() - self.toquinho)

        matriz = [
            [
                100
                if (j == 2) or (i == 3 and j in (1, 3)) or (i == 2 and j in (0, 4))
                else 0
                for j in range(5)
            ]
            for i in range(5)
        ]
        matriz = Matrix(matriz)
        self.hub.display.icon(matriz)

        self.ir_para_mm(self.dimensao_arena - self.dimensao_robo)

    def mapear(self, frontal: float):
        matriz = [[0 if j != 2 or i == 3 else 100 for j in range(5)] for i in range(5)]
        matriz = Matrix(matriz)
        self.hub.display.icon(matriz)

        distancia_cm = self.ultrassom.distance() / 10.0

        self.objetos_qtd += 1
        dimensao = 10
        x = 60 - distancia_cm
        y = 80 - frontal

        self.enviador.adicionar_objeto(
            id_obj=self.objetos_qtd, x=x, y=y, dimensao=dimensao
        )
        self.enviador.enviar()
