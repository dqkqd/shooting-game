sudo apt-get update

# g++-9
sudo apt install g++-9 -y

#Remove the previous alternatives
sudo update-alternatives --remove-all gcc
sudo update-alternatives --remove-all g++

#Define the compiler
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 30
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 30

sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
sudo update-alternatives --set cc /usr/bin/gcc

sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
sudo update-alternatives --set c++ /usr/bin/g++

#Confirm and update (You can use the default setting)
sudo update-alternatives --config gcc
sudo update-alternatives --config g++

# upgrade cmake
pip install cmake --upgrade
