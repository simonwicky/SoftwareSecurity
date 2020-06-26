#! /usr/bin/env python3

import argparse
import os
import subprocess
import sys

def check_running_containers():
  cmd = ['docker', 'ps', '-aq', '--filter=name=grass']
  ps_proc = subprocess.run(cmd, stdout = subprocess.PIPE)

  if ps_proc.stdout != b'':
    print('Existing container with name \'grass\'')
    print('Kill with command \'docker rm grass\' before running exploit')
    sys.exit(1)

def pull_image():
  cmd = ['docker', 'pull', 'atrib/grass']
  subprocess.run(cmd)

def get_docker_base_cmd(args = []):
  cwd = os.getcwd()
  uid = os.getuid()
  docker_base_cmd = ['docker', 'run'] + args + ['--name=grass', '--cap-add=SYS_PTRACE', '-e', 'GRASS_ROOT=/grass', '-v', cwd + ':/grass', 'atrib/grass:latest', str(uid), '--']

  return docker_base_cmd

def launch_interactive():
  docker_interactive_cmd = get_docker_base_cmd(['-it'])
  subprocess.run(docker_interactive_cmd)

  subprocess.run(['docker', 'rm', 'grass'])
  

def get_patch_cmd(patchfile, args):
  base_patch_args = ['-p0', '-ruN', '-f', '-i', patchfile, '-d', '.']

  return ['patch'] + args + base_patch_args

def patch_source(exploit_num, reverse = False):
  patchfile = os.path.join(os.path.join(os.path.join(os.getcwd(), 'bugs'), 
                                        str(exploit_num)), 'fix.patch')
  
  if not reverse:
    print('%%%%%%%% Patch dry run')
    patch_proc = subprocess.run(get_patch_cmd(patchfile, ['--dry-run']))

    if patch_proc.returncode == 1:
      print('Invalid patch file: ' + patchfile)
      sys.exit(1)

  print('%%%%%%%% ' + ('Reverting patch...' if reverse else 'Patching...'))
  patch_proc = subprocess.run(get_patch_cmd(patchfile, ['-R'] if reverse else []))
  print('-------- Done: Patching')


def build_grass(image = 'atrib/grass:latest', clean = False):
  print('%%%%%%%% Building project')
  docker_base_cmd = get_docker_base_cmd()
  make_base_cmd =  ['make', '-C', '/grass']
  
  if clean:
    subprocess.run(docker_base_cmd + make_base_cmd + ['clean'])
    subprocess.run(['docker', 'rm', 'grass'])
  
  subprocess.run(docker_base_cmd + make_base_cmd)
  subprocess.run(['docker', 'rm', 'grass'])
  print('-------- Done: Building project')

def run_exploit(exploit_num):
  docker_base_cmd = get_docker_base_cmd()
  runexploit_cmd =  ['/grass/run_exploit.py', str(exploit_num)]

  print('%%%%%%%% Running exploit')
  subprocess.run(docker_base_cmd + runexploit_cmd)
  print('-------- Done: Running exploit')
  subprocess.run(['docker', 'rm', 'grass'])


def parseargs():
  N_bugs = 4

  parser = argparse.ArgumentParser(
    description = '''
Run useful tasks for testing for bugs and exploits in the provided docker 
environment based on Ubuntu 18.04. Without any arguments, it runs an 
interactive docker shell within the container, allowing for manual testing
and debugging. With arguments, it runs automatedly, allowing building and 
running GRASS exploits with and without patches.
'''
  ) 
  parser.add_argument('-u', action = 'store_true', default = False,
                      help = 'Pull updated docker image, use if TAs recommend')
  parser.add_argument('-b', action = 'store_true', default = False, 
                      help = 'Build the grass binaries')
  parser.add_argument('-c', action = 'store_true', default = False, 
                      help = 'Clean before build, implies build')
  parser.add_argument('-e', type=int, choices = range(0, N_bugs + 1), 
                      help = 'Run exploit for bug N, implies build')
  parser.add_argument('-p', action = 'store_true', default = False, 
                      help = 'Patch bug N, requires exploit number N')
  parser.add_argument('-i', action = 'store_true', default = False,
                      help = 'Launch interactive shell. Only allowed with pull')

  args = parser.parse_args()
  if args.i:
    args.b = False
    args.c = False
    args.e = None
    args.p = False

  if args.c:
    args.b = True

  if args.e != None:
    args.b = True

  if args.p and args.e == None:
    parser.error('Patch requires exploit number')

  return args

def main():
  args = parseargs()

  check_running_containers()

  # Pull update image if reqested
  if args.u:
    pull_image()

  # Launch interactive shell if requested
  if args.i:
    launch_interactive()
    return

  # If asked for, patch before building
  if args.p:
    patch_source(args.e)

  if args.b:
    build_grass(clean = args.c)

  if args.e != None:
    run_exploit(args.e)

  # Revert patch
  if args.p:
    patch_source(args.e, reverse = True)

if __name__ == '__main__':
  main()
