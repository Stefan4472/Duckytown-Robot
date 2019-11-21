import sys
import time
from arduino_interface import ArduinoInterface

if len(sys.argv) != 2:
    print('Missing port argument. Usage is "pwd_test.py [SerialPort]"')
    sys.exit(1)

port = sys.argv[1]
print('Establishing serial connection on {}...'.format(port))
a_int = ArduinoInterface(port, 115200, timeout=3)
time.sleep(2)
print('Done')

a_int.serial_port.flushInput()

#a_int.command_openloop_straight(10, 100)
try:
    while (1):
        a_int.command_closedloop(32, 0, 0)
        while a_int.serial_port.in_waiting:
            print(a_int.serial_port.readline())
        time.sleep(0.1)
finally:
    print('Turning motors off')
    a_int.command_motor_pwms(0, 0)
# start_time = time.time()
# while(time.time() < start_time + 5.0):
#     print(a_int.serial_port.readline())
# print('Turning off motors')
# a_int.set_motor_pwm(0, 0)
# start_time = time.time()

