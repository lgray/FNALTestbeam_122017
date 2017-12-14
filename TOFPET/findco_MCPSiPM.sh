#declare -a arr=("23" "19" "26" "14" "7" "9" "32" "63" "56" "34" "33" "43" "41")
#declare -a arr=("20" "31" "23" "19" "26" "14" "7" "9" "32" "63" "56" "34" "33" "43" "41")
#declare -a arr=("20" "31" "23" "19" "26" "14" "7" "9" "32" "40" "41" "56" "34" "33" "43" "41")
#declare -a arr=("31" "23" "19" "26" "14" "7" "9" "32" "40" "41" "56" "34" "33" "43" "41")
#declare -a arr=("23" "19" "26" "14" "7" "9" "32" "56" "34" "33" "43")
declare -a arr=("43" "63")
newline=TOFPET_Test_12032017_Run175
ch1=41
for i in "${arr[@]}"
do
   echo "$i"
   sed -i -e "s/chID/$i/g" petsys_tables/febd_gbe_trigger_map.tsv
   #./run_convert_coi.sh 1
   echo "./convert_raw_to_coincidence --config config.ini -i /home/daq/sw_daq_tofpet2/data/${newline} -o data/coincidences --writeBinary"
   ./convert_raw_to_coincidence --config config.ini -i /home/daq/sw_daq_tofpet2/data/${newline} -o data/coincidences --writeBinary
   echo "./convert_raw_to_coincidence --config config.ini -i /home/daq/sw_daq_tofpet2/data/${newline} -o data/coincidences.root --writeRoot"
   ./convert_raw_to_coincidence --config config.ini -i /home/daq/sw_daq_tofpet2/data/${newline} -o data/coincidences.root --writeRoot
   sed -i -e "s/$i/chID/g" petsys_tables/febd_gbe_trigger_map.tsv
   root -l psDrawCTR.C+'("data/coincidences")' -b
   rm -rf data/${newline}_${ch1}_${i}
   mkdir data/${newline}_${ch1}_${i}
   mv data/coincidences* data/${newline}_${ch1}_${i}
   echo "$i"
done
