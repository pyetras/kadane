#!/usr/bin/env python
seed = 1234

job = """# @ job_name = %(name)s
# @ account_no = G52-5
# @ class = kdm-dev
# @ error = %(name)s.err
# @ output = %(name)s.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 01:00:00
# @ notification = error
# @ notify_user = $(user)@icm.edu.pl
# @ job_type = bluegene
# @ bg_size = %(nodes)s
# @ queue
echo "::Started at" `date`
echo "0:name:%(name)s"
echo "0:nodes:%(nodes)s"
echo "0:proc:%(proc)s"
echo "0:nproc:%(nproc)s"
mpirun -np %(nproc)s -mode %(proc)s /home/users/ps292408/zad3/msp-par.exe 4000 4000 %(seed)s
echo "::Finished at" `date`
(sleep 1 && cat %(name)s.out | python ../poster.py %(name)s) &
"""

nodes = [1, 2, 4, 8, 16]
proc = [1, 2, 4]

procname = {1: 'SMP', 2: 'DUAL', 4: 'VN'}

for n in nodes:
  for p in proc:
    name = 'MSP_4000_4000_%s_%s_%s'%(seed, n, p)
    j = job%({'name': name, 'nodes': n, 'proc': procname[p], 'nproc': p*n, 'seed': seed})
    print>>open('jobs/%s.ll'%name, 'w'), j
    print 'llsubmit %s.ll'%name

