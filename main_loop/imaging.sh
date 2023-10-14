#!/bin/bash
#M56
goal_ra=289.0125000
goal_dec=30.188

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
    read -n1 -t 2 input 
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



#!/bin/bash
#M56
goal_ra=335.12370958
goal_dec=18.95620026
shutter=5000000
gain=7
#Starting Capture Process
echo "Capturing Image...Please Wait"
libcamera-still --shutter $shutter --gain $gain --awbgains 1,1 --immediate -o "../captures/image.jpg" -n --immediate  # Take a new photo
start_time=$(date +%s)
echo "Copying Newly Taken Image For Solving."
cp "../captures/image.jpg" "../captures/$start_time.jpg"
cancel=true
iter=0
feh --geometry 1000x1000 --reload 1 --scale-down --borderless ../captures/image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyUSB0" -b 115200 & \
echo "Starting Capture Loop." & \
while $cancel; do
    echo "Capturing Image...Please Wait"
    libcamera-still --shutter $shutter --gain $gain --awbgains 1,1 --immediate -o "../captures/image.jpg" -n --immediate  # Take a new photo
    start_time=$(date +%s)
    echo "Copying Newly Taken Image For Solving."
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    if  [[ $iter -eq 5 ]];then
        iter=0
        echo "Solving..."
        solve-field  --scale-units arcsecperpix  --scale-low 0.627 --scale-high 0.647 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
        get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
        get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
        echo "Calculating Expected Movement to Target"        
        /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
    fi
    iter=$iter+1
done