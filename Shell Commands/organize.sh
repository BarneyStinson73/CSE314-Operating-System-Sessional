#!/bin/bash

visit()
{
	
    if [ -d "$1" ]
	then
	
		for i in "$1"/*
		do
			visit "$i" "$2"
		done
	
	elif [ -f "$1" ]
	then
        #name=${1##*/}
		if [[ "$1" = *.c ]]
        then
            if [ "$verbose" = true ]; then 
                echo "Organizing files of $groll"
            fi
            mkdir -p "$2"/C/$groll
            mv "$1" "$2"/C/$groll/main.c
            if [ "$noexecute" = false ]; then
                if [ "$verbose" = true ]; then 
                    echo "Executing files of $groll"
                fi
                gcc "$2"/C/$groll/main.c -o "$2"/C/"$groll"/a.out
                matched=0
                unmatched=0
                for file in $tests/*
                do
                    name=${file##*/}
                    name=${name%%.txt}
                    name=${name##*t}
                    out="out"
                    out+=$name
                    ans="ans"
                    ans+=$name
                    ./"$2"/C/"$groll"/a.out<"$file">"$2"/C/"$groll"/"$out".txt
                    string=`diff "$2/C/$groll/$out.txt" "$answers/$ans.txt"`
                    if [ -z "$string" ]; then
                        matched=`expr $matched + 1`
                    else
                        unmatched=`expr $unmatched + 1`
                    fi
                done
                echo $groll,C,$matched,$unmatched >> "$2"/result.csv
            fi
        elif [[ "$1" = *.java ]]
        then
            mkdir -p "$2"/Java/$groll
            mv "$1" "$2"/Java/$groll/Main.java
            if [ "$noexecute" = false ]; then
                if [ "$verbose" = true ]; then 
                    echo "Executing files of $groll"
                fi
                javac "$2"/Java/$groll/Main.java
                matched=0
                unmatched=0
                for file in $tests/*
                do
                    name=${file##*/}
                    name=${name%%.txt}
                    name=${name##*t}
                    out="out"
                    out+=$name
                    ans="ans"
                    ans+=$name
                    java -cp "$2/Java/$groll/" Main < "$file" > "$2"/Java/"$groll"/"$out".txt
                    # cat "$file" | java "$2/Java/$groll/" Main > "$2/Java/$groll/$out.txt"
                    string=`diff "$2/Java/$groll/$out.txt" "$answers/$ans.txt"`
                    if [ -z "$string" ]; then
                        matched=`expr $matched + 1`
                    else
                        unmatched=`expr $unmatched + 1`
                    fi
                done
                echo $groll,Java,$matched,$unmatched >> "$2"/result.csv
            fi
            #java -cp "$2"/Java/$groll/Main
        elif [[ "$1" = *.py ]]
        then
            mkdir -p "$2"/Python/$groll
            mv "$1" "$2"/Python/$groll/main.py
            #cat "$file" | python3 "$2"/Python/$groll/main.py
            if [ "$noexecute" = false ]; then
                if [ "$verbose" = true ]; then 
                    echo "Executing files of $groll"
                fi
                matched=0
                unmatched=0
                for file in $tests/*
                do
                    name=${file##*/}
                    name=${name%%.txt}
                    name=${name##*t}
                    out="out"
                    out+=$name
                    ans="ans"
                    ans+=$name
                    #java -cp "$2"/Java/$groll/ Main<"$file"> "$2"/Java/"$groll"/"$out".txt
                    cat "$file" | python3 "$2"/Python/$groll/main.py > "$2/Python/$groll/$out.txt"
                    string=`diff "$2/Python/$groll/$out.txt" "$answers/$ans.txt"`
                    if [ -z "$string" ]; then
                        matched=`expr $matched + 1`
                    else
                        unmatched=`expr $unmatched + 1`
                    fi
                done
                echo $groll,Python,$matched,$unmatched >> "$2"/result.csv
            fi
        fi
	fi
}
verbose="false"
noexecute="false"
if [ $# -lt 4 ] || [ $# -gt 6 ]; then  
    echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
    exit 1
fi
if [ ! -d $1 ]; then
    echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
    exit 1
fi
if [ ! -d $3 ]; then
    echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
    exit 1
fi
if [ ! -d $4 ]; then
    echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
    exit 1
fi
if [ $# -eq 5 ]; then
    if [ $5 = -v ]; then verbose="true"
    elif [ $5 = -noexecute ]; then noexecute="true"
    else 
        echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
        exit 1
    fi
fi

if [ $# -eq 6 ]; then
    verbose="true"
    noexecute="true"
    if [ $5 != -v ] && [ $5 != -noexecute ]; then 
        echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
        exit 1
    fi
    if [ $6 != -v ] && [ $6 != -noexecute ]; then 
        echo "Usage: ./organize.sh <submission_directory> <target_directory> <tests_directory> <answers_directory> OPTIONAL: [-v(verbose) -noexecute]"
        exit 1
    fi
fi

groll=0
rm -rf "$2"
mkdir -p "$2"/C
mkdir -p "$2"/Java
mkdir -p "$2"/Python
if [ "$noexecute" = "false" ]; then
    touch "$2"/result.csv
    echo "roll,type,matched,unmatched" > "$2"/result.csv    
fi
answers=$4
tests=$3
for file in "$1"/*.zip
do
    roll=${file##*_}
    roll=${roll%%.zip}
    mkdir -p unzipper/"$roll"
    unzip -q "$file" -d  unzipper/"$roll"
done


for file in unzipper/*
do
    groll=${file##*/}
    visit "$file" "$2"
done

rm -rf unzipper

# for file in "$2"/C/*/*.c
# do
#     echo "$file"
#     groll=${file##*/}
#     gcc "$file" -o "$2"/C/"$groll"/'a.out'
# done



























# for file in unzipper/*
# do
    
#     if [ -d "$file"/"$file"/*.c ];
#     then
#         mkdir -p "$2"/C/"$file"
#         mv "$file"/"$file"/*.c "$2"/C/"$file"/main.c
#     elif [ -d "$file"/"$file"/*.java ];
#     then
#         mkdir -p "$2"/Java/"$file"
#         mv "$file"/"$file"/*.java "$2"/Java/"$file"/Main.java
#     else
#         mkdir -p "$2"/Python/"$file"
#         mv "$file"/"$file"/*.py "$2"/Python/"$file"/main.py
#     fi
# done