#!/usr/bin/python

import os
import select
import sys
import signal
import time
from ptyprocess import PtyProcessUnicode

STDIN_FD = sys.stdin.fileno()
STDOUT_FD = sys.stdout.fileno()
user_input = ""

expect = ""

class PTY(object):
    def __init__(self):
        self.log = ''
        self.pty = None

    def set_quit_signal(self):
        oldsignal = signal.getsignal(signal.SIGINT)
        import types
        if isinstance(oldsignal, types.BuiltinFunctionType):
            signal.signal(signal.SIGINT, self.quit)

    def _process(self, argv):
        self.set_quit_signal()
        self.pty = PtyProcessUnicode.spawn(argv)
        #self._interactive()
        #self.pty.wait()
        #return (self.pty.exitstatus, self.log)

    def _exit(self):

        self.pty.wait()
        return (self.pty.exitstatus, self.log)

    def quit(self, *args, **kwargs):
        sys.exit(1)  # must exit with something not '0'

    def write_log(self, b):
        assert isinstance(b, str), 'log only str'
        self.log += b

    def _recvuntil(self, wants_data):

        assert isinstance(self.pty, PtyProcessUnicode)
        rfds, wfds, xfds = select.select([self.pty.fd, STDIN_FD], [], [])
        anything = False
        if self.pty.fd in rfds:
            anything |= self._print_response(wants_data)
        if not anything:
            return False
        else:
            return True
    
    def _print_response(self, wants_data):

        assert isinstance(self.pty, PtyProcessUnicode)
        while 1:
            try:
                data = self.pty.read(4096)
            except EOFError:
                return False
            self._write_stdout(data.encode("utf-8"))
            if wants_data in data:
                return True 
            else:
                continue

    def _send(self, send_data):

        assert isinstance(self.pty, PtyProcessUnicode)
        rfds, wfds, xfds = select.select([self.pty.fd, STDIN_FD], [], [])
        anything = False
        print "try sending"
        if STDIN_FD in rfds:
            anything |= self._read_input(send_data)
        if not anything:
            return False
        else:
            return True
    
    def _read_input(self, input_data):

        assert isinstance(self.pty, PtyProcessUnicode)
        #we do not acquire input from stdin this time.
        if not input_data:
            return False
        self.pty.write(input_data)
        return True

    def _interactive(self):
        '''
        Main select loop. Passes all data to self.master_read() or self.stdin_read().
        '''
        assert isinstance(self.pty, PtyProcessUnicode)
        while True:
            rfds, wfds, xfds = select.select([self.pty.fd, STDIN_FD], [], [])

            anything = False

            if self.pty.fd in rfds:
                anything |= self._has_child_response()
            if STDIN_FD in rfds:
                anything |= self._has_user_input()

            if not anything:
                break

    def _has_child_response(self):
        global user_input 
        assert isinstance(self.pty, PtyProcessUnicode)
        try:
            data = self.pty.read(4096)
            self.pty.flush()
        except EOFError:
            return False

        # redirect the child's output to the stdout
        self._write_stdout(data.encode('utf-8'))
        #self.write_log(data)

        return True

    def _has_user_input(self):
        global user_input
        assert isinstance(self.pty, PtyProcessUnicode)
        data = self._read_stdin(4096)
        user_input = data
        if not data:
            return False
        self.pty.write(data)
        self.pty.flush()
        return True

    def _read_stdin(self, bufsize):
        return os.read(STDIN_FD, bufsize)

    def _write_stdout(self, data):
        assert isinstance(data, bytes), '`data` must be a bytes'
        os.write(STDOUT_FD, data)

def process_input():
    
    #this function turns input into dict.
    #we suppose that all parameters sends inorder && _process invokes with a list
    inputer = sys.argv[1]
    inputer = inputer.split(" ")
    return inputer

inputs = process_input()
ptyer = PTY()
ptyer._process(inputs)
ptyer._interactive()



