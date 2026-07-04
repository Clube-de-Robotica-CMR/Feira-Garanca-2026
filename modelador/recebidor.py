import sys
from json import JSONDecodeError, loads

class Recebidor:
    def __init__(self, caminho: str):
        self.caminho_json = caminho
        self.texto_limpo = ""
        # sys.stderr garante que este print apareça no terminal e não polua o pipe
        print("[PC] Aguardando dados do robô via pipe...", file=sys.stderr)

    def ouvir(self) -> bool:
        """
        Lê a próxima linha disponível na entrada padrão (sys.stdin).
        Retorna True se uma linha foi capturada com sucesso.
        """
        # Lê apenas uma linha por vez do buffer do sys.stdin
        linha = sys.stdin.readline()
        
        # Se a linha for vazia, significa que o pipe foi fechado (fim da execução)
        if not linha:
            return False
            
        self.texto_limpo = linha.strip()
        return True
    
    def salvar_json(self):
        """
        Valida se o texto limpo é um JSON legítimo e atualiza o arquivo da arena.
        """
        # Ignora linhas vazias ou mensagens de log que não sejam o JSON do robô
        if not (self.texto_limpo.startswith('{') and self.texto_limpo.endswith('}')):
            return

        try:
            # Valida a sintaxe do JSON
            loads(self.texto_limpo)

            # Salva exatamente a string recebida no arquivo da arena
            with open(self.caminho_json, "w", encoding="utf-8") as arquivo:
                arquivo.write(self.texto_limpo)
                arquivo.flush()
                
            print("[PC] arena.json atualizado com sucesso!", file=sys.stderr)

        except JSONDecodeError as erro:
            print(f"[PC ERRO] Falha na decodificação do JSON: {erro}", file=sys.stderr)
            print(f"[PC DADO INVÁLIDO]: {self.texto_limpo}", file=sys.stderr)