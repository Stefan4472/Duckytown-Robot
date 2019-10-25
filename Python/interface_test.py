import time
from arduino_interface import ArduinoInterface, PiToArduinoPacket, ArduinoToPiPacket

a_int = ArduinoInterface('COM3', 9600, timeout=3)
time.sleep(2)

# r1 = a_int.echo(-5.237, -10.192, -1000.501)
# print(r1)
# r2 = a_int.get_ticks()
# print(r2)

a_int.set_motor_pwm(0, 200)
a_int.serial_port.flushInput()
for i in range(20):
    print(a_int.serial_port.readline())
a_int.set_motor_pwm(0, 0)