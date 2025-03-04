import toupcam


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
                self.hcam.PullImageV4(self.buf, 1, 24, 0, None)
                self.total += 1
                print('pull image ok, total = {}'.format(self.total))
                
                # Calculate row_bytes according to the DIB (padded to a multiple of 4 bytes)
                row_bytes = (self.width * 3 + 3) & ~3
                # Create the image (assuming raw data in BGR format)
                image = Image.frombuffer("RGB", (self.width, self.height), self.buf, "raw", "BGR", row_bytes, 1)
                # Flip vertically if necessary (DIB images are usually bottom-up)
                image = image.transpose(Image.FLIP_TOP_BOTTOM)
                
                # Save the image in the working directory
                filename = "image_{:03d}.raw".format(self.total)
                image.save(filename)
                print('Image saved as {}'.format(filename))
            except toupcam.HRESULTException as ex:
                print('pull image failed, hr=0x{:x}'.format(ex.hr & 0xffffffff))
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
                    width, height = self.hcam.get_Size()
                    self.width, self.height = width, height
                    bufsize = toupcam.TDIBWIDTHBYTES(width * 24) * height
                    print('image size: {} x {}, bufsize = {}'.format(width, height, bufsize))
                    self.buf = bytes(bufsize)
                    if self.buf:
                        try:
                            self.hcam.StartPullModeWithCallback(self.cameraCallback, self)
                        except toupcam.HRESULTException as ex:
                            print('failed to start camera, hr=0x{:x}'.format(ex.hr & 0xffffffff))
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