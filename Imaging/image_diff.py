import numpy as np
import cv2
import scipy.signal


RAArcMinutePerPixel = 5.9734
DecArcMinutePerPixel = 4.6906

def get_images():
    image1 = cv2.imread('First.png')
    image2 = cv2.imread('Second.png')
    return image1, image2

def cross_image(im1, im2):
   # get rid of the color channels by performing a grayscale transform
   # the type cast into 'float' is to avoid overflows
   im1_gray = np.sum(im1.astype('float'), axis=2)
   im2_gray = np.sum(im2.astype('float'), axis=2)
   # get rid of the averages, otherwise the results are not good
   im1_gray -= np.mean(im1_gray)
   im2_gray -= np.mean(im2_gray)
   # calculate the correlation image; note the flipping of onw of the images
   return scipy.signal.fftconvolve(im1_gray, im2_gray[::-1,::-1], mode='same')

def get_pixel_difference(image1, image2):
    image_dif = cross_image(image1, image2)
    cross_dimensions = np.unravel_index(np.argmax(image_dif), image_dif.shape)
    RAPixelDiff = cross_dimensions[0] - image1.shape[0]/2
    DecPixelDiff = cross_dimensions[1] - image1.shape[1]/2
    return RAPixelDiff, DecPixelDiff


def getArcSecondChange(image1, image2):
    RAPixelDiff, DecPixelDiff = get_pixel_difference(image1, image2)
    RAArcSecondChange = RAPixelDiff * RAArcMinutePerPixel * 60
    DecArcSecondChange = DecPixelDiff * DecArcMinutePerPixel * 60
    return RAArcSecondChange, DecArcSecondChange

image1, image2 = get_images()

RAArcSecondChange, DecArcSecondChange = getArcSecondChange(image1, image2)
print(RAArcSecondChange, DecArcSecondChange)
