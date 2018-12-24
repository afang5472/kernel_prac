#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Auth0r : afang
# nice day mua! :P
# desc:
'''
complete file uploading or automatic interactive procedure
by co-operating with pwntools & pesudo tty
'''
#lambs:
wait = lambda x: raw_input(x)

# imports
from pwn import *
p = ""
payload = "" 

def make_conn():
    global p
    context.log_level = "debug"
    p = process('./sshtty.py "ssh ctf@localhost"', shell=True)

def init_interact():
    global p
    p.recvuntil("password: ")
    p.sendline("123") #send password here.
    p.recvuntil("turned off")
    p.recvuntil(" $ ") #recvuntil basic interactive starting point.

def form_payload():
    
    #you can open file , or do what ever you like to obtain your payloads.
    global payload
    payload = "a" * 1000 #just a test..

def create_remote_file():

    global payload 
    global p 
    #assume payload is base64d here.
    cut_payload(payload)
    p.send("cat > /tmp/1 <<EOF\n{}\nEOF\n".format(payload))

def cut_payload(payload):

    pass

def continue_inter():

    global p
    content = p.recvuntil("$ ")
    print content
    p.interactive()

if __name__=="__main__":


    make_conn()
    init_interact()
    form_payload()
    create_remote_file() 
    continue_inter()
