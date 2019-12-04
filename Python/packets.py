from enum import Enum

# Command IDs
class PiToArduinoCmd(Enum):
  ECHO = 1
  SET_SPEEDLIMIT = 2
  SET_MOTORS = 3
  SET_OPENLOOP_TARGET = 4
  GET_ODOMETRY = 5
  GET_TICKS = 6
  OPENLOOP_STRAIGHT = 7
  OPENLOOP_R_CURVE = 8
  OPENLOOP_L_CURVE = 9
  CLOSEDLOOP = 10
  RESET_ODOMETRY = 11
  TURN_STATISTICS_ON = 12

# TODO: PREAMBLES?
class PiToArduinoPacket:
  # Creates a Pi->Arduino packet.
  # command: value of type PiToArduinoCmd
  # seq_num: sequence number
  # arg1, arg2, arg3: optional float arguments
  def __init__(self, command, seq_num, arg1=0, arg2=0, arg3=0):
    self.command = command
    self.arg1 = arg1
    self.arg2 = arg2
    self.arg3 = arg3
    self.seq_num = seq_num

  # Convert to formatted byte string
  def to_byte_string(self):
    args = []
    args.append(int(self.command.value).to_bytes(1, 'big'))
    args.append(int(1000 * self.arg1).to_bytes(4, 'big', signed=True))
    args.append(int(1000 * self.arg2).to_bytes(4, 'big', signed=True))
    args.append(int(1000 * self.arg3).to_bytes(4, 'big', signed=True))
    args.append(int(self.seq_num % 256).to_bytes(1, 'big'))
    return b''.join(args)

class ArduinoToPiRsp(Enum):
  CMD_1 = 1
  CMD_2 = 2
  CMD_3 = 3
  #CMD_GET_TICKS = 6
  CONTROL_FINISHED = 7
  STATISTICS = 8

class ArduinoToPiPacket:
  # Parses Arduino->Pi packet from the given byte array.
  # The byte array should be 14 bytes long.
  def __init__(self, byte_array):
    self.command = ArduinoToPiRsp(byte_array[0])
    self.arg1 = parse_float16(byte_array[1:5])
    self.arg2 = parse_float16(byte_array[5:9])
    self.arg3 = parse_float16(byte_array[9:13])
    self.seq_num = byte_array[13]

  def __str__(self):
    return '{}: {} {} {} {}'.format(self.command, self.arg1, \
      self.arg2, self.arg3, self.seq_num)

def parse_float16(byte_array_4):
  return int.from_bytes(bytes(byte_array_4), 'big', signed=True) / 1000.0

def parse_ufloat16(byte_array_4):
  return int.from_bytes(bytes(byte_array_4), 'big') / 1000.0

def parse_uint16(byte_array_4):
  return int.from_bytes(bytes(byte_array_4), 'big')
