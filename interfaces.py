class Posicao:
    x: float
    y: float

class Dimensoes:
    largura: float
    comprimento: float

class ArenaObjeto:
    posicao: Posicao
    dimensoes: Dimensoes

class Arena:
    posicao_robo: Posicao
    objetos: list[ArenaObjeto]