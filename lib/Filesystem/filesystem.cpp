#include "filesystem.h"

void beginFileSystem() 
{
    if (!SPIFFS.begin(true)) 
    {
        LOG("SPIFFS", "Erro ao montar o sistema de arquivos SPIFFS!");
    } 
    else 
    {
        LOG("SPIFFS", "Sistema de arquivos SPIFFS inicializado com sucesso!");
    }
}
