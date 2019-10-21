import serial
import time

# TODO: PREAMBLES?
class PiToArduinoPacket:
    # Command IDs
    CMD_1 = 1
    CMD_2 = 2
    CMD_3 = 3

    def __init__(self, commandID, seq_num, arg1, arg2=0, arg3=0):
        self.commandID = commandID
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3
        self.seq_num = seq_num

    # Convert to formatted byte string
    def to_byte_string(self):
        args = []
        args.append(int(self.commandID).to_bytes(1, 'big'))
        args.append(int(1000 * self.arg1).to_bytes(4, 'big'))
        args.append(int(1000 * self.arg2).to_bytes(4, 'big'))
        args.append(int(1000 * self.arg3).to_bytes(4, 'big'))
        args.append(int(self.seq_num).to_bytes(1, 'big'))
        return b''.join(args)


class ArduinoToPiPacket:  # TODO: WHAT KIND OF ENDIANNESS?
    # Command IDs
    CMD_1 = 1
    CMD_2 = 2
    CMD_3 = 3

    def parse_ufloat16(byte_array_4):  # TODO: SHOULD THIS BE LITTLE ENDIAN?
        print('Parsing {}'.format(byte_array_4))
        return int.from_bytes(bytes(byte_array_4), 'big') / 1000.0

    def parse_uint16(byte_array_4):
        return int.from_bytes(bytes(byte_array), 'big')

    # Parse bytestring
    def __init__(self, byte_array):
        print(byte_array)
        print(byte_array[0])
        print(byte_array[1:5])
        self.commandID = byte_array[0]
        self.arg1 = ArduinoToPiPacket.parse_ufloat16(byte_array[1:5])
        self.arg2 = ArduinoToPiPacket.parse_ufloat16(byte_array[5:9])
        self.arg3 = ArduinoToPiPacket.parse_ufloat16(byte_array[9:13])
        self.seq_num = byte_array[13]

    def __str__(self):
        return '{}: {} {} {} {}'.format(self.commandID, self.arg1, \
            self.arg2, self.arg3, self.seq_num)



# Sequence number
seq_num = 1

packet1 = PiToArduinoPacket(PiToArduinoPacket.CMD_1, seq_num, 1, 2, 3)
p1_str = packet1.to_byte_string()
print(p1_str)


ser = serial.Serial('COM5', 9600, timeout=3)
ser.flushInput()
# Sleep while serial port initializes
time.sleep(2)
bytes_written = ser.write(bytes(p1_str))
print('Wrote {} bytes'.format(bytes_written))
print('Wrote the packet {}'.format(p1_str))
while (1):  # TODO: MAKE SERIAL READS ASYNC AND CALL A HANDLER
    if ser.in_waiting:
        print()
        # print('{} bytes waiting'.format(ser.in_waiting))
        # print(ser.readline())
        # print(ser.read(size=14))

        line = ser.readline()
        print ('Read {}'.format(line))
        read_packet = ArduinoToPiPacket(bytes(line))
        print(read_packet)
        print()
