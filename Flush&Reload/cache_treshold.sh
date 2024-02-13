#!/bin/bash

max=0
for i in {1..10}
do 
	echo "Iteration $i:"  
	test=$(./cache_timing)
	#echo "$test"
	
	#  while read a single line of input (-r prevents backslashes)
	while read -r line
	do	  
		if [[ $line = *array[3* ]]
		then
			echo "$line"
			number1=$(echo "$line" | grep -o -P '(?<=: ).*(?= CPU)') 
			if [ $number1 -gt $max ]
			then
				max=$number1
			fi
		fi	
		
		if [[ $line = *array[7* ]]
		then
			echo "$line"
		        number2=$(echo "$line" | grep -o -P '(?<=: ).*(?= CPU)') 
			if [ $number2 -gt  $max ]
			then
				max=$number2	
			fi
		fi
	# <<< - here string: give pre-made string to a program
	done <<< "$test"
	
	
done
echo "Threshold: $max"
