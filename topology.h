#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "queue.h"
#include "packet.h"
#include "node.h"
#include "factory.h"
#include "assert.h"

class Topology{
	public:             
        Topology();
        virtual Queue *get_next_hop(Packet *p, Queue *q) = 0;
        virtual double get_oracle_pct(Packet* p) = 0 ;

        uint32_t num_hosts;
        uint32_t num_myq;

        std::vector<Host *> hosts;
        std::vector<Switch *> switches;
        std::vector<Queue *> myq;

};

class SimpleTopology: public Topology {
    public:
        SimpleTopology(uint32_t num_hosts, double bandwidth, uint32_t queue_type, uint32_t num_myq);
        virtual Queue *get_next_hop(Packet *p, Queue *q);
        virtual double get_oracle_pct(Packet *p);

        CoreSwitch* the_switch;    
};








#endif
