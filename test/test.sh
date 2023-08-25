#!/usr/bin/env bash

for cmd in gcc clang; do
	which ${cmd} > /dev/null 2>&1
	if [ "$?" -ne 0 ]; then
		echo "dependency command '${cmd}' missing, aborting."
		exit 1
	fi
done

if [ $(basename `pwd`) != "test" ]; then
	echo "please run from /test directory, aborting."
	exit 1
fi

compilers=("gcc" "clang")
tests=("zero" "readme" "warn" "assert")

for compiler in ${compilers[@]}; do
	for test in ${tests[@]}; do
		echo "${compiler}: test case '${test}'"
		"${compiler}" -I.. "${test}.c" -o mint
		./mint > mint.txt
		diff mint.txt "${test}.txt"
	done
done

rm -f mint mint.txt
