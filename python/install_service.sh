# based on http://blog.scphillips.com/posts/2013/07/getting-a-python-script-to-run-in-the-background-as-a-service-on-boot/
sudo cp car_service.sh /etc/init.d/car_service.sh
sudo chmod +xX /etc/init.d/car_service.sh
sudo update-rc.d car_service.sh defaults

