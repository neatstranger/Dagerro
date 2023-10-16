#!/bin/bash
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

#Calculate Coordinates of Destination From RA HH:MM:SS
#((Hours * 60 * 60) + (Minutes*60)+Seconds)
#------------------------------------------- * 15 = Conversion
#                  3600
#24 Hours = 360 Degrees
#1 Hour = 15 Degrees
#3600 Seconds = 54000





#!/bin/bash
#M56
#Andromeda = RA_10.6917, DEC_41.2711
#Triangulum Galaxy = RA_23.46825, DEC_30.661
#Deer Lick Group = RA_339.271, 34.41975
#Fireworks Galaxy = RA_317.5411714, DEC_60.15919444

goal_ra=317.5411714
goal_dec=60.15919444
shutter=30000000
gain=8
#Starting Capture Process
echo "Capturing Image...Please Wait"
libcamera-still --shutter $shutter --gain $gain --awbgains 1,1 -r -o "../captures/image.jpg" -n --immediate  # Take a new photo
start_time=$(date +%s)
echo "Copying Newly Taken Image For Solving."
cp "../captures/image.jpg" "../captures/$start_time.jpg"
cp "../captures/image.dng" "../captures/$start_time.dng"
cancel=true
iter=0
feh --geometry 1000x1000 --reload 1 --scale-down --borderless ../captures/image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyUSB0" -b 115200 & \
echo "Starting Capture Loop." & \
while $cancel; do
    echo "Capturing Image...Please Wait"
    libcamera-still --shutter $shutter --gain $gain --awbgains 1,1 -r -o "../captures/image.jpg" -n --immediate  # Take a new photo
    start_time=$(date +%s)
    echo "Copying Newly Taken Image."
    cp "../captures/image.jpg" "../captures/$start_time.jpg"
    cp "../captures/image.dng" "../captures/$start_time.dng"
    if  [[ $iter -eq 20 ]];then
        iter=0
        input=
        echo "Would you like to solve?(y/n)"
        read -n1 -t 5 input 
        if  [[ "$input" =~ 'y' ]];then
            echo "Solving..."
            solve-field --cpulimit 30  --scale-units arcsecperpix  --scale-low 0.627 --scale-high 0.647 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
            get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
            get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
            echo "Calculating Expected Movement to Target"        
            /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt"
            echo "Would you like to send these coordinates to the mount?"
            input=
            read -n1 -t 5 input 
            if  [[ "$input" =~ 'y' ]];then
                /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
            fi
        fi
    fi
    iter=$iter+1
done