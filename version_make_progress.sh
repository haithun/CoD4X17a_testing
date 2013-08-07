#!/bin/bash
# increments the real number held in version.h by 0.01

read line <version.h
line=$(echo $line + 1 | bc)
echo $line >version.h