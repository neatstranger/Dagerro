import serial
import argparse
import csv
import time


def setupSerial(port, baud):
    mount = serial.Serial(port, baudrate=baud, timeout=1)
    return mount

def serialGetData(mount):
    all_lines = []
    reading = None
    while reading != b'':
        reading = mount.readline(100)
        all_lines.append(reading.strip(b'\r\n').decode())
    for line in all_lines:
        print(line)

def sendSerialMovement(ra, dec, mount):
    command_string = str(ra)+";"+str(dec)+'\r'
    byte_string = bytes(command_string, 'utf-8')
    print("Sending command to mount: "+command_string)
    mount.write(byte_string)
    serialGetData(mount)

def setupArgParser():
    parser = argparse.ArgumentParser(description="Look at a csv file and move according to that.", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-f', '--movement-file', type=str, help='CSV File With Moves', required=True)
    parser.add_argument('-s', '--serial-port', type=str, help='Serial Port to Connect to Mount', required=True)
    parser.add_argument('-b', '--baudrate', type=int, help="Serial port baud rate")
    args = parser.parse_args()
    config = vars(args)
    return config['movement_file'], config['serial_port'], config['baudrate']

def updateCSVFile(fileLocation, ra_arcseconds, dec_arcseconds):
    with open(fileLocation, 'a', newline='\n') as output:
        writer = csv.writer(output, delimiter=',')
        writer.writerow([ra_arcseconds, dec_arcseconds, 'true'])

def getMovmentCommand(file_location):
    last_line = []
    ra_movement = None
    dec_movement = None
    with open(file_location, 'r', newline='\n') as movement_file:
        csv_reader = csv.reader(movement_file, delimiter=',')
        for line in csv_reader:
            last_line = line
    if last_line[2] == 'false':
        ra_movement = float(last_line[0])
        dec_movement = float(last_line[1])
        updateCSVFile(file_location, ra_movement, dec_movement)
    return ra_movement, dec_movement
    

def main():
    move_file, serial_port, baudrate = setupArgParser()
    mount = None
    try:
        mount = setupSerial(serial_port, baudrate)
    except:
        print("Could Not Start Serial Port, Please Check Connection and Try again")
        exit()
    time.sleep(1)
    serialGetData(mount)
    while True:
        ra, dec = getMovmentCommand(move_file)
        if(ra != None or dec != None):
            sendSerialMovement(ra, dec, mount)
        time.sleep(1)


if __name__ == '__main__':
    main()




