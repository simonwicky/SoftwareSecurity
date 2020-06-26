# Task 1: Manual code review (50 points)

In this part of the project, you will be performing manual code review to find
bugs in a software called Grep-as-a-Service (or GRASS). If you're unfamiliar
with grep, see `man grep`. The source, documentation, and tutorials for the
software can be found in the `grass/` folder. In this part
of the project, you will be expected to find **5 bugs** in this code base,
write bug-reports for them (as we have done in the labs) along with exploits
and suggest fixes. Each bug is worth 10 points: 6 points for the report, 2 for 
the exploit, and 2 for the patch.


## GRASS: A summary

The software, GRASS, comprises a server and a client. The server allows the
client to access a sandboxed folder, upload to and download files from this
folder, and run a regular-expression search on files in this folder. You are
searching bugs in the server.

The client includes an interactive mode, where the user can type commands.
Alternatively, the client can automatically read commands from a file and send
them to the server. The server reads its configuration from a file.

See the `grass/GRASS.pdf` file for detailed documentation of the protocol.


## Your TODO: Bug reporting

Bugs will be reported in the folder `grass/bugs`. Each bug is reported in a separate
folder, leading to a directory structure including `grass/bugs/1`, `grass/bugs/2`,
etc. You will receive points for the first 5 bugs. An example (bug 0) is already
provided. Each bug requires:

- A report (6 points). This is the file `grass/bugs/<n>/report`. The report is 
  structured as a series of questions. Answer all these questions. Give as much 
  information as you need to understand the bug, your exploit, or your fix.
- An exploit (2 points). This exploit will be run using the `grass/run_exploit.py` 
  script. The full exploit requires these files:
  - A configuration file `grass.conf` for the server. Do not change the name of
    this file. The config includes a base directory for the server and a port
    number it uses to listen for incoming connections. Do not change these values.
    The configuration file also includes a list of users. Feel free to modify, add
    or delete users as required by your exploit.
  - A list of commands issued by the client in the file `client_command`. Modify
    this file as required. In the example, `grass/bugs/0/client_command`, the
    client executes commands to log into the server, get its username, upload
    the file `hello`, download the file `server_hello`, and then execute the
    bug-triggering instruction. Triggering the bug does not require all these
    commands, but they are kept as an example of a command file.
  - Client files in the folder `grass/bugs/<n>/client_files`. These files will
    exist in the client's working directory, can be uploaded to the server.
    Create the files you need for your exploit. In the example bug, the client
    was able to upload the file `grass/bugs/0/client_files/hello` to the server.
  - Server files in the folder `grass/bugs/<n>/server_files`. These files will
    exist in the server's base directory, as defined in the configuration file.
    Create the files you need for your exploit. In the example bug, the client
    was able to download the file `grass/bugs/0/server_files/server_hello`.
  > Note: The exploits will be run automatically using a script. 
    Therefore, carefully follow the description above when submitting your work.
    The configuration file, and the list of commands is mandatory for each exploit. 
    The server and client files are needed only if your exploit needs it.
  > Note 2: try to keep your exploits minimal, i.e., reduce the amount of
    interactions with the server and simplify as much as you can.
- A fix (2 points). This is the file `grass/bugs/<n>/fix.patch` and uses the format
  recognized by the program `patch`, a standard GNU utility. The fix for the
  example bug can be seen in `grass/bugs/0/fix.patch`. This file contains a
  list of lines to add (lines starting in '+') and delete ('-') from the source.
  The `grass/run_exploit.py` script will use this file to patch the source to
  check that the bug has, indeed, been fixed. 
  > Note: The fix must be described in the report.

  > Note: The patch file will be applied automatically during grading. Ensure that your 
    patch follows the correct format. The final section shows the commands that 
    may be used for generating the patch. 


## Environment and `run.py`

