memcheck: run_checks
	valgrind --leak-check=full --show-leak-kinds=all ./run_checks



import os
import subprocess
import sys

def absolute_path(relative_path):
  return os.path.join(os.path.abspath(sys.path[0]), relative_path) 

LOXPP_PATH = absolute_path('../bin/loxpp') 

def memcheck_file(path):
  subprocess.run(['valgrind', '--leak-check=full', '--show-leak-kinds=all',  LOXPP_PATH, '<', path], stdout=subprocess.PIPE)
  print(result.stdout.decode('utf-8'))


if __name__ == '__main__':

  files = []

  for path in files:
    memcheck_file(path)
 
