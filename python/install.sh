sudo mkdir /dev/car
sudo mkfifo /dev/car/command
sudo chmod o+w /dev/car/command
sudo mkdir /var/log/car
sudo chmod o+rw /var/log/car
