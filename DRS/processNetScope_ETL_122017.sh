#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Please provide a run number"
    exit 1
fi

RED='\033[0;31m'
NC='\033[0m' # No Color

runNum=$1
echo "Processing NetScope data for run ${runNum}"

# ./envHDF5.sh
export HDF5_ROOT_PATH=`pwd`/hdf5
export HDF5_INCLUDE_PATH=${HDF5_ROOT_PATH}/include
export HDF5_LIBRARY_PATH=${HDF5_ROOT_PATH}/lib
export LD_LIBRARY_PATH=${HDF5_LIBRARY_PATH}:$LD_LIBRARY_PATH

echo ""
echo "Converting raw NetScope data to HDF5 format"
echo -e "./convert_ots2hdf5 /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeRaw/RawDataSaver0NetScope_Run${runNum}_0_Raw.dat /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeHDF5/Run${runNum}.hdf5"
./convert_ots2hdf5 /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeRaw/RawDataSaver0NetScope_Run${runNum}_0_Raw.dat /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeHDF5/Run${runNum}.hdf5

echo ""
echo "Converting NetScope HDF5 format to ROOT"
echo -e "./scopehdf2root -ifile=/eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeHDF5/Run${runNum}.hdf5 -ofile=/eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeROOT/Run${runNum}.root"
./scopehdf2root -ifile=/eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeHDF5/Run${runNum}.hdf5 -ofile=/eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/NetScopeROOT/Run${runNum}.root





