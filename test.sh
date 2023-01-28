#!/bin/bash

#set umask to allow 0600
umask 000

#run server in the background
./server &

#sleep to allow server to set up
sleep 2

lines_counter=0

while read input; do
  ./client  <<< "$input" >> 'output.txt' &
  lines_counter=$((lines_counter + 1))
done <$1 

sleep 20

valid_counter=0
while read input; do
  echo -e "\e[34mLooking: $input\e[0m"
  res=$(grep "$input" 'output.txt')
  echo "$res"
  count=$(echo "$res" | wc -l)

  if [ $? -eq 0 ]; then
    valid_counter=$((valid_counter + count))
  fi
done <$2 


echo -e "\e[35m\nRESULTS: \e[0m"
if [ $valid_counter -eq $lines_counter ]; then
  echo -e "\e[32mTEST RESULTS ARE VALID\e[0m"
else
  echo -e "\e[31mTEST RESULTS ARE INVALID\e[0m"
fi

rm -f 'output.txt'