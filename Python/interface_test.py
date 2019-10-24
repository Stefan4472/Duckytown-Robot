import time
from arduino_interface import ArduinoInterface, PiToArduinoPacket, ArduinoToPiPacket

a_int = ArduinoInterface('COM3', 9600, timeout=3)
time.sleep(2)
# r1 = a_int.echo(-1.5, -2.5, 3)
# print (r1)

# Construct the packet
send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_ECHO, \
    1, -5.237, -10.192, -1000.501)
print(send_packet.to_byte_string())
# Write the packet
bytes_written = a_int.serial_port.write(bytes(send_packet.to_byte_string()))
print(a_int.serial_port.readline())
raw_packet = a_int.serial_port.read(size=14)
print(raw_packet)
# Parse response
parsed_packet = ArduinoToPiPacket(bytes(raw_packet))
print (parsed_packet)