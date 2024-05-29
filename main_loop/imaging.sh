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

# mkdir -p ./Capture/jpg
# mkdir -p ./Capture/cr2
# #gphoto2 --set-config shutterspeed=bulb
# while true; do
#         start_time=$(date +%s)
#         gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s
#         echo "Copying Newly Taken Image."
#         mv "capt0000.jpg" "./Capture/jpg/$start_time.jpg"
#         mv "capt0001.cr2" "./Capture/cr2/$start_time.cr2"
#         echo "Finished Capture and Move, Sleeping 2Seconds"
#         sleep 2s
#         echo "Capturing New Image"
# done



#!/bin/bash
#M56
#Black Eye Galaxy = RA: 194.18 / DEC: 21.68
#Andromeda = RA_10.6917, DEC_41.2711
#Triangulum Galaxy = RA_23.46825, DEC_30.661
#Deer Lick Group = RA_339.271, 34.41975
#Fireworks Galaxy = 308.7208, DEC_60.1592

goal_ra=194.18
goal_dec=21.68
bulb_secs=30
gphoto2 --set-config /main/imgsettings/iso=400
#Capture First Image, Solve
echo "Capturing Image...Please Wait"
start_time=$(date +%s)
gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s --force-overwrite
echo "Copying Newly Taken Image For Solving."
cp "capt0000.jpg" "../captures/image.jpg"
mv "capt0000.jpg" "../captures/$start_time.jpg"
mv "capt0001.cr2" "../captures/$start_time.cr2"
cancel=true
iter=0
feh --geometry 1000x1000 --reload 1 --scale-down --borderless ../captures/image.jpg & \
/home/millerad/Desktop/Dagerro/venv/bin/python /home/millerad/Desktop/Dagerro/main_loop/move_mount.py -f "../moves.csv" -s "/dev/ttyACM0" -b 115200 & \
echo "Starting Capture Loop." & \
while $cancel; do
    echo "Capturing Image...Please Wait"
    start_time=$(date +%s)
    gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s --force-overwrite
    echo "Copying Newly Taken Image For Solving."
    mv "capt0000.jpg" "../captures/$start_time.jpg"
    mv "capt0001.cr2" "../captures/$start_time.cr2"
    if  [[ $iter -eq 2 ]];then
        iter=0
        input=
        echo "Would you like to solve?(y/n)"
        read -n1 -t 5 input 
        if  [[ "$input" =~ 'y' ]];then
            echo "Solving..."
            solve-field --downsample 4 --match none --new-fits none --rdls none --index-xyls none -p --corr none --solved none --temp-axy -O --wcs "$start_time.wcs" "../captures/$start_time.jpg"> "$start_time.log"
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