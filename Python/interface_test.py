import time
from arduino_interface import ArduinoInterface 

a_int = ArduinoInterface('COM5', 9600, timeout=3)
time.sleep(2)
r1 = a_int.echo(1, 2, 3)
print (r1)