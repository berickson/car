mkdir /dev/car
mkfifo /dev/car/command
chmod o+w /dev/car/command
mkdir /var/log/car
chmod o+rw /var/log/car
python /home/pi/car/python/car_service.py
