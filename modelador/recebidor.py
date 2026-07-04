import sys
from json import JSONDecodeError, loads

class Recebidor:
    def __init__(self, caminho: str):
        self.caminho_json = caminho
        self.texto_limpo = ""
        print("[PC] Aguardando dados dos objetos via pipe...", file=sys.stderr)

    def ouvir(self) -> bool:
        """Lê a próxima linha disponível na entrada padrão (Pipe do terminal)."""
        linha = sys.stdin.readline()
        
        # Se a linha for vazia, significa que o processo do robô encerrou
        if not linha:
            return False
            
        self.texto_limpo = linha.strip()
        
        # Validação do novo protocolo: o JSON agora é uma lista pura de objetos
        if self.texto_limpo.startswith('[') and self.texto_limpo.endswith(']'):
            return True
            
        return True # Retorna True para continuar o loop mesmo se receber uma linha de log comum

    def salvar_json(self):
        try:
            # Valida se o texto recebido é um JSON válido
            loads(self.texto_limpo)

            # Salva o array de objetos exatamente como veio
            with open(self.caminho_json, "w", encoding="utf-8") as arquivo:
                arquivo.write(self.texto_limpo)
                arquivo.flush()
                
            print("[PC] arena.json atualizado via Recebidor!", file=sys.stderr)

        except JSONDecodeError as erro:
            # Ignora erros de decodificação para prints normais de log que não sejam o JSON
            pass