import toupcam
from PIL import Image


a = toupcam.Toupcam.EnumV2()
if len(a) > 0:
    print('{}: flag = {:#x}, preview = {}, still = {}'.format(a[0].displayname, a[0].model.flag, a[0].model.preview, a[0].model.still))
    for r in a[0].model.res:
        try:
            total = 0
            print('\t = [{} x {}]'.format(r.width, r.height))
            hcam = toupcam.Toupcam.Open(a[0].id)
            hcam.put_ExpoTime(1000000)
            hcam.put_Option(toupcam.TOUPCAM_OPTION_RAW, 1)
            hcam.put_Option(toupcam.TOUPCAM_OPTION_TRIGGER, 1)
            hcam.put_Option(toupcam.TOUPCAM_IOCONTROLTYPE_SET_TRIGGERSOURCE, 0x05)

            width, height = hcam.get_Size()
            bufsize = toupcam.TDIBWIDTHBYTES(width * 24) * height
            print('image size: {} x {}, bufsize = {}'.format(width, height, bufsize))
            buf = bytes(bufsize)
            hcam.TriggerSyncV4(0, buf, 24, 0, None)
            #hcam.PullImageV4(buf, 0, 24, 0, None)
            # total += 1
            print('pull image ok, total = {}'.format(total))
            # row_bytes = (width * 3 + 3) & ~3
            # image = Image.frombuffer("RGB", (width, height), buf, "raw", "BGR", row_bytes, 1)
            # image = image.transpose(Image.FLIP_TOP_BOTTOM)
            # filename = "image_{:03d}.bmp".format(total)
            # image.save(filename)
            # print('Image saved as {}'.format(filename))
        except toupcam.HRESULTException as ex:
                print('pull image failed, hr=0x{:x}'.format(ex.hr & 0xffffffff))