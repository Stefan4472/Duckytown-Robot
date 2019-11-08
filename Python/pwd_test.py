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

a_int.set_motor_pwm(100, 100)
while (1):
    print(a_int.serial_port.readline())
    
# while (1):
#     speed = int(input('Enter motor speed: '))
#     a_int.set_motor_pwm(speed, speed)
#     start_time = time.time()
#     for i in range(20):
#         time.sleep(0.1)
#         # wx, wy, wt = a_int.get_odometry()
#         # print('Got odometry x{} y{} t{}'.format(wx, wy, wt))
#         lt, rt = a_int.get_ticks()
#         print('Got ticks left {} right {}'.format(lt, rt))
#     # time.sleep(2)
#     a_int.set_motor_pwm(0, 0)
#     end_time = time.time()
#     print('Ran for {} seconds'.format(end_time - start_time))
