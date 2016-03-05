import getopt
import sys


num_packets= 10
packet_size= 64
queue_size= 2000
arrival_rate= 3
schedule_rate= 2

output_filename = 'default.txt'

print 'ARGV      :', sys.argv[1:]

options, remainder = getopt.getopt(sys.argv[1:], 'o:a:s:n:p:q:', ['output=', 
                                                         'arrivalrate=',
                                                         'schedulerate=',
                                                         'numpkts=',
                                                         'pktsize=',
                                                         'qsize=',
                                                         ])
print 'OPTIONS   :', options

for opt, arg in options:
    if opt in ('-o', '--output'):
        output_filename = arg
    elif opt in ('-a', '--arrivalrate'):
        arrival_rate = arg
    elif opt in ('-s', '--schedulerate'):
        schedule_rate = arg
    elif opt in ('-n', '--numpkts'):
        num_packets = arg
    elif opt in ('-p', '--pktsize'):
        packet_size = arg
    elif opt in ('-q', '--qsize'):
        queue_size = arg



print 'VERSION   :', version
print 'VERBOSE   :', verbose
print 'OUTPUT    :', output_filename
print 'num_packets :', num_packets
print 'packet_size :', packet_size
print 'queue_size :', queue_size
print 'arrival_rate :', arrival_rate
print 'schedule_rate :', schedule_rate


