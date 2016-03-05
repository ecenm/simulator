#include "topology.h"
#include "params.h"

extern DCExpParams params;

Topology::Topology() {}

SimpleTopology::SimpleTopology(
       uint32_t num_hosts,
       double bandwidth, 
       uint32_t queue_type,
       uint32_t num_myq
       ): Topology() {
    this->num_hosts = num_hosts;
    this->num_myq = num_myq;
    double c1= bandwidth;


    // Creating hosts
    for (uint32_t i=0; i < num_hosts; i++) {
        hosts.push_back(Factory::get_host(i,c1,queue_type, 1));
    }

    // Creating myq's
    
    for (uint32_t i=0; i < num_myq; i++) {
        myq.push_back(Factory::get_queue(i,0, params.queue_size,1,0,i));
        std::cout<< "creating queue : "<< i << std::endl;
    }

    // Create the switch 

    the_switch = new CoreSwitch(0, num_hosts, c1, queue_type);
    this->switches.push_back(the_switch);

    // Connect the host queues to switch and vice versa
    
    for (uint32_t i = 0; i < num_hosts; i++){
        hosts[i]->queue->set_src_dst(hosts[i], the_switch);
        Queue *q = the_switch->queues[i];
        q->set_src_dst(the_switch, hosts[i]);

    }
}

Queue* SimpleTopology::get_next_hop(Packet *p, Queue *q) {
    if  (q->dst->type == HOST) {
        assert(p->dst->id == q->dst->id);
        return NULL; // Packet arriving
    }

}

double SimpleTopology::get_oracle_pct(Packet *p) {
    //double propagation_delay = 2 * 1000000.0 * 2 * f->src->queue->propagation_delay; //us
    double propagation_delay = 2 * 1000000.0 * 2 ; //us

    //uint32_t np = ceil(f->size / params.mss); // TODO: Must be a multiple of 1460
    uint32_t np = p->size; // TODO: Must be a multiple of 1460
    //double bandwidth = f->src->queue->rate / 1000000.0; // For us
    double bandwidth = 1000000.0; // For us
   // double transmission_delay = ((np + 1) * (params.mss + params.hdr_size)
            //+ 2.0 * params.hdr_size) // ACK has to travel two hops
       // * 8.0 / bandwidth;
    double transmission_delay = 2000000;

    return (propagation_delay + transmission_delay); //us
}