GRASS is written assuming a Ubuntu environment, and has been tested on
Ubuntu 18.04 and Ubuntu 19.10. It was written as a project, not as a commercial
software. Therefore, it is not particularly portable across different environments.
To reduce any effects due to environment, the project will be run and graded within 
a [Docker](https://www.docker.com) container. These provide
a lightweight, consistent virtualization environment. Docker containers are
significantly lighter than VMs, requiring less hardware support, and spawning
almost instantaneously. For a quick tutorial on Docker, check out this
[video](https://www.youtube.com/watch?&v=fqMOX6JJhGo). 
The Docker image is based on Ubuntu 18.04 and is hosted on Docker Hub. The tools
installed in the image includes the below. Let us know if you need other tools.

- gcc, g++ (multilib)
- build-essential
- gdb
- Editors (vim nano)
- git
- python3
- tmux (for spawning multiple shells in a single terminal)

> Note: Please follow the 
  [post-installation steps](https://docs.docker.com/engine/install/linux-postinstall/) 
  for Docker on Linux. This will enable you to spawn containers as non-root
  users. Do not execute `run.py` as root.

Running the docker image with the correct parameters is crucial. Therefore, we
also provide you with a run script (`run.py`) which runs a container with the
correct arguments. It should aid you test and run GRASS and its exploits. 
It should be run from the `grass/` folder. The script spawns containers based 
on the provided image, with the `grass/` directory on the host mounted as 
`/grass/` in the container. Therefore, you can use your favourite code editor
on your host machine to directly work on code in the folder. 
With the `-i` argument, it opens an interactive shell within the container you
can use to explore the project. With other arguments, it builds the project
within the container, runs exploits, and patches code. This script should allow 
you to test your exploits and fixes. For example, consider the following runs 
with the example bug (0).

Below are a few more details which you will probably never need.
Within the container, code will be run as a user ('cs412') belonging to 
group ('cs412'). The user's password is 'cs412', and its UID matches that
of the user spawning the container on the host. This setup means that 
any files created during the container's execution belong to the user on
the host. 

Example: Clean and build the project. The files `bin/server` and `bin/client` are
created.

```bash
>>ls
bugs  inc  Makefile  run_exploit.py  run.py  src  trash

>>./run.py -bc                                  # Perform a clean build
%%%%%%%% Building project
make: Entering directory '/grass'
rm -f -r bin/client bin/server
rm -drf bin
make: Leaving directory '/grass'
grass
make: Entering directory '/grass'
mkdir -p bin; 
g++ -Wall -Wextra -g -fno-stack-protector -m32 -z execstack -pthread -I . -Wl,--section-start=.text=0x11111111 src/client/client_main.cpp src/services/logging.cpp src/services/socket.cpp src/services/threadpool.cpp src/client/client.cpp src/grass.c -o bin/client
g++ -Wall -Wextra -g -fno-stack-protector -m32 -z execstack -pthread -I . -Wl,--section-start=.text=0x11111111 src/server/server_main.cpp src/commands/command.cpp src/commands/parser.cpp src/server/server.cpp src/services/logging.cpp src/services/socket.cpp src/services/threadpool.cpp src/user/user.cpp src/grass.c -o bin/server
make: Leaving directory '/grass'                # Successful build
grass                                           # Removing container after build
-------- Done: Building project

>>ls                                            # GRASS binaries in bin/
bin  bugs  inc  Makefile  run_exploit.py  run.py  src  trash

>>ls bin
client  server
```

Example: The exploit for bug 0 is run. As can be seen, the server crashes and
is terminated with the SIGSEGV (11) signal.

```bash
>>./run.py -e0                                  # Running exploit 0
%%%%%%%% Building project                       # Building GRASS before running exploit
make: Entering directory '/grass'
mkdir -p bin; 
make: Leaving directory '/grass'
grass                                           # Removing container after build
-------- Done: Building project
%%%%%%%% Running exploit
Server is made
Server is running
Exit handle added, waiting for termination
Starting client
Automated mode started
login u1                                        # The example client commands file performs 
Success:                                        # login, pass, whoami, get and put commands
pass p1                                         # to demonstrate normal functioning of GRASS
Success: 
whoami
u1
put hello 11
put port: 44633
get server_hello
get port: 45675 size: 5
fault                                           # Issuing the fault command which triggers bug 
logout
exit
-------- Client done
-------- Server terminated by signal 11         # We see that the server crashed (SIGSEGV = 11)
-------- Done: Running exploit
grass                                           # Removing container after running exploit
```

Example: The exploit for bug 0 is run with the patch to fix the bug. Here, you
see that the server is still running after the client is done. This indicates
that the server did not crash and is running. Finally, the script sends the
SIGINT signal to the server to ask it to gracefully quit. The bug has been patched.

```bash
>>./run.py -e0 -p                               # Running exploit 0 with patch
%%%%%%%% Patch dry run                          # Patching dry run. Tests if patch is valid
checking file src/commands/command.cpp
%%%%%%%% Patching...                            # Actual patch application
patching file src/commands/command.cpp
-------- Done: Patching
%%%%%%%% Building project                       # Rebuilding with patch
make: Entering directory '/grass'
mkdir -p bin; 
g++ -Wall -Wextra -g -fno-stack-protector -m32 -z execstack -pthread -I . -Wl,--section-start=.text=0x11111111 src/server/server_main.cpp src/commands/command.cpp src/commands/parser.cpp src/server/server.cpp src/services/logging.cpp src/services/socket.cpp src/services/threadpool.cpp src/user/user.cpp src/grass.c -o bin/server
make: Leaving directory '/grass'
grass                                           # Removing container after build
-------- Done: Building project
%%%%%%%% Running exploit                        # Running exploit on patched binaries  
Server is made
Server is running
Exit handle added, waiting for termination
Starting client
Automated mode started
login u1
Success: 
pass p1
Success: 
whoami
u1
put hello 11
put port: 54133
get server_hello
get port: 48435 size: 5
fault

logout
Success: logged-out
exit
-------- Client done
%%%%%%%% Server running
%%%%%%%% Checking for grassexternal             # A `ps` command shows the server running
cs412         6     1  0 15:52 ?        00:00:00 /grass/bin/server
cs412        16     1  0 15:53 ?        00:00:00 grep -E (grassexternal|server)
-------- Done: Checking for grassexternal
%%%%%%%% Killing server with SIGINT
error: FD is 9, input is Hello World
Interrupt signal (2) received.
Server finished, now thread pool
Pool finished
-------- Done: Running exploit                  # The exploit was not able to crash the patched server
grass                                           # Removing container after running exploit
%%%%%%%% Reverting patch...                     # Reverting source to unpatched state
patching file src/commands/command.cpp
-------- Done: Patching
```


### Required software:

To run this project, the required software are listed below. The list is short
since most of the compilation and running happens within a container.

- Linux install
- docker (see [post-installation steps for Linux](https://docs.docker.com/engine/install/linux-postinstall/)
          to run docker without root)
- python3


## Exploits, and how to detect them

In your project, you want to exploit bugs in GRASS before fixing them. Akin to
real software, bugs in GRASS may allow you to corrupt memory, crash the program,
redirect control-flow, or execute arbitrary commands in a shell. For each of these
outcomes, we will track specific events to check that the exploit succeeded.

- Crashing the server. If a bug allows your exploit to crash the server, the
  `run_exploit.py` detects and reports it. For example, we see the server
  being terminated due to a SIGSEGV for the bug 0.
- Corrupt memory: Uncontrolled memory corruption will probably lead to crashing
  the server. However, if you are capable of arbitrary writes to memory, you
  may overwrite the global variable `global_flag`. After executing each client
  command, the server checks this flag and outputs a message. Thus, the output
  of `run.py` will contain proof of this corruption.
- Control-flow hijack: To demonstrate the ability to hijack control-flow, redirect
  execution to the function `hijack_flow()`. This function will print a message
  indicating that it ran.
- Arbitrary command execution: Various bugs, including arbitrary code injection,
  can lead to the ability to launch programs. We provide the program
  `/usr/bin/grassexternal.py` whose execution you should trigger to show this
  capability. The script will print a message and loop infinitely. The
  `run_exploit.py` script also looks at the list of running processes to check
  for the `grassexternal.py` script. When executed, the output include the
  following text.
  
  ```bash
  %%%%%%%% Checking for grassexternal
  cs412         6     1  0 16:31 ?        00:00:00 /grass/bin/server
  cs412        13     1  0 16:31 ?        00:00:00 python3 /usr/bin/grassexternal.py
  cs412        15     1  0 16:31 ?        00:00:00 grep -E (grassexternal|server)
  -------- Done: Checking for grassexternal
  ```

> Note: Your exploits must use one of these methods to demonstrate the respective
  capability


## List of useful commands

Here we provide a short list of helpful commands that come in handy when you're
creating the patches, exploits, and reports.


### Creating a patch file

We assume that the original (buggy) source files are in `grass/src`, and the
fixed source files are in `grass/src_patched`. The command to generate the
patch file for the example bug is below. The `-r` flag recursively diffs the 
two folders. The `-u` flag creates an 'unified' diff, which includes unchanged
lines before and after the change. This command is run from the `grass/` directory.

```bash
diff -ru src src_patched > bugs/0/fix.patch
```


### Applying a patch file

To apply a patch file, say the patch for bug 0, run the following command from
the `grass/` directory. The `-ru` are for recursive and unified, respectively.
The `-p0` option tells `patch` to treat paths relative to the working
directory (`.`). Other interesting options are `-R`, `-f` and `--dry-run`.

```bash
patch -p0 -ru -i bugs/0/fix.patch -d .
```
