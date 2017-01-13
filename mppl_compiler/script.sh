#!/bin/sh
bin_path=bin/mpplc
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
  resultfile=${mplfile%.*}.csl
  ./$bin_path $file -o $result_dirpath/$resultfile
done

for path in ${dir[@]}; do
  mkdir -p $result_dirpath/${path##${test_dir}/}
  for file in $path/*; do
    mplfile=${file##${path}/}
    resultfile=${mplfile%.*}.csl
    # echo $result_dirpath/${path##${test_dir}/}/$resultfile
    ./$bin_path -o $result_dirpath/${path##${test_dir}/}/$resultfile $file
  done
done
