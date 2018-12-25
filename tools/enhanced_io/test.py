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
import base64 


p = ""
payload = "" 
p2_length = ""

def make_conn():
    global p
    #context.log_level = "debug"
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
    fp = open("./testbin","rb")
    payload = fp.read()

def create_remote_file():

    global payload 
    global p 
    global p2_length
    #assume payload is base64ed here.
    payloads = cut_payload()
    p2 = []
    for pay in payloads:
        p2.append(b64_encode(pay))
    
    #write into file in sequence
    #p2 is the target to write
    p2_length = len(''.join(p2))
    print p2_length
    raw_input("")
    
    for load in p2:
        p.sendline("echo -n {}>>/tmp/temp".format(load))
    #p.recvuntil("$ ")
    #p.send("")

def cut_payload():

    global payload
    length = len(payload)
    counts = len(payload) / 600 
    if length % 600 > 0:
        counts += 1
    #print "counts: " + str(counts)
    payloads = []
    if counts == 1:
        payloads.append(payload)
        return payloads

    for i in range(counts):
        if i == counts - 1:
            payloads.append(payload[i*600:])    
        else:
            payloads.append(payload[i*600:(i+1)*600])
    return payloads 

def b64_encode(payloader):

    return base64.b64encode(payloader) 
        
def continue_interactive():

    global p
    global p2_length 
    #content = p.recvuntil("$ ")
    #print content
    p.recv(p2_length)
    p.interactive()

if __name__=="__main__":

    #form_payload()
    #create_remote_file()
    make_conn()
    init_interact()
    form_payload()
    create_remote_file() 
    continue_interactive()

