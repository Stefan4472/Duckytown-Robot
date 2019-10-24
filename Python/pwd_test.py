import sys
import time
from arduino_interface import ArduinoInterface

print(sys.argv)
if len(sys.argv) != 2:
    print('Missing port argument. Usage is "pwd_test.py [SerialPort]"')
    sys.exit(1)

port = sys.argv[1]
print('Establishing serial connection on {}...'.format(port))
a_int = ArduinoInterface(port, 9600, timeout=3)
time.sleep(2)
print('Done')

while (1):
    speed = int(input('Enter motor speed: '))
    a_int.set_motor_pwm(speed, speed)
    time.sleep(2)
    a_int.set_motor_pwm(0, 0)
