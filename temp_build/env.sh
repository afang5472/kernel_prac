#!/bin/bash 

#basic env sets
export KERNENV_DIR="/home/afang/sysinternal/kernel_env/"
export BUSYBOX_DIR=$KERNENV_DIR"busybox-1.29.3/"
export KERNSRC_DIR=$KERNENV_DIR"linux-3.7.1/"

#appending env sets
export BUSYBOX_INSTALL=$BUSYBOX_DIR"temp_install/"

#mods and exploits directories

export KERNPRAC_DIR="/home/afang/sysinternal/kernel_prac/"
export KERNPRAC_TESTS=$KERNPRAC_DIR"temp_build/temp"
