#!/bin/bash
#Go to Desktop Directory
cd ~/Desktop/

#Creating and moving into new directory with start time
echo "Setting up and changing into directories."
start_time=$(date +%s)
mkdir $start_time && cd $start_time

#Set up Directory For Capture and Guiding
mkdir ../capture_and_guide && cd ../capture_and_guide
mkdir captures && mkdir solves
cd solves



#!/bin/bash
#M56
#Black Eye Galaxy = RA: 194.18 / DEC: 21.68
#Andromeda = RA_10.6917, DEC_41.2711
#Triangulum Galaxy = RA_23.46825, DEC_30.661
#Deer Lick Group = RA_339.271, 34.41975
#Fireworks Galaxy = 308.7208, DEC_60.1592


#Take the first image
#Try to solve limit 30 seconds
#Go into a loop 
goal_ra=194.18
goal_dec=21.68
bulb_secs=30
gphoto2 --set-config /main/imgsettings/iso=400
#Capture First Image
echo "Capturing Image...Please Wait"
start_time=$(date +%s)
gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s --force-overwrite
echo "Copying Newly Taken Image For Solving."
#Copy Image To Destination
cp "capt0000.jpg" "image.jpg"
mv "capt0000.jpg" "$start_time.jpg"
mv "capt0001.cr2" "../captures/$start_time.cr2"
#Solve Image
solve-field  --cpulimit 30 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "$start_time.jpg"> "$start_time.log"
if [ -e "$start_time.wcs"]
then
    #If Solve was successful, move mount
    get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
    get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
    /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
else
    #Don't Move Mount
    echo "No Capture Was able to be solved, Moving On"
fi
cancel=true
#Display Image, and Start Mount Move Program
feh --geometry 1000x1000 --reload 30 --scale-down --borderless image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyACM0" -b 115200 & \
echo "Starting Capture Loop." & \
while $cancel; do
    echo "Capturing Image...Please Wait"
    start_time=$(date +%s)
    #Capture Image With 30 Second Bulb
    gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s --force-overwrite
    echo "Copying Newly Taken Image For Solving."
    #Copy IMage
    cp "capt0000.jpg" "image.jpg"
    mv "capt0000.jpg" "$start_time.jpg"
    mv "capt0001.cr2" "../captures/$start_time.cr2"
    echo "Solving..."
    #Try To SOlve
    solve-field  --cpulimit 30 --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "$start_time.jpg"> "$start_time.log"
    if [ -e "$start_time.wcs"]
    then
        #If Solved, Move Mount
        echo "Plate has been solved, sending to mount"
        get-wcs "$start_time.wcs" | grep crval1  >> "$start_time.txt"
        get-wcs "$start_time.wcs" | grep crval2  >> "$start_time.txt"
        /home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/calculate_movement.py -r "$goal_ra" -d "$goal_dec" -c "./$start_time.txt" -o "../moves.csv" -w
    else
        echo "No Capture Was able to be solved, Moving On"
    fi
done