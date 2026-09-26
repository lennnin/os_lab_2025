#!/bin/bash
sr=0;


for a in $@; do
    b=${#};

    sr=$((sr + a / b)) ;

done
echo ${#}
echo $b
echo $sr
