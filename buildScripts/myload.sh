# script to automate loading the image onto an sd card
# I'm not sure if the username needs to be there
# I just added the -p flag for now

sudo mkdir -p /media/michael/sd &&
sudo mount -o umask=000 /dev/mmcblk0p1 /media/michael/sd &&
cp images/* /media/michael/sd &&
sudo umount /media/michael/sd &&
sudo rmdir /media/michael/sd

