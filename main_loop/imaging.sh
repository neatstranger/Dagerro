#!/bin/bash
#Andromeda
goal_ra=335.12370958
goal_dec=18.95620026

#Go to Desktop Directory
cd ~/Desktop/

#Creating and moving into new directory with start time
echo "Setting up and changing into directories."
start_time=$(date +%s)
mkdir $start_time && cd $start_time


#Confirm Focus From User
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


#Set up Directory For Capture and Guiding
mkdir ../capture_and_guide && cd ../capture_and_guide
mkdir captures && mkdir solves
cd solves



echo "Capturing Image...Please Wait"
cp "../1.jpg" "../captures/image.jpg" #Take a new image
start_time=$(date +%s)
echo "Copying Newly Taken Image For Solving."
cp "../captures/image.jpg" "../captures/$start_time.jpg"
echo "Solving..."
solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
echo "Calculating Expected Movement to Target"
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt"
echo "Would you like to send these commands to the mount? y/n"
read -n1 input
if  [[ "$input" =~ 'y' ]];then
        /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
    fi
feh --geometry 1000x1000 --reload 1 --scale-down --borderless ../captures/image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyACM0" -b 115200 & \
echo "Starting Capture Loop." & \
while true; do
    echo "Capturing Image...Please Wait"
    cp "../2.jpg" "../captures/image.jpg"
    start_time=$(date +%s)
    echo "Copying Newly Taken Image For Solving."
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    echo "Solving..."
    solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
    get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
    get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
    echo "Calculating Expected Movement to Target"
    /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt"
    echo "Would you like to send these commands to the mount? y/n"
    read -n1 input
    if  [[ "$input" =~ 'y' ]];then
        /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
    fi
    echo "Capturing Image...Please Wait"
    cp "../3.jpg" "../captures/image.jpg"
    start_time=$(date +%s)
    echo "Copying Newly Taken Image For Solving."
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    echo "Solving..."
    solve-field --scale-units arcsecperpix --scale-low 1.18 --scale-high 1.20 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
    get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
    get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
    echo "Calculating Expected Movement to Target"
    /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt"
    echo "Would you like to send these commands to the mount? y/n"
    read -n1 input
    if  [[ "$input" =~ 'y' ]];then
        /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
    fi
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