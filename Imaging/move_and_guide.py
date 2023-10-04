import serial
import io

mount = serial.Serial()
mount.baudrate = 115200
mount.port = "TTYACM0"


