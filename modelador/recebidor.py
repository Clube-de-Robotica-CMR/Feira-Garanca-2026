from json import JSONDecodeError, loads
from serial import Serial

class Recebidor:
    def __init__(self, caminho: str, porta: str, velocidade: int):
        self.serial = Serial(porta, velocidade, timeout=1)
        self.caminho_json = caminho
        self.texto_limpo = ""
        print("Aguardando dados do robô...")

    def ouvir(self) -> bool:
        if self.serial.in_waiting > 0:
            dado_bruto = self.serial.readline()
            
            self.texto_limpo = dado_bruto.decode("utf-8").strip()

            return True
        return False
    
    def salvar_json(self):
        try:
            loads(self.texto_limpo)

            with open(self.caminho_json, "w", encoding="utf-8") as arquivo:
                arquivo.write(self.texto_limpo)
                arquivo.flush()

        except JSONDecodeError as erro:
            print(f"Erro na decodificação do texto limpo para JSON: {erro}")
            print(f"Dado inválido recebido: {self.texto_limpo}")