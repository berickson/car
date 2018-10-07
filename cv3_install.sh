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

if [ ! -d ~/opencv ] 
then
  git clone https://github.com/Itseez/opencv.git
fi


if [ ! -d ~/opencv_contrib ] 
then
  git clone https://github.com/Itseez/opencv_contrib.git
fi

cd ~/opencv_contrib
git pull
git checkout 3.4.3
cd ~/opencv
git pull
git checkout 3.4.3
mkdir build
cd build
#CC=/usr/bin/gcc-5 
#CXX=/usr/bin/g++-5
cmake  -D ENABLE_PRECOMPILED_HEADERS=OFF  -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local cmake -D CMAKE_BUILD_TYPE=Release -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules ..
make clean
make -j8 # runs n jobs in parallel

sudo make install
