#!/bin/bash

BASH_VERSION=$(bash --version | grep "GNU bash" | grep "version" | sed -E 's/.*version ([0-9|.]*)(.*)/\1/g')
MAJOR_VERSION=$(echo $BASH_VERSION | sed -E 's/([0-9]*)(.*)/\1/g')
if [ $MAJOR_VERSION -lt 4 ] 
    then
        echo "Bash version is $BASH_VERSION"
        echo "Major bash version must be at least 4!"
        exit
fi

#grep "$p" feasibility.l$level.$k.out | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g' | tr -d '\n' >> $FEASIBILITY_REPORT
#grep "quad tree build time" T | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g'

# size of a sequence to sort 2^SIZE
SIZE_MAX=5
# number of runs to average the running time
RUNS_NUM=5

SCRIPT_DIR="${PWD}"
TEMP_DIR="${SCRIPT_DIR}/temp"
FEASIBILITY_REPORT="${SCRIPT_DIR}/feasibility.report.excel"

if [[ -e ${TEMP_DIR} ]]; then
    rm -rf ${TEMP_DIR}
    echo "Deleted ${TEMP_DIR}"
fi

if [ -f ${FEASIBILITY_REPORT} ]; then
    rm -rf ${FEASIBILITY_REPORT}
    echo "Deleted ${FEASIBILITY_REPORT}"
fi

mkdir ${TEMP_DIR}
cd ${TEMP_DIR}

# declare an array of patterns to search
declare -a patterns=("reference" "std::sort" "sequential" "parallel")
# declare an array of averaged per pattern running times
declare -a arr_times
# declare an array of running times
declare -a arr_runs
# declare report dictionary
declare -A report

# run feasibility study with different parameters (level) 
for size in `seq 1 $SIZE_MAX`;
do
    # run feasibility study several times in order to average the results
    for k in  `seq 1 $RUNS_NUM`;
    do
        echo "size $size" >> feasibility.l$size.$k.out
        ../feasibility $level >> feasibility.l$size.$k.out
    done
done

# write a final post-run feasibility study report
touch $FEASIBILITY_REPORT 

for size in `seq 1 $SIZE_MAX`;
do
    echo "$size"
    report[$size]=array
done

exit

for size in `seq 1 $SIZE_MAX`;
do
    report[$size]=array 
    
    for p in "${patterns[@]}"
    do
        i=0
        for k in  `seq 1 $RUNS_NUM`;
        do
            array[$k]=$(grep "$p" feasibility.l$level.$k.out | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g' | tr -d '\n')

            
            if [[ -f $file ]]; then
                array[$i]=$file
                i=$(($i+1))
            fi
        done

    done
done




for size in `seq 1 $SIZE_MAX`;
do
    echo "$size" >> $FEASIBILITY_REPORT
    report[$size]

    for p in "${patterns[@]}"
    do
        echo "$p:" | tr -d '\n' >> $FEASIBILITY_REPORT
        for k in  `seq 1 $RUNS_NUM`;
        do
            grep "$p" feasibility.l$level.$k.out | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g' | tr -d '\n' >> $FEASIBILITY_REPORT
            echo ":" | tr -d '\n' >> $FEASIBILITY_REPORT
        done
        echo "" >> $FEASIBILITY_REPORT
    done

    for p in "${vector_patterns[@]}"
    do
        echo "$p:" | tr -d '\n' >> $FEASIBILITY_REPORT
        for k in  `seq 1 $RUNS_NUM`;
        do
            grep "$p" feasibility.l$level.$k.out | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g' | tr -d '\n' >> $FEASIBILITY_REPORT
            echo ":" | tr -d '\n' >> $FEASIBILITY_REPORT
        done
        echo "" >> $FEASIBILITY_REPORT
    done
        
    echo "" >> $FEASIBILITY_REPORT
done


    
# Total numbers 
n=5 
# copying the value of n 
m=$n 
   
# initialized sum by 0 
sum=0 
   
# array initialized with 
# some numbers 
array=(1 2 3 4 5) 
   
# loop until n is greater 
# than 0 
while [ $n -gt 0 ] 
do
    # copy element in a  
    # temp variable 
    num=${array[`expr $n - 1`]}     
                   
                    # add them to sum 
                        sum=`expr $sum + $num` 
                           
                            # decrement count of n 
                                n=`expr $n - 1` 
                            done 
                               
                            # displaying the average  
                            # by piping with bc command 
                            # bc is bash calculator 
                            # command 
                            avg=`echo "$sum / $m" | bc -l` 
                            printf '%0.3f' "$avg"



