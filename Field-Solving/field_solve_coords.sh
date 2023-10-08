#!/bin/bash

#Go to Desktop Directory
cd ~/Desktop/

echo "Setting up and changing into directories."
start_time=$(date +%s)
mkdir $start_time && cd $start_time



echo "Ready For Focus, Press any key to Continue"
read -n1 empty


#Focusing Loop
mkdir focus && cd focus
echo "Taking First Focus Picture"
libcamera-still --shutter 5000000 --gain 8 --awbgains 1,1 -o test.jpg -n --immediate  # Take a new photo
cancel=true
feh --geometry 1000x1000 --reload 1 --scale-down --borderless test.jpg & \
while $cancel; do
    echo "Taking Focus Photo, Press c in 1 Sec to Cancel"
    read -n1 -t 1 input 
    if  [[ "$input" =~ 'c' ]];then
        cancel=false
    fi
    current_time=$(date +%s)
    cp test.jpg "$current_time".jpg #Save the Current Image to It's Timestamp
    libcamera-still --shutter 5000000 --gain 8 --awbgains 1,1 --immediate -o test.jpg -n --immediate  # Take a new photo
done

echo "Now we need to field solve an image to figure out where we are currently pointing."
echo "Using last focus image"
mkdir ../field_solve
cp test.jpg ../field_solve/
cd ../field_solve
solve-field --scale-units arcsecperpix --scale-low 1.1 --scale-high 1.3 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O test.jpg > solve_log.txt
echo "Field Solved" 
echo "Exporing Coordinates of Current Location"
get-wcs test.wcs | grep crval1  >> coords.txt
get-wcs test.wcs | grep crval2  >> coords.txt


#Output of this will look something like this:
#crval1 = 335.12370958
#crval2 = 18.95620026



#Andromeda:RA: 335.12370958, DEC: 18.95620026


#Andromeda = RA 0.73°, DEC 41.270694
#RA: 335.12370958, DEC: 18.95620026
# #Take a Picture
# libcamera-still --shutter 500000 --gain 

# #Field Solve Picture


# #export coordinates

# #Move Telescope?