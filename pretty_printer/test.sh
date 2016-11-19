#!/bin/sh
bin_path=bin/pp
test_dir=tests
result_dirpath=results
files=()
dir=()

for filepath in ${test_dir}/*; do
  if [ -f $filepath ] ; then
    files+=("$filepath")
  elif [ -d $filepath ] ; then
    dir+=("$filepath")
  fi
done

for file in ${files[@]}; do
  mplfile=${file##${test_dir}/}
  resultfile=result_${mplfile%.*}.txt
  ./$bin_path $file > $result_dirpath/$resultfile
done

for path in ${dir[@]}; do
  mkdir -p $result_dirpath/${path##${test_dir}/}
  for file in $path/*; do
    mplfile=${file##${path}/}
    resultfile=result_${mplfile%.*}.txt
    ./$bin_path $file > $result_dirpath/${path##${test_dir}/}/$resultfile
  done
done

# for file in ${test_dir}/*
# do
#     mplfile=${file##${test_dir}/}
#     resultfile=result_${mplfile%.*}.txt
#     ./$bin_path $file > $result_dirpath/$resultfile
# done
#
# for file in ${test_dir}/*
# do
#     mplfile=${file##${test_dir}/}
#     resultfile=result_${mplfile%.*}.txt
#     ./$bin_path $file > $result_dirpath/$resultfile
# done
