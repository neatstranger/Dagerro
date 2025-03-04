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
                # Pull raw image with 16-bit depth.
                self.hcam.PullImageV4(self.buf, 0, 16, 0, None)
                self.total += 1
                print('pull raw image ok, total = {}'.format(self.total))
                
                # Convert the raw byte buffer to a NumPy array (16-bit grayscale).
                # The buffer size and sensor dimensions were set in run().
                raw_array = np.frombuffer(self.buf, dtype=np.uint16).reshape((self.height, self.width))
                
                # Debayer the raw Bayer pattern data to get an RGB image.
                # Adjust the conversion flag if your sensor uses a different Bayer pattern.
                rgb_image = cv2.cvtColor(raw_array, cv2.COLOR_BayerBG2BGR)
                
                # Flip vertically if needed (DIB images are often stored upside-down).
                rgb_image = cv2.flip(rgb_image, 0)
                
                # Save the image using OpenCV's imwrite to preserve 16-bit data.
                filename = "raw_image_{:03d}.png".format(self.total)
                cv2.imwrite(filename, rgb_image)
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
                    self.hcam.put_ExpoTime(2000)
                    self.hcam.put_Option(toupcam.TOUPCAM_OPTION_RAW, 1)
                    self.hcam.put_Option(toupcam.TOUPCAM_OPTION_BITDEPTH, 1)
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