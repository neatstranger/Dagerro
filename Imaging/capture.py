from picamera import PiCamera
import time
from fractions import Fraction
import datetime
camera = PiCamera(framerate=Fraction(1,6))


while True:
    cur_time = datetime.datetime.now()
    stub = cur_time.strftime("%Y%m%d%H%M%S_low")
    print(stub)
    # You can change these as needed. Six seconds (6000000)
    # is the max for shutter speed and 800 is the max for ISO.
    camera.shutter_speed = 24000000
    camera.iso = 800
    time.sleep(1)
    camera.exposure_mode = 'off'
    outfile = "%s.jpg" % (stub)
    camera.capture(outfile)
