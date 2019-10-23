import time
from arduino_interface import ArduinoInterface

print('Establishing serial connection...')
a_int = ArduinoInterface('COM5', 9600, timeout=3)
time.sleep(2)
print('Done')

while (1):
    speed = int(input('Enter motor speed: '))
    a_int.set_motor_pwm(speed, speed)
    time.sleep(2)
    a_int.set_motor_pwm(0, 0)
