#!/bin/sh
tc_path=bin/tc
test_dir=tests
result_dirpath=results

for file in ${test_dir}/*
do
    mplfile=${file##${test_dir}/}
    resultfile=result_${mplfile%.*}.txt
    ./$tc_path $file > $result_dirpath/$resultfile
done
