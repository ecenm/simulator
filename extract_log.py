import os
import re

newpath = 'results'
if not os.path.exists(newpath):
    os.makedirs(newpath)

with open('log.txt') as f_input:
    data = f_input.read()

    for match in re.finditer(r'log_rx_packets_start_queue (\d+)\n(.*?)log_rx_packets_end_queue', data, re.M + re.S):
        queue, block =  match.groups()

        with open(os.path.join(newpath,'queue_{}.log'.format(queue)), 'w') as f_output:
            f_output.write(block)
