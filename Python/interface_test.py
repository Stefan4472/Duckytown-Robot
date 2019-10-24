import time
from arduino_interface import ArduinoInterface, PiToArduinoPacket, ArduinoToPiPacket

a_int = ArduinoInterface('COM3', 9600, timeout=3)
time.sleep(2)
# r1 = a_int.echo(-5.237, -10.192, -1000.501)
# print(r1)
# r2 = a_int.get_ticks()
# print(r2)
while(1):
    print(a_int.serial_port.readline())