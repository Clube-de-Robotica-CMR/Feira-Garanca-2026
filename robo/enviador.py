import json

from interfaces import Objeto

class Enviador:
    def __init__(self):
        # Mantemos apenas a lista de objetos que serão transmitidos
        self.objetos = []

    def adicionar_objeto(self, id_obj: int, x: float, y: float, dimensao: float):
        """Adiciona um obstáculo detectado pelo sensor à lista."""
        novo_objeto = Objeto(id_obj, x, y, dimensao)
        self.objetos.append(novo_objeto)

    def limpar(self):
        """Limpa os objetos detectados para preparar uma nova varredura."""
        self.objetos.clear()

    def enviar(self):
        """Converte APENAS a lista de objetos para JSON e transmite via Print (Serial)."""
        # Enviamos apenas a lista pura de objetos, reduzindo drasticamente o pacote
        dados_transmissao = [obj.to_dict() for obj in self.objetos]
        
        # Converte para string compacta de uma única linha
        json_string = json.dumps(dados_transmissao)
        
        # Envia imediatamente pelo canal Bluetooth
        print(json_string)