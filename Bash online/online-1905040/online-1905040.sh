#!/bin/bash
input="input_dir"
output="output_dir"
visit()
{
	
    if [ -d "$1" ]
	then
	
		for i in "$1"/*
		do
			visit "$i" 
		done
	
	elif [ -f "$1" ]
	then
        name=${1##*/}
        length=${#name}
        mkdir -p $output/$length/$name
        cp "$1" "$output/$length/$name"
    fi
                
}
sort_try()
{
	
    if [ -d "$1" ]
	then
	
		for i in "$1"/*
		do
			visit "$i" 
		done
	
	elif [ -f "$1" ]
	then
        ls -S
    fi
                
}
visit "A2"/$input
# cd "$output"
# for file in `ls`
# do
#     #echo "$file"
# done