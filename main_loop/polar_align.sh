mkdir -p ./Capture/jpg
mkdir -p ./Capture/cr2
#gphoto2 --set-config shutterspeed=bulb
while true; do
	start_time=$(date +%s)
	gphoto2 --set-config bulb=1 --wait-event=30s --set-config bulb=0 --wait-event-and-download=2s	
	echo "Copying Newly Taken Image."
	mv "capt0000.jpg" "./Capture/jpg/$start_time.jpg"
	mv "capt0001.cr2" "./Capture/cr2/$start_time.cr2"
	echo "Finished Capture and Move, Sleeping 2Seconds"
	sleep 2s
	echo "Capturing New Image"
done
