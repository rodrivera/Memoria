#!/bin/bash
AUX=""
#AUX="../"

#city="Oldenburg"
city="$1"
network="$AUX""data/""$city""/nodes.txt ""$AUX""data/""$city""/edges.txt"

if [ ! -f "$AUX""data/""$city""/nodes.txt" ]; then
    echo "Queries file not found!"
    echo "Usage: testAllComplete.sh cityname"
    echo "with perc values between 0 and 100"
    exit
fi

declare -a SUFFIXES=("5000")
for N in "${SUFFIXES[@]}"
do
	trajectories="$AUX""data/""$city""/trajectories/""$N"".dat"
	
	echo "----- Starting tests for ""$N"" objects.. -----"

	runFNR="./""$AUX""fnrtest.out ""$network"" ""$trajectories"
	runX="./""$AUX""xtest.out"" ""$network"" ""$trajectories"
	runC="./""$AUX""ctest.out"" ""$network"" ""$trajectories"

	declare -a QTYPES=("1_1_1" "10_10_10" "20_20_20" "1_1_10" "1_1_100" "10_10_100" "1_1_0" "10_10_0" "100_100_0")
	for QT in "${QTYPES[@]}"
	do
		queries="$AUX""data/""$city""/queries/""$QT"".txt"
		outFNR="$AUX""data/""$city""/output/FNR/""$QT""-""$N"".txt"
		outX="$AUX""data/""$city""/output/X/""$QT""-""$N"".txt"
		outC="$AUX""data/""$city""/output/C/""$QT""-""$N"".txt"

		runFNR="$runFNR"" ""$queries"" ""$outFNR"
		runX="$runX"" ""$queries"" ""$outX"
		runC="$runC"" ""$queries"" ""$outC"
	done

	eval "time ""$runFNR"
	echo ""
	eval "time ""$runX"
	echo ""
	eval "time ""$runC"
	echo ""
done
