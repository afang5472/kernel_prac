#!/bin/bash 

gcc -std=gnu99 --static $1 -o $2 -masm=intel
