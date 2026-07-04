class Posicao:
    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y

    def to_dict(self):
        return {"x": self.x, "y": self.y}

class Objeto:
    def __init__(self, id_obj: int, x: float, y: float, dimensao: float):
        self.id = id_obj
        self.posicao = Posicao(x, y)
        self.dimensao = dimensao

    def to_dict(self):
        return {
            "id": self.id,
            "posicao": self.posicao.to_dict(),
            "dimensao": self.dimensao
        }