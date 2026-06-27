from json import JSONDecodeError, loads
from serial import Serial
from interfaces import Arena
from config import porta, velocidade

class Recebidor:
    def __init__(self):
        self.serial = Serial(porta, velocidade, timeout=1)
        print("Aguardando dados do robô...")

    def ouvir(self) -> bool:
        if self.serial.in_waiting > 0:
            dado_bruto = self.serial.readline()
            
            self.texto_limpo = dado_bruto.decode("utf-8").strip()

            return True
        return False
    
    def modificador(self) -> Arena | None:
        try:
            self.json =  loads(self.texto_limpo)

            arena = Arena(**self.json)

            return arena

        except JSONDecodeError as erro:
            print(f"Erro na decodificação do texto limpo para JSON: {erro}")

        except TypeError as erro:
            print(f"Erro na conversão do JSON para a interface da arena: {erro}")