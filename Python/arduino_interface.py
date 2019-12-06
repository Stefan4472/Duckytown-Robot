from serial import Serial
from packets import ArduinoToPiPacket, PiToArduinoPacket, PiToArduinoCmd, ArduinoToPiRsp

class ArduinoInterface:
  def __init__(self, port, baudrate, timeout=3.0):  # NOTE: REQUIRES TWO SECONDS TO START
    self.serial_port = Serial(port, baudrate, timeout=timeout)
    self.serial_port.flushInput()
    self.seq_num = -1
    self.callbacks = {}
    # self.packet_queue = []
    # self.serial_ready = False  # TODO: WAIT FOR IT TO BE READY. ASYNC?

  # Reads the buffer and processes packets.
  # Set debug=True to print off strings.
  def process_buffer(self, debug=False):
    if debug:
      while self.serial_port.in_waiting:
        print(self.serial_port.readline())
    else:
      while self.serial_port.in_waiting >= 14:
        bytes_read = self.serial_port.read(14)
        #print('Got bytes {}'.format(bytes_read))
        read_packet = ArduinoToPiPacket(bytes_read)
        # Look up whether there is a registered callback function
        if read_packet.seq_num in self.callbacks:
          print('Found a callback')
          self.callbacks[read_packet.seq_num] \
              (read_packet.arg1, read_packet.arg2, read_packet.arg3)

  # Creates and sends the packet. Also registers the callback function.
  def send_packet(self, command_id, arg1=0.0, arg2=0.0, arg3=0.0, callback_fcn=None):
    #print('Sending packet')
    # Increment and save sequence number
    self.seq_num += 1
    seq_num = self.seq_num
    # Construct the packet
    send_packet = PiToArduinoPacket(command_id, seq_num, arg1, arg2, arg3)
    # Register the callback, if any
    if callback_fcn:
      self.callbacks[seq_num] = callback_fcn
    # Send the packet
    self.serial_port.write(bytes(send_packet.to_byte_string()))

  def echo(self, arg1, arg2, arg3, callback=None):
    self.send_packet(PiToArduinoCmd.ECHO, arg1, arg2, arg3, callback)

  def set_speed_limit(self, cm_per_sec):
    self.send_packet(PiToArduinoCmd.SET_SPEEDLIMIT, cm_per_sec)
      
  def command_motor_pwms(self, left, right, callback=None):
    self.send_packet(PiToArduinoCmd.SET_MOTORS, arg1=left, arg2=right, \
        callback_fcn=callback)

  def get_odometry(self, callback):
    self.send_packet(PiToArduinoCmd.GET_ODOMETRY, callback_fcn=callback)

  # def get_ticks(self, callback):
  #   self.send_packet(PiToArduinoCmd.CMD_GET_TICKS, callback_fcn=callback)

  def command_openloop_straight(self, speed, distance, callback=None):
    self.send_packet(PiToArduinoCmd.OPENLOOP_STRAIGHT, arg1=speed, \
        arg2=distance, callback_fcn=callback)

  def command_openloop_rcurve(self, speed, turn_radius, theta, callback=None):
      self.send_packet(PiToArduinoCmd.OPENLOOP_R_CURVE, arg1=speed, \
          arg2=turn_radius, arg3=theta, callback_fcn=callback)

  def command_openloop_lcurve(self, speed, turn_radius, theta, callback=None):
      self.send_packet(PiToArduinoCmd.OPENLOOP_L_CURVE, arg1=speed, \
          arg2=turn_radius, arg3=theta, callback_fcn=callback)
      #self.serial_port.write(bytes(self.send_packet.to_byte_string()))

  # Sends a closed-loop command with the given robot coordinates (x, y, theta)
  def command_closedloop(self, speed, theta_error, callback=None):
      self.send_packet(PiToArduinoCmd.CLOSEDLOOP, arg1=speed, arg2=theta_error, \
          callback_fcn=callback)
          
  def reset_odometry(self):
    self.send_packet(PiToArduinoCmd.RESET_ODOMETRY)
    
  def turn_statistics_on(self, period_sec, callback=None):
      self.send_packet(PiToArduinoCmd.TURN_STATISTICS_ON, arg1=period_sec, callback_fcn=callback)
