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

goal_ra=335.12370958
goal_dec=18.95620026

mkdir ../capture_and_guide && cd ../capture_and_guide
mkdir captures && mkdir solves
cd solves



echo "Capturing Image...Please Wait"
cp "../1.jpg" "../captures/image.jpg" #Take a new image
sleep 2
start_time=$(date +%s)
cp "../captures/image.jpg" "../captures/$start_time.jpg"
solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv"

feh --geometry 1000x1000 --reload 1 --scale-down --borderless ../captures/image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyACM0" -b 115200 & \
while true; do
    echo "Taking Photo"
    cp "../2.jpg" "../captures/image.jpg"
    sleep 2
    start_time=$(date +%s)
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
    get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
    get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
    /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv"

    echo "Taking Photo"
    cp "../3.jpg" "../captures/image.jpg"
    sleep 2
    start_time=$(date +%s)
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
    get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
    get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
    /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv"
done
# #Field Solving
# echo "Now we need to field solve an image to figure out where we are currently pointing."
# echo "Using last focus image"
# mkdir ../field_solve
# cp test.jpg ../field_solve/
# cd ../field_solve
# solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O test.jpg > solve_log.txt
# echo "Field Solved" 
# echo "Exporing Coordinates of Current Location"
# get-wcs test.wcs | grep crval1  >> coords.txt
# get-wcs test.wcs | grep crval2  >> coords.txt





#Andromeda:RA: 335.12370958, DEC: 18.95620026
