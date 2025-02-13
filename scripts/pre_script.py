import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

print("🚀 PlatformIO carregou o pre_script.py com sucesso!")

# Executa att_html.py ANTES de qualquer coisa
script_path = os.path.join(env.subst("$PROJECT_DIR"), "att_html.py")
print(f"🔄 Executando script de atualização diretamente: {script_path}")

os.system(f"python {script_path}")