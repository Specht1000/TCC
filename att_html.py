import re

# Caminhos dos arquivos
html_path = "lib/Webpage/index.html"  # HTML cru que será inserido no webpage.h
webpage_h_path = "lib/Webpage/webpage.h"  # Arquivo webpage.h

# Função para ler o conteúdo do arquivo HTML sem modificar caracteres especiais
def ler_html(caminho):
    try:
        with open(caminho, "r", encoding="utf-8") as arquivo:
            conteudo = arquivo.read()
            return conteudo
    except FileNotFoundError:
        print(f"❌ Erro: Arquivo não encontrado -> {caminho}")
        return None

# Função para atualizar o arquivo webpage.h
def atualizar_webpage_h():
    # Lê o conteúdo do HTML e do webpage.h
    html = ler_html(html_path)
    try:
        with open(webpage_h_path, "r", encoding="utf-8") as arquivo:
            webpage_h = arquivo.read()
    except FileNotFoundError:
        print(f"❌ Erro: Arquivo não encontrado -> {webpage_h_path}")
        return

    if html is None:
        return

    print("🔍 HTML lido com sucesso. Iniciando substituição no webpage.h...")

    # Regex para encontrar o bloco de conteúdo no webpage.h
    padrao = re.compile(r'(const char webpageHTML\[\] = R"rawliteral\()[\s\S]*?(?=\)rawliteral";)', re.DOTALL)

    # Substituição do conteúdo
    novo_webpage_h = re.sub(padrao, rf'\1\n{html}\n', webpage_h)

    if novo_webpage_h == webpage_h:
        print("⚠️ Nenhuma alteração feita! O conteúdo pode estar idêntico ao anterior.")
        return

    # Salva o novo conteúdo no webpage.h sem alterar formatação
    try:
        with open(webpage_h_path, "w", encoding="utf-8", newline="\n") as arquivo:
            arquivo.write(novo_webpage_h)
        print("✅ Arquivo webpage.h atualizado com sucesso!")
    except PermissionError:
        print("🚨 Erro de permissão ao tentar salvar webpage.h! Execute como administrador.")

# Executar a função
atualizar_webpage_h()