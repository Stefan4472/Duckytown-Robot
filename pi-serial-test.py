import serial

ser = serial.Serial('COM5', 9600)
ser.flushInput()

# byte = b'\x01'
counter = 0

while (1):
    # print ('Writing to port')
    counter = (counter + 1) % 10
    ser.write(bytes([counter]))

    if ser.in_waiting:
        # print ('{} bytes waiting'.format(ser.in_waiting));
        # print (ord(ser.read()))
        # print (char(ord(ser.read())))
        print (ser.readline())
    # if ser.out_waiting:
