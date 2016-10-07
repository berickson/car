# install instructions based on 
#    http://docs.opencv.org/3.1.0/d7/d9f/tutorial_linux_install.html#gsc.tab=0CD

# I originally ran this as root and it doesn't work, force not to run as root
if [ "$USER" = "root" ]
then 
  echo "don't run this as root"
  exit
else
  echo "running as $USER"
fi

sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev python2.7-dev
cd ~
sudo rm -rf opencv
sudo rm -rf opencv_contrib
git clone https://github.com/Itseez/opencv.git
git clone https://github.com/Itseez/opencv_contrib.git

cd ~/opencv
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules ..

make -j7 # runs 7 jobs in parallel

sudo make install
