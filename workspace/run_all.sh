#!/bin/bash
echo -n "Running si2_r001 "
date
./run_tests.sh data/mivia/si2/rand/r001/ >> results_si2_r001.txt

echo -n "Running si2_r005 "
date
./run_tests.sh data/mivia/si2/rand/r005/ >> results_si2_r005.txt

echo -n "Running si2_r01 "
date
./run_tests.sh data/mivia/si2/rand/r01/ >> results_si2_r01.txt


echo -n "Running si4_r001 "
date
./run_tests.sh data/mivia/si4/rand/r001/ >> results_si4_r001.txt

echo -n "Running si4_r005 "
date
./run_tests.sh data/mivia/si4/rand/r005/ >> results_si4_r005.txt

echo -n "Running si4_r01 "
date
./run_tests.sh data/mivia/si4/rand/r01/ >> results_si4_r01.txt


echo -n "Running si6_r001 "
date
./run_tests.sh data/mivia/si6/rand/r001/ >> results_si6_r001.txt

echo -n "Running si6_r005 "
date
./run_tests.sh data/mivia/si6/rand/r005/ >> results_si6_r005.txt

echo -n "Running si6_r01 "
date
./run_tests.sh data/mivia/si6/rand/r01/ >> results_si6_r01.txt
