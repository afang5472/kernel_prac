#!/usr/bin/python

import socket
from hashlib import md5
from pwn import remote
import sys, os
import time

if len(sys.argv) < 3:
    print >> sys.stderr, "Usage: %s <upload_file> <host:port> [<dest_path>] [-i|--interactive]" % sys.argv[
        0]
    exit(1)

interactive = "-i" in sys.argv or "--interactive" in sys.argv

argv = [i for i in sys.argv if i not in ["-i", "--interactive"]]

upload_file = argv[1]
if not os.path.isfile(upload_file):
    print >> sys.stderr, "Error: File %s does not exist or is not a regular file." % upload_file
    exit(2)

hostport = argv[2]
if ":" not in argv[2]:
    host = "0.0.0.0"
    port = int(argv[2], 0)
else:
    hplist = argv[2].split(":")
    host = hplist[0]
    port = int(hplist[1], 0)

if len(argv) > 3:
    dest_path = argv[3]
else:
    dest_path = "/tmp/" + upload_file.split("/")[-1]

print >> sys.stderr, "[+] Start generate payload"

payload = ""

with open(upload_file, "rb") as f:
    raw = f.read()

tmp_file = dest_path + "." + md5(raw).hexdigest() + ".base64"

payload += "\n"
payload += "rm -f " + dest_path + " && "
payload += "cat > " + tmp_file + " <<EOF\n"
payload += raw.encode("base64")
payload += "\nEOF\n"
payload += "base64 -d " + tmp_file + " > " + dest_path + " && chmod a=rx " + dest_path + " && rm -f " + tmp_file + "\n"


def cut_payload(s):
    if not s:
        return []
    result = []
    cur_str = ""
    for i in s.split("\n"):
        if len(cur_str) + len(i) > 4095:
            result.append(cur_str)
            cur_str = i + "\n"
        else:
            cur_str += i + "\n"
    result.append(cur_str[:-1])
    return result


#print >> sys.stderr, "[ ] Connecting server"
#s = socket.socket()
#s.connect((host, port))

if port == -1:
    print payload
    exit(0)

r = remote(host, port)

#print >> sys.stderr, "[+] Connected to server " + host + ":" + str(port)
cp = cut_payload(payload)
for i in range(len(cp)):
    print "Wave", i
    r.send(cp[i])
    print "recv:", repr(r.recvrepeat(0.1))[:64], "[...]"
    #time.sleep(0.5)

print >> sys.stderr, "[+] Finished sending payload"

if interactive:
    r.interactive()
