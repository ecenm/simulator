#ifndef FACTORY_H
#define FACTORY_H

#include "node.h"
#include "queue.h"

/* Queue types */
#define DROPTAIL_QUEUE 1

/* Host types */

#define NORMAL_HOST 1
#define SCHEDULING_HOST 2

/* Packet types */
#define NORMAL_PKT 1


class Factory {
    public:

        static Packet *get_packet(
                double start_time,
                uint32_t seq_no,
                uint32_t pf_priority,
                uint32_t size,
                Host *src,
                Host *dst
                //double paced_rate = 0.0
                );


        static Queue *get_queue(
                uint32_t id, 
                double rate,
                uint32_t queue_size, 
                uint32_t type,
                double drop_prob, 
                int location
                );

        static Host* get_host(
                uint32_t id,
                double rate,
                uint32_t queue_type,
                uint32_t host_type
                );
 
};

#endif
