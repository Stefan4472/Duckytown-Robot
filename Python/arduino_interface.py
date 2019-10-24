import serial

# TODO: PREAMBLES?
class PiToArduinoPacket:
    # Command IDs
    CMD_ECHO = 1
    CMD_SET_SPEEDLIMIT = 2
    CMD_SET_MOTORS = 3
    CMD_SET_OPENLOOP_TARGET = 4
    CMD_GET_ODOMETRY = 5
    CMD_GET_TICKS = 6

    def __init__(self, commandID, seq_num, arg1=0, arg2=0, arg3=0):
        self.commandID = commandID
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3
        self.seq_num = seq_num

    # Convert to formatted byte string
    def to_byte_string(self):
        args = []
        args.append(int(self.commandID).to_bytes(1, 'big'))
        args.append(int(1000 * self.arg1).to_bytes(4, 'big', signed=True))
        args.append(int(1000 * self.arg2).to_bytes(4, 'big', signed=True))
        args.append(int(1000 * self.arg3).to_bytes(4, 'big', signed=True))
        args.append(int(self.seq_num).to_bytes(1, 'big'))
        return b''.join(args)


class ArduinoToPiPacket:
    # Command IDs  TODO: USE ENUMS
    CMD_1 = 1
    CMD_2 = 2
    CMD_3 = 3
    CMD_GET_TICKS = 6

    def parse_float16(byte_array_4):
        return int.from_bytes(bytes(byte_array_4), 'big', signed=True) / 1000.0

    def parse_ufloat16(byte_array_4):
        return int.from_bytes(bytes(byte_array_4), 'big') / 1000.0

    def parse_uint16(byte_array_4):
        return int.from_bytes(bytes(byte_array), 'big')

    # Parse bytestring
    def __init__(self, byte_array):
        self.commandID = byte_array[0]
        self.arg1 = ArduinoToPiPacket.parse_float16(byte_array[1:5])
        self.arg2 = ArduinoToPiPacket.parse_float16(byte_array[5:9])
        self.arg3 = ArduinoToPiPacket.parse_float16(byte_array[9:13])
        self.seq_num = byte_array[13]

    def __str__(self):
        return '{}: {} {} {} {}'.format(self.commandID, self.arg1, \
            self.arg2, self.arg3, self.seq_num)


class ArduinoInterface:
    def __init__(self, port, baudrate, timeout=None):
        self.serial_port = serial.Serial(port, baudrate, timeout=timeout)
        self.serial_port.flushInput()
        self.seq_num = -1
        self.serial_ready = False  # TODO: WAIT FOR IT TO BE READY. ASYNC?

    def echo(self, arg1, arg2, arg3):
        self.seq_num += 1
        # Construct the packet
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_ECHO, \
            self.seq_num, arg1, arg2, arg3)
        # Write the packet
        bytes_written = self.serial_port.write(bytes(send_packet.to_byte_string()))
        # Read the response
        raw_packet = self.serial_port.read(size=14)
        # Parse response
        parsed_packet = ArduinoToPiPacket(bytes(raw_packet))
        # Return values
        return (parsed_packet.arg1, parsed_packet.arg2, parsed_packet.arg3)

    def set_motor_pwm(self, left, right):
        self.seq_num += 1
        # Construct the packet
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_SET_MOTORS, \
            self.seq_num, left, right)
        # Write the packet
        bytes_written = self.serial_port.write(bytes(send_packet.to_byte_string()))
        
    def get_odometry(self):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_GET_ODOMETRY, \
            self.seq_num)
        bytes_written = self.serial_port.write(bytes(send_packet.to_byte_string()))
        raw_packet = self.serial_port.read(size=14)
        parsed_packet = ArduinoToPiPacket(bytes(raw_packet))
        return (parsed_packet.arg1, parsed_packet.arg2, parsed_packet.arg3)

    def get_ticks(self):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_GET_TICKS, \
            self.seq_num)
        bytes_written = self.serial_port.write(bytes(send_packet.to_byte_string()))
        raw_packet = self.serial_port.read(size=14)
        print('Raw packet {}'.format(raw_packet))
        parsed_packet = ArduinoToPiPacket(bytes(raw_packet))
        return (parsed_packet.arg1, parsed_packet.arg2)
