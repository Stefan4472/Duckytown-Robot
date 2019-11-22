import time
from arduino_interface import ArduinoInterface, PiToArduinoPacket, ArduinoToPiPacket

def callback(arg1, arg2, arg3):
    print('Got args {} {} {}'.format(arg1, arg2, arg3))
    
a_int = ArduinoInterface('/dev/ttyACM0', 115200)
time.sleep(2)

a_int.echo(1, 2, 3, callback)
#a_int.serial_port.flushInput()
a_int.command_openloop_straight(10.0, 30.0, callback)
time.sleep(7)
a_int.process_buffer()

'''
# r1 = a_int.echo(-5.237, -10.192, -1000.501)
# print(r1)
# r2 = a_int.get_ticks()
# print(r2)

while 1:
    input('Press enter to continue')
    a_int.serial_port.flushInput()
    a_int.set_motor_pwm(200, 200)
    start_time = time.time()
    while(time.time() < start_time + 5.0):
        print(a_int.serial_port.readline())
    print('Turning off motors')
    a_int.set_motor_pwm(0, 0)
    start_time = time.time()
    while(time.time() < start_time + 2.0):
        print(a_int.serial_port.readline())
'''
