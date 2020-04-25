#/bin/bash

sudo apt-get update -y && 
sudo apt-get upgrade -y && 
sudo apt-get install build-essential software-properties-common -y && 
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y && 
sudo apt-get update -y && 
sudo apt-get install gcc-8 g++-8 -y && 
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 && 
sudo update-alternatives --config gcc

sudo apt purge -y --auto-remove cmake &&
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add - &&
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ `lsb_release -cs` main" &&
sudo apt update -y && sudo apt install -y cmake

sudo apt install -y ninja-build
