import sys
import time
from arduino_interface import ArduinoInterface

if len(sys.argv) != 2:
    print('Missing port argument. Usage is "pwd_test.py [SerialPort]"')
    sys.exit(1)

port = sys.argv[1]
print('Establishing serial connection on {}...'.format(port))
a_int = ArduinoInterface(port, 9600, timeout=3)
time.sleep(2)
print('Done')

a_int.command_openloop_lcurve(20, 20, 6.28)
while (1):
    print(a_int.serial_port.readline())