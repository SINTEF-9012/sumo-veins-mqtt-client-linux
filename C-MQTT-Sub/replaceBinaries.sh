#! /bin/sh


AHPATH="/userdata/apps/MQTT/Sub"
#AHPATH="/home/volvo/Desktop/Sub"

sudo mkdir $(pwd)/build
cd $(pwd)/build
sudo cmake ..
sudo make

rm -r $AHPATH/backup
mkdir $AHPATH/backup
cp $AHPATH/* $AHPATH/backup

rm $AHPATH/C_MQTT_Sub
cp C_MQTT_Sub $AHPATH