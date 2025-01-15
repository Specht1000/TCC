Import("env")
env.Execute("esptool.py --chip esp32 erase_flash")