import argparse
import csv

def setupArgParser():
    parser = argparse.ArgumentParser(description="Calculate Difference Between Current Location and Destination", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '--ra', type=float, help='Right Ascension of Destination', required=True)
    parser.add_argument('-d', '--dec', type=float, help='Declination of Destination', required=True)
    parser.add_argument('-c', '--coords-file', type=str, help='Current Location Coords.TXT File', required=True)
    parser.add_argument('-o', '--output-file', type=str, help="Where to put the output csv file.", required=False)
    parser.add_argument('-w', '--write-to-file', help="Should we write the output?", required=False, action="store_true" )
    args = parser.parse_args()
    config = vars(args)
    return config['ra'], config['dec'], config['coords_file'], config['output_file'], config['write_to_file']

def getCurrentLocation(coords_file):
    lines = []
    with open (coords_file) as file:
        for line in file:
            lines.append(line.strip('\n').split(' ')[1])
    ra = float(lines[0])
    dec = float(lines[1])
    return ra, dec

def calculateMovement(current_ra, dest_ra, current_dec, dest_dec):
    dec_diff = 0
      #  -28   > 34 = -62
    if current_dec > dest_dec:
        dec_diff = dest_dec - current_dec
    elif current_dec < dest_dec:
        dec_diff = current_dec - dest_dec
    ra_diff = dest_ra - current_ra
    return ra_diff, dec_diff

def calculateArcSeconds(degrees):
    return degrees * 3600

def updateCSVFile(fileLocation, ra_arcseconds, dec_arcseconds):
    with open(fileLocation, 'a', newline='\n') as output:
        writer = csv.writer(output, delimiter=',')
        writer.writerow([ra_arcseconds, dec_arcseconds, 'false'])

def main():
    dest_ra, dest_dec, coords_file, output_file, write_out = setupArgParser()
    current_ra, current_dec = getCurrentLocation(coords_file)
    ra_diff_degrees, dec_diff_degrees = calculateMovement(current_ra, dest_ra, current_dec, dest_dec)
    ra_diff_arcseconds = calculateArcSeconds(ra_diff_degrees)
    dec_diff_arcseconds = calculateArcSeconds(dec_diff_degrees)
    if write_out:
        updateCSVFile(output_file, ra_diff_arcseconds, dec_diff_arcseconds)
    else:
        print("RA Degrees To Move: {ra}, DEC Degrees To Move: {dec}".format(ra=ra_diff_degrees, dec=dec_diff_degrees))
        print("RA ArcSeconds To Move: {ra}, DEC ArcSeconds To Move: {dec}".format(ra=ra_diff_arcseconds, dec=dec_diff_arcseconds))

if __name__ == '__main__':
    main()