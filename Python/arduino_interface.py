import serial

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

    def command_openloop_straight(self, speed, distance=0.0):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_OPENLOOP_STRAIGHT, \
            self.seq_num, speed, distance)
        self.serial_port.write(bytes(send_packet.to_byte_string()))

    def command_openloop_rcurve(self, speed, turn_radius, theta=0.0):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_OPENLOOP_R_CURVE, \
            self.seq_num, speed, turn_radius, theta)
        self.serial_port.write(bytes(send_packet.to_byte_string()))

    def command_openloop_lcurve(self, speed, turn_radius, theta=0.0):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_OPENLOOP_L_CURVE, \
            self.seq_num, speed, turn_radius, theta)
        self.serial_port.write(bytes(send_packet.to_byte_string()))

    def command_closedloop(self, wx, wy, wtheta):
        self.seq_num += 1
        send_packet = PiToArduinoPacket(PiToArduinoPacket.CMD_CLOSEDLOOP, \
            self.seq_num, wx, wy, wtheta)
        self.serial_port.write(bytes(send_packet.to_byte_string()))
