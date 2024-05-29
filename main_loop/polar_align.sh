cd ~/Desktop
mkdir polar && cd polar
start_time=$(date +%s)
gphoto2 --set-config /main/imgsettings/iso=800
gphoto2 --set-config bulb=1 --wait-event=15s --set-config bulb=0 --wait-event-and-download=1s	
cp capt0000.jpg image.jpg
mv "capt0000.jpg" "$start_time.jpg"
feh --geometry 1000x1000 --reload 30 --scale-down --borderless image.jpg & \
while true; do
	echo "Capturing New Image"
	start_time=$(date +%s)
	gphoto2 --set-config bulb=1 --wait-event=15s --set-config bulb=0 --wait-event-and-download=1s	
	echo "Copying Newly Taken Image."
	cp capt0000.jpg image.jpg
	mv "capt0000.jpg" "$start_time.jpg"
	echo "Finished Capture and Move, Sleeping 2 Seconds"
	sleep 2s
done
