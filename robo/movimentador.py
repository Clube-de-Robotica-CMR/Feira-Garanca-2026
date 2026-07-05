from math import pi
from pybricks.parameters import Direction, Port
from pybricks.pupdevices import Motor
from pybricks.hubs import PrimeHub
from pybricks.tools import wait

class Movimentador:
    def __init__(self) -> None:
        self.veloc = 150  # Velocidade em graus por second (°/s)
        self.motor_esquerdo = Motor(Port.E, positive_direction=Direction.COUNTERCLOCKWISE)
        self.motor_direito = Motor(Port.F)
        self.hub = PrimeHub()
        self.imu = self.hub.imu

        self.raio_roda = 4  # cm
        # Quantos cm o robô anda por 1 grau de rotação do motor
        self.fator_conversao_graus_cm = (1 / 360) * (self.raio_roda * 2) * pi

    def mover_cm(self, x: float):
        """Move o robô para frente (x positivo) ou para trás (x negativo) por x cm."""
        if x == 0:
            return

        graus_motor = x / self.fator_conversao_graus_cm

        self.motor_esquerdo.reset_angle(0)
        self.motor_direito.reset_angle(0)

        self.motor_esquerdo.run_target(self.veloc, graus_motor, wait=False)
        self.motor_direito.run_target(self.veloc, graus_motor, wait=False)

        while not (self.motor_esquerdo.done() and self.motor_direito.done()):
            wait(10)

        self.motor_esquerdo.brake()
        self.motor_direito.brake()
        wait(100) 

    def iniciar_movimento(self, fator_direcao: int):
        """O robô começa a mover-se com base no fator_direcao. Um fator positivo move o robô para frente e um valor negativo move o robô para trás"""

        if not fator_direcao:
            return

        fator_direcao = -1 if fator_direcao < 0 else 1

        self.motor_esquerdo.run(self.veloc * fator_direcao)
        self.motor_direito.run(self.veloc * fator_direcao)

    def curvar_graus(self, graus: float):
        """Curva o robô no próprio eixo. Graus positivos = Direita, Negativos = Esquerda."""
        if graus == 0:
            return

        # Como o heading cresce para a direita e decresce para a esquerda continuamente,
        # o alvo é puramente a posição atual somada ao deslocamento desejado.
        self.imu.reset_heading(0)

        # Determina a direção do giro dos motores baseado no sinal de 'graus'
        fator_direcao = 1 if graus > 0 else -1

        # Liga os motores em sentidos opostos para rotacionar no próprio eixo
        self.motor_esquerdo.run(self.veloc * fator_direcao)
        self.motor_direito.run(-self.veloc * fator_direcao)

        # Monitora o giroscópio até que o robô cruze ou atinja o ângulo alvo
        if graus > 0:
            while self.imu.heading() < graus:
                wait(5)
        else:
            while self.imu.heading() > graus:
                wait(5)

        # Trava os motores imediatamente ao atingir o alvo
        self.motor_esquerdo.hold()
        self.motor_direito.hold()
        wait(100)  # Estabilização física

    def iniciar_curva(self, fator_direcao: int):
        """O robõ começa a curvar na direção escolhida. Positivo = direita, negativo = esquerda, nulo = nada"""
        if not fator_direcao:
            return

        fator_direcao = -1 if fator_direcao < 0 else 1

        self.motor_esquerdo.run(self.veloc * fator_direcao)
        self.motor_direito.run(-self.veloc * fator_direcao)

    def parar(self):
        self.motor_esquerdo.brake()
        self.motor_direito.brake()

    def resetar_imu(self):
        self.imu.reset_heading(0)
        wait(50)

    def angulacao(self):
        angulo = self.imu.heading()
        return abs(angulo) % 360