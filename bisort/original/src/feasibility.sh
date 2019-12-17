#!/bin/sh
echo Olden bitonic sorting feasibility run
for i in {1..32};
do
   ./feasibility $i >> OUTPUT
done

