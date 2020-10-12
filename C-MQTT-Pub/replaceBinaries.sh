#! /bin/sh


AHPATH="/userdata/apps/MQTT/Pub"
#AHPATH="/home/volvo/Desktop/Pub"

sudo mkdir $(pwd)/build
cd $(pwd)/build
sudo cmake ..
sudo make

rm -r $AHPATH/backup
mkdir $AHPATH/backup
cp $AHPATH/* $AHPATH/backup

rm $AHPATH/C_MQTT_Pub
cp C_MQTT_Pub $AHPATH