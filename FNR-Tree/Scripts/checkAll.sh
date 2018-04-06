#!/bin/bash
AUX=""
#AUX="../"

#city="Oldenburg"
city="$1"

declare -a SUFFIXES=("1000" "2000" "3000" "4000" "5000")
for N in "${SUFFIXES[@]}"
do
	declare -a QTYPES=("1_1_1" "10_10_10" "20_20_20" "1_1_10" "1_1_100" "10_10_100" "1_1_0" "10_10_0" "100_100_0")
	for QT in "${QTYPES[@]}"
	do
		outFNR="$AUX""data/""$city""/output/FNR/""$QT""-""$N"".txt"
		outX="$AUX""data/""$city""/output/X/""$QT""-""$N"".txt"
		outC="$AUX""data/""$city""/output/C/""$QT""-""$N"".txt"

		echo "Checking differences between Output files.. ""$QT""-""$N"
		if diff $outFNR $outX >/dev/null; then
			printf "Passed (FNR and X)\n"
		else
			printf "¡¡ Failed (FNR or X) !!\n"
			printf "   > # differences: "
			diff -U 0 $outFNR $outX | grep ^@ | wc -l
			sdiff $outFNR $outX  > _DIFF_LOG_FNR-X_-"$QT"-"$N"-_.txt
			printf "See _DIFF_LOG_C-X_""$Q""-""$N"".txt for detailed diff\n"
			#exit
		fi
		if diff $outC $outX >/dev/null; then
			printf "Passed (C)\n"
		else
			printf "¡¡ Failed (C) !!\n"
			printf "   > # differences: "
			diff -U 0 $outC $outX | grep ^@ | wc -l
			sdiff $outC $outX > _DIFF_LOG_C-X_-"$QT"-"$N"-_.txt
			printf "See _DIFF_LOG_C-X_""$Q""-""$N"".txt for detailed diff\n"
			#exit
		fi		
		echo "-------------------------------------------";
	done
done
