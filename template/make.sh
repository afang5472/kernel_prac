#!/bin/bash 

gcc --static $1 -o $2 -masm=intel
