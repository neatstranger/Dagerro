#!/bin/bash

#Go to Desktop Directory
cd ~/Desktop/

echo "Setting up and changing into directories."
start_time=test
#start_time=$(date +%s)
mkdir $start_time && cd $start_time
mkdir focus && cd focus


echo "Ready For Focus, Press any key to Continue"
read -n1 empty
echo "Taking First Focus Picture"

#feh --geometry 640x360 --reload 1 --scale-down --borderless test.jpg
#echo "Press c to continue"


#!/bin/bash
#Focusing Loop
cp ~/Desktop/solve_test/long6.jpg ./test.jpg
cancel=true
feh --geometry 640x360 --reload 1 --scale-down --borderless test.jpg & \
while $cancel; do
    echo "Taking Focus Photo, Press c in 1 Sec to Cancel"
    read -n1 -t 1 input 
    if  [[ "$input" =~ 'c' ]];then
        cancel=false
    fi
    current_time=$(date +%s)
    cp test.jpg "$current_time".jpg
    cp ~/Desktop/solve_test/long10.jpg ./test.jpg #Photo Taking Line

    
    echo "Taking Focus Photo, Press c to Cancel"
    read -n1 -t 2 input
    if  [[ "$input" =~ 'c' ]];then
        cancel=false
    fi
    current_time=$(date +%s)
    cp test.jpg "$current_time".jpg
    cp ~/Desktop/solve_test/long6.jpg ./test.jpg
done





# #Take a Picture
# libcamera-still --shutter 500000 --gain 

# #Field Solve Picture


# #export coordinates
# get-wcs test.wcs | grep crval1  >> coords.txt
# get-wcs test.wcs | grep crval2  >> coords.txt

# #Move Telescope?