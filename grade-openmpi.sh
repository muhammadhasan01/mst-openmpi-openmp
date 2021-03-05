#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: ./autograde.sh <nim> <openmpi_output_directory>"
    exit
fi

rm -rf $2/*

rm -rf ./tmp
mkdir ./tmp
mkdir ./tmp/testcases

if ! python3 ./python_script/get_testcase.py $1; then
    exit
fi

testcases_names=()

readarray -d '' testcases_names < <(find ./tmp/testcases -type f -print0)

for tc in ${testcases_names[@]}; do
    ./openmpi-script.sh $tc
done

if ! python3 ./python_script/check_openmpi.py $1 $2; then
    exit
fi

zip -r $1.zip .

if ! python3 ./python_script/submit_file.py $1 $1.zip; then
    exit
fi
echo "Thank you for using our service!"
