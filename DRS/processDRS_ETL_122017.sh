#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Please provide a run number"
    exit 1
fi

RED='\033[0;31m'
NC='\033[0m' # No Color

runNum=$1
echo "Processing DRS/Pixel data for run ${runNum}"

nfiles=$(ls /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ/CMSTiming/RawData*Run${runNum}_*.dat | wc -l)
if [ $nfiles -gt 1 ]
then
    echo "Combining .dat files"
    if [ -e /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat ]
    then
	echo -e "${RED}Combined .dat file already exists.${NC} Delete it first if you would like to recombine."
	echo "Filename: /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat"
    else
	cat $(ls -v /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/RawDataSaver0CMSVMETiming_Run${runNum}_*.dat) > /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat
	echo "Created /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat"
    fi
fi

if [ $nfiles -gt 1 ]
then 
    echo -e "./dat2rootPixels /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/Run${runNum}_CMSTiming_converted.root /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerCount__${runNum}.cnt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerTimingRun${runNum}_station5.txt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//DRSPixelMerged/Run${runNum}.root -1 --config=config/December2017_LGADOnly.config"
    ./dat2rootPixels /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/combined_dat/RawDataSaver0CMSVMETiming_Run${runNum}_combined.dat /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/Run${runNum}_CMSTiming_converted.root /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerCount__${runNum}.cnt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerTimingRun${runNum}_station5.txt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//DRSPixelMerged/Run${runNum}.root -1 --config=config/December2017_LGADOnly.config
else
    inputFile=$(ls /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/RawDataSa\
ver0CMSVMETiming_Run${runNum}_*.dat)
    echo -e "./dat2rootPixels ${inputFile} /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/Run${runNum}_CMSTiming_converted.root /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerCount__${runNum}.cnt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerTimingRun${runNum}_station5.txt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//DRSPixelMerged/Run${runNum}.root -1 --config=config/December2017_LGADOnly.config"
    ./dat2rootPixels ${inputFile} /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//CMSTiming/Run${runNum}_CMSTiming_converted.root /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerCount__${runNum}.cnt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//NimPlus/TriggerTimingRun${runNum}_station5.txt /eos/uscms/store/user/cmstestbeam/ETL/MT6Section1Data/122017/OTSDAQ//DRSPixelMerged/Run${runNum}.root -1 --config=config/December2017_LGADOnly.config
fi





