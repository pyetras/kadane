import random, sys, subprocess, os
DEVNULL = open(os.devnull, 'wb')
random.seed()

wd = os.getcwd()
d = os.path.dirname(os.path.abspath(__file__))

failed = False

for tests in [[[10, 64, 100][i/3], [10, 64, 100][i%3], random.randrange(1000)] for i in range(9)]:
  print tests,
  args = [d + '/seq-orignial'] + tests
  t1 = subprocess.Popen(map(str, args), stderr=subprocess.PIPE, stdout=DEVNULL)
  out1 = t1.communicate()[1].strip().split(' Time')[0]
  
  args = ['/usr/local/bin/mpirun', '-n', '4', os.path.join(wd, sys.argv[1])] + tests
  t2 = subprocess.Popen(map(str, args), stderr=subprocess.PIPE, stdout=DEVNULL)
  out2 = t2.communicate()[1].strip().split(' Time')[0]
  
  if out1 == out2:
    print "OK"
  else:
    print "FAIL", ' '.join(map(lambda x: ' '.join(x.split(' ')[1:]), (out1, out2)))
    failed = True
  
if failed:
  sys.exit(1)