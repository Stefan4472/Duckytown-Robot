import time
from arduino_interface import ArduinoInterface, PiToArduinoPacket, ArduinoToPiPacket

def callback(arg1, arg2, arg3):
    print('Got callback with args {} {} {}'.format(arg1, arg2, arg3))
    
a_int = ArduinoInterface('/dev/ttyACM0', 115200, timeout=0.0)
time.sleep(2)
a_int.serial_port.flushInput()

for i in range(200):
    a_int.echo(i, 0, 0, callback)
    a_int.process_buffer(debug=False)
    #input('Press Enter\n')

#a_int.echo(10, 0, 0, callback)
while True:
    a_int.process_buffer(debug=False)
