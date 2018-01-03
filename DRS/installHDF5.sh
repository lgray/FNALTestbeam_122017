#!/bin/sh

source envHDF5.sh
wget https://support.hdfgroup.org/ftp/HDF5/current18/src/hdf5-1.8.20.tar.bz2
tar -xvjf hdf5-1.8.20.tar.bz2
pushd hdf5-1.8.20
./configure --prefix=`pwd`/../hdf5
make 
make check
make install
make check install
popd

