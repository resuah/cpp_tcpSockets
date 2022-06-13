#!/bin/bash

rm -rf "build" "install"

source_dir=$(pwd);
#echo "$source_dir"

mkdir -p "build"  # "install"
pushd "build"

cmake -DCMAKE_INSTALL_PREFIX="${source_dir}/install" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=/bin/x86_64-linux-gnu-g++-9 \
      $source_dir 
make
#make install
popd


