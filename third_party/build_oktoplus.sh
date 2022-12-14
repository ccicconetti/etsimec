#!/bin/bash

if [ "$CONCURRENCY" == "" ] ; then
  CONCURRENCY=4
fi
if [ "$CXX_COMPILER" == "" ] ; then
  CXX_COMPILER=g++
fi

echo "concurrency level: $CONCURRENCY"
echo "C++ compiler:      $CXX_COMPILER"

TMPFILENAME=/tmp/build.$$.cc
cat > $TMPFILENAME << EOF
int main() {}
EOF
$CXX_COMPILER -std=c++20 $TMPFILENAME -o /dev/null 2> /dev/null
RET=$?
rm -f $TMPFILENAME
if [ $RET -ne 0 ] ; then
  echo "The C++ compiler does not support c++20 dialect: $CXX_COMPILER"
  exit 1
fi

if [ ! -d oktoplus ] ; then
  echo "**********************************************************************"
  echo "BUILDING OKTOPLUS"
  if [ "$(dpkg -l | grep jsoncpp-dev)" == "" ] ; then
    sudo apt install -y libjsoncpp-dev
    if [ $? -ne 0 ] ; then
      echo "Could not install the required dependency libjsoncpp-dev, bailing out"
      exit 1
    fi
  fi
  git clone https://github.com/kalman5/oktoplus
  pushd oktoplus
  git checkout a15096ca2fb5d1f7b4f0f4817195029a245c2c3e
  git submodule update --init --recursive
  patch -p1 < ../patch_oktoplus
  mkdir _build/
  pushd _build/
  if [ $? -ne 0 ] ; then
    echo "Could not patch oktoplus, bailing out"
    exit 1
  fi
  cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_COMPILER=$CXX_COMPILER \
    -DCMAKE_BUILD_TYPE=optimized \
    .. >& build.log
  make -j $CONCURRENCY
  RET=$?
  popd
  popd

  if [ $RET -ne 0 ] ; then
    echo "build failed, see oktoplus/_build/build.log for details"
    exit 1
  fi
fi
