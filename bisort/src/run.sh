#!/bin/bash

#grep "$p" feasibility.l$level.$k.out | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g' | tr -d '\n' >> $FEASIBILITY_REPORT
#grep "quad tree build time" T | sed -e 's/[^0-9|.]//g' | sed -e 's/.$//g'

LEVEL_MAX=12
RUNS_NUM=10

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

## declare an array variable
declare -a quad_tree_patterns=("quad tree build time" "quad tree perimeter comp time" "quad tree total time")
declare -a vector_patterns=("comp grid image mapping time" "grid perimeter comp time" "array grid total time")

# run feasibility study with different parameters (level) 
for level in `seq 1 $LEVEL_MAX`;
do
    # run feasibility study several times in order to average the results
    for k in  `seq 1 $RUNS_NUM`;
    do
        echo "level $level" >> feasibility.l$level.$k.out
        ../feasibility $level >> feasibility.l$level.$k.out
    done
done

# write a final post-run feasibility study report
touch $FEASIBILITY_REPORT 

for level in `seq 1 $LEVEL_MAX`;
do
    echo "l$level" >> $FEASIBILITY_REPORT

    for p in "${quad_tree_patterns[@]}"
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
