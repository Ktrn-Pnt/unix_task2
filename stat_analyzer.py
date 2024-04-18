import sys
import re
from statistics import variance 

GET_PID = re.compile('^PID = \d+')
GET_LOCKS_COUNT = re.compile('locks: \d+$')
MAGIC_CONSTANT = 3

def main(file_name, process_count):
    pid_locks = {}
    with open(file_name, 'r+') as file:
        line = file.readline()

        while (line):
            pid = int(((GET_PID.findall(line)[0]).split())[2])
            locks = int(GET_LOCKS_COUNT.findall(line)[0].split()[1])
            if (pid and locks):
                pid_locks[pid] = locks
            line = file.readline()

        if (len(pid_locks) != process_count):
            file.write('Not all processes have completed successful\n')
        else:
            file.write('All processes have completed successful\n')

        average = 0

        for i in pid_locks.values():
            average += i

        average = average / len(pid_locks.values())

        variance = 0

        for i in pid_locks.values():
            variance = (i - average) ** 2

        variance = variance / len(pid_locks.values())

        file.write('Variance: {0}\n'.format(variance))

        if (variance < MAGIC_CONSTANT):
            file.write('Lock statistics are approximately the same\n')
        else:
            file.write('Lock statistics are not the same\n')
        

if __name__ == "__main__":
    main(sys.argv[1], int(sys.argv[2]))