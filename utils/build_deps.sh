#!/bin/bash

if [ -z $CONCURRENCY ] ; then
  CONCURRENCY=4
fi

distro=$(lsb_release -sc 2> /dev/null)

if [[ "$distro" != "bionic" && "$distro" != "focal" ]] ; then
  echo "Error: script only tested on Ubuntu 18.04 (Bionic) and 20.04 (Focal)"
  exit 1
fi

echo "**********************************************************************"
echo "INSTALLING SYSTEM PACKAGES"

apt update
apt install -y \
  libgoogle-glog-dev libboost-all-dev \
  g++ build-essential autoconf libtool pkg-config \
  libssl-dev wget \
  ninja-build

echo "**********************************************************************"
echo "INSTALLING CMAKE"

if [[ "$(which cmake)" != "" && "$(cmake --version | head -n 1)" == "cmake version 3.22.2" ]] ; then
  echo "it seems it is already installed, skipping"
else
  wget -q -O cmake-linux.sh https://github.com/Kitware/CMake/releases/download/v3.22.2/cmake-3.22.2-linux-x86_64.sh
  sh cmake-linux.sh -- --skip-license --prefix=/usr
  rm cmake-linux.sh

  if [ "$(cmake --version | head -n 1)" != "cmake version 3.22.2" ] ; then
    echo "installation failed"
    exit 1
  fi
fi

echo "**********************************************************************"
echo "INSTALLING GRPC"

if [ -r /usr/local/lib/libgrpc++.so.1.44.0 ] ; then
  echo "it seems it is already installed, skipping"
else
  git clone https://github.com/grpc/grpc
  pushd grpc
  git checkout v1.44.0
  git submodule update --init --recursive
  mkdir -p "cmake/build"
  pushd "cmake/build"
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DgRPC_SSL_PROVIDER=package \
    ../..
  make -j$CONCURRENCY install
  popd
  popd

  if [ ! -r /usr/local/lib/libgrpc++.so.1.44.0 ] ; then
    echo "installation failed"
    exit 1
  fi
fi

echo "**********************************************************************"
echo "INSTALLING C++ REST SDK"

if [ -r /usr/local/lib/libcpprest.so.2.10 ] ; then
  echo "it seems it is already installed, skipping"
else
  git clone https://github.com/Microsoft/cpprestsdk.git
  pushd cpprestsdk
  if [ "$distro" == "bionic" ] ; then
    git checkout v2.10.14
  elif [ "$distro" == "focal" ] ; then
    git checkout v2.10.18
  else
    echo "Invalid distro: $distro"
    exit -1
  fi
  git submodule update --init --recursive
  mkdir build
  pushd build
  cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=1
  ninja -j$CONCURRENCY
  ninja install
  popd
  popd

  if [ ! -r /usr/local/lib/libcpprest.so.2.10 ] ; then
    echo "installation failed"
    exit 1
  fi
fi

ldconfig

