import toupcam
import numpy as np
import cv2
from PIL import Image


class App:
    def __init__(self):
        self.hcam = None
        self.buf = None
        self.total = 0

# the vast majority of callbacks come from toupcam.dll/so/dylib internal threads
    @staticmethod
    def cameraCallback(nEvent, ctx):
        if nEvent == toupcam.TOUPCAM_EVENT_IMAGE:
            ctx.CameraCallback(nEvent)

    def CameraCallback(self, nEvent):
        if nEvent == toupcam.TOUPCAM_EVENT_IMAGE:
            try:
                # For RAW output, assume 16-bit per pixel (adjust if needed)
                # Here we pass 16 for the bit depth.
                self.hcam.PullImageV4(self.buf, 0, 16, 0, None)
                self.total += 1
                print('pull raw image ok, total = {}'.format(self.total))
                
                # Convert the raw data buffer to a NumPy array.
                # Assume each pixel is 16 bits, so use dtype=np.uint16.
                raw_array = np.frombuffer(self.buf, dtype=np.uint16).reshape((self.height, self.width))
                
                # Debayering (demosaicing): convert Bayer pattern to RGB.
                # Adjust cv2.COLOR_BayerBG2BGR if your sensor uses a different Bayer layout.
                rgb_image = cv2.cvtColor(raw_array, cv2.COLOR_BayerBG2BGR)
                
                # Optionally, if the image is upside down, flip it vertically.
                rgb_image = cv2.flip(rgb_image, 0)
                
                # Convert the NumPy array to a PIL Image and save it.
                image = Image.fromarray(rgb_image)
                filename = "raw_image_{:03d}.bmp".format(self.total)
                image.save(filename)
                print('RAW image saved as {}'.format(filename))
            except toupcam.HRESULTException as ex:
                print('pull raw image failed, hr=0x{:x}'.format(ex.hr & 0xffffffff))
        else:
            print('event callback: {}'.format(nEvent))

    def run(self):
        a = toupcam.Toupcam.EnumV2()
        if len(a) > 0:
            print('{}: flag = {:#x}, preview = {}, still = {}'.format(a[0].displayname, a[0].model.flag, a[0].model.preview, a[0].model.still))
            for r in a[0].model.res:
                print('\t = [{} x {}]'.format(r.width, r.height))
            self.hcam = toupcam.Toupcam.Open(a[0].id)
            if self.hcam:
                try:
                    self.hcam.put_ExpoTime(1000)
                    self.hcam.put_Option(toupcam.TOUPCAM_OPTION_RAW, 1)
                    self.hcam.put_Option(toupcam.TOUPCAM_OPTION_TRIGGER, 1)
                    self.hcam.put_Option(toupcam.TOUPCAM_IOCONTROLTYPE_SET_TRIGGERSOURCE, 0x05)
                    width, height = self.hcam.get_Size()
                    self.width, self.height = width, height
                    bufsize = self.width * self.height * 2
                    print('raw image size: {} x {}, bufsize = {}'.format(self.width, self.height, bufsize))
                    self.buf = bytes(bufsize)
                    if self.buf:
                        try:
                            self.hcam.StartPullModeWithCallback(self.cameraCallback, self)
                        except toupcam.HRESULTException as ex:
                            print('failed to start camera, hr=0x{:x}'.format(ex.hr & 0xffffffff))
                    self.hcam.Trigger(1)
                    input('press ENTER to exit')
                finally:
                    self.hcam.Close()
                    self.hcam = None
                    self.buf = None
            else:
                print('failed to open camera')
        else:
            print('no camera found')

if __name__ == '__main__':
    app = App()
    app.run()