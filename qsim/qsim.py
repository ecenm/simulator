#!/usr/bin/python

import sys, getopt
#Get the arrival rate, service rate and queue size, packet size and number of packets

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
        arrival_rate = float(arg)
    elif opt in ('-s', '--schedulerate'):
        schedule_rate = float(arg)
    elif opt in ('-n', '--numpkts'):
        num_packets = int(arg)
    elif opt in ('-p', '--pktsize'):
        packet_size = int(arg)
    elif opt in ('-q', '--qsize'):
        queue_size = int(arg)



print 'OUTPUT    :', output_filename
print 'num_packets :', num_packets
print 'packet_size :', packet_size
print 'queue_size :', queue_size
print 'arrival_rate :', arrival_rate
print 'schedule_rate :', schedule_rate

finish_time =200

bytes_in_queue = 0 
bytes_dropped = 0 

new_service_event =0 
new_arrival_event =0 

output_file = open(output_filename, "w")

arrival_time_unit = 1.0/(arrival_rate) 
service_time_unit = 1.0/(schedule_rate)

# create two lists
arrival_event=[]
service_event=[]
event_queue=[]

#arrival_event.insert(0,[0,'a'])
#service_event.insert(0,[0,'d'])

service_time = 1
processing_time = 0
print arrival_event
print service_event
max_val = 0 

for x in range(0,num_packets):
    processing_time = (packet_size/64) * service_time_unit
    new_arrival_event= new_arrival_event+arrival_time_unit 
    if len(arrival_event) == 0:
        arrival_event.insert(x,[new_arrival_event,'a', new_arrival_event, new_arrival_event+processing_time]) 	
        new_service_event= new_arrival_event+processing_time ;
        service_event.insert(x,[new_service_event,'d'])
    else:
        max_val = max(arrival_event[x-1][3], new_arrival_event) 
        new_service_event = max_val+processing_time 	
        arrival_event.insert(x,[new_arrival_event,'a', max_val, new_service_event]) 	
        service_event.insert(x,[new_service_event,'d'])

# For printing the events
print arrival_event
print service_event

# Merge both lists and sort the list
event_queue = sorted(arrival_event + service_event)

# For printing the event queue 

print event_queue

# For each item in the list, identify bytes_in_queue

for x in range (0,len(event_queue)):
    #if(event_queue[i][1]=='a')
    if event_queue[x][1] == 'a':
        if (bytes_in_queue + packet_size) < queue_size:
            bytes_in_queue = bytes_in_queue + packet_size
        else:
            bytes_dropped = bytes_dropped + packet_size
        
        print event_queue[x][1], event_queue[x][0] , event_queue[x][2], event_queue[x][3] , bytes_in_queue, bytes_dropped 

    else:    
        if bytes_in_queue > 0:
            bytes_in_queue = bytes_in_queue - packet_size
        else:
            print "cannot go in negative"
        #print event_queue[x][1], event_queue[x][0] , event_queue[x][2], event_queue[x][3] , bytes_in_queue, bytes_dropped 
        print event_queue[x][1], event_queue[x][0] , bytes_in_queue, bytes_dropped 


#print bytes_in_queue 

    newline= "%c\t%s\t%s\t%s\n" % (event_queue[x][1], event_queue[x][0] , bytes_in_queue, bytes_dropped)

    output_file.write(newline)





