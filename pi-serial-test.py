import serial

# Command IDs
CMD_1 = 1
CMD_2 = 2
CMD_3 = 3

class PiToArduinoPacket:
    def __init__(self, commandID, arg1, arg2, arg3, seq_num):
        self.commandID = commandID
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3
        self.seq_num = seq_num

    # Convert to formatted byte string
    def to_bytes(self):
        args = []
        args.append(int(self.commandID).to_bytes(1, 'big'))
        args.append(int(1000 * self.arg1).to_bytes(4, 'big'))
        args.append(int(1000 * self.arg2).to_bytes(4, 'big'))
        args.append(int(1000 * self.arg3).to_bytes(4, 'big'))
        args.append(int(self.seq_num).to_bytes(1, 'big'))
        return b''.join(args)

# Sequence number
seq_num = 0

packet1 = PiToArduinoPacket(CMD_1, 1, 2, 3, seq_num)
print (packet1.to_byte_array())

# ser = serial.Serial('COM5', 9600)
# ser.flushInput()
#
# # byte = b'\x01'
# counter = 0
#
# while (1):  # TODO: MAKE SERIAL READS ASYNC AND CALL A HANDLER
#     # print ('Writing to port')
#     counter = (counter + 1) % 10
#     ser.write(bytes([counter]))
#
#     if ser.in_waiting:
#         # print ('{} bytes waiting'.format(ser.in_waiting));
#         # print (ord(ser.read()))
#         # print (char(ord(ser.read())))
#         print (ser.readline())
#     # if ser.out_waiting:
