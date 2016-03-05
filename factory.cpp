#include "factory.h"
#include <assert.h>
#include "packet.h"


/* Factory method to return appropriate queue */
Queue* Factory::get_queue(
        uint32_t id, 
        double rate,
        uint32_t queue_size, 
        uint32_t type,
        double drop_prob, 
        int location
        ) { // Default drop_prob is 0.0

    switch(type) {
        case DROPTAIL_QUEUE:
            return new Queue(id, rate, queue_size, location);
    }
    assert(false);
    return NULL;
}

Host* Factory::get_host(
        uint32_t id,
        double rate,
        uint32_t queue_type,
        uint32_t host_type
        ) {
    switch (host_type) {
        case NORMAL_HOST:
            return new Host(id, rate, queue_type, NORMAL_HOST);
    }
    std::cout << host_type << " unknown\n";
    assert(false);
    return NULL;
}


Packet* Factory::get_packet(
        double start_time,
        uint32_t seq_no,
        uint32_t pf_priority,
        uint32_t size,
        Host *src,
        Host *dst
        ) { 
            return new Packet(start_time, seq_no, pf_priority,size,src,dst)  ;
    }

