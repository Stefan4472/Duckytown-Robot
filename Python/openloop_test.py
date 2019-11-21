import sys
import time
from arduino_interface import ArduinoInterface

if len(sys.argv) != 2:
    print('Missing port argument. Usage is "pwd_test.py [SerialPort]"')
    sys.exit(1)

port = sys.argv[1]
print('Establishing serial connection on {}...'.format(port))
a_int = ArduinoInterface(port, 115000, timeout=3)
time.sleep(2)
print('Done')

a_int.serial_port.flushInput()
# a_int.command_openloop_rcurve(10, 10, 2*3.14)
#a_int.command_openloop_straight(10, 60)
try:
    a_int.command_openloop_rcurve(10.0, 5.0, 3.1415926535 / 2.0)
    while (1):
        print(a_int.serial_port.readline())
finally:
    a_int.command_motor_pwms(0, 0)
# start_time = time.time()
# while(time.time() < start_time + 5.0):
#     print(a_int.serial_port.readline())
# print('Turning off motors')
# a_int.set_motor_pwm(0, 0)
# start_time = time.time()
