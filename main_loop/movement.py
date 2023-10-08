import argparse

def setupArgParser():
    parser = argparse.ArgumentParser(description="Calculate Difference Between Current Location and Destination", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '--ra', type=float, help='Right Ascension of Destination', required=True)
    parser.add_argument('-d', '--dec', type=float, help='Declination of Destination', required=True)
    parser.add_argument('-c', '--coords-file', type=str, help='Current Location Coords.TXT File', required=True)
    args = parser.parse_args()
    config = vars(args)
    return config['ra'], config['dec'], config['coords_file']

def getCurrentLocation(coords_file):
    lines = []
    with open (coords_file) as file:
        for line in file:
            lines.append(line.strip('\n').split(' ')[1])
    ra = float(lines[0])
    dec = float(lines[1])
    return ra, dec

def calculateMovement(current_ra, dest_ra, current_dec, dest_dec):
    ra_diff = dest_ra - current_ra
    dec_diff = dest_dec - current_dec
    return ra_diff, dec_diff

def calculateArcSeconds(degrees):
    return degrees * 3600

def main():
    dest_ra, dest_dec, coords_file = setupArgParser()
    current_ra, current_dec = getCurrentLocation(coords_file)
    ra_diff_degrees, dec_diff_degrees = calculateMovement(current_ra, dest_ra, current_dec, dest_dec)
    ra_diff_arcseconds = calculateArcSeconds(ra_diff_degrees)
    dec_diff_arcseconds = calculateArcSeconds(dec_diff_degrees)
    print("RA Difference: " + str(ra_diff_degrees) + " degrees")
    print("Dec Difference: " + str(ra_diff_degrees) + " degrees")

if __name__ == '__main__':
    main()