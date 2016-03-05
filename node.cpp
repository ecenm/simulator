#include "node.h"
#include "factory.h"
#include "params.h"

extern DCExpParams params;


Node::Node(uint32_t id, uint32_t type) {
    this->id = id;
    this->type = type;
}

// TODO FIX superclass constructor
Host::Host(uint32_t id, double rate, uint32_t queue_type, uint32_t host_type) : Node(id, HOST) {
    queue = Factory::get_queue(id, rate, params.queue_size, queue_type, 0, 0);
    this->host_type = host_type;
}

Link::Link(uint32_t id, uint32_t link_type) : Node(id, LINK) {
    this->link_type = link_type;
}

CopperLink::CopperLink(uint32_t id, double rate, uint32_t type) : Link(id, COPPER_LINK) {
    queue = Factory::get_queue(id, rate, params.queue_size, type, 0, 0);
}

FiberLink::FiberLink(uint32_t id, double rate, uint32_t type) : Link(id, FIBER_LINK) {
    queue = Factory::get_queue(id, rate, params.queue_size, type, 0, 0);
}



// TODO FIX superclass constructor
Switch::Switch(uint32_t id, uint32_t switch_type) : Node(id, SWITCH) {
    this->switch_type = switch_type;
}



CoreSwitch::CoreSwitch(uint32_t id, uint32_t nq, double rate, uint32_t type) : Switch(id, CORE_SWITCH) {
    for (uint32_t i = 0; i < nq; i++) {
        queues.push_back(Factory::get_queue(i, rate, params.queue_size, type, 0, 2));
    }
}

//nq1: # host switch, nq2: # core switch
AggSwitch::AggSwitch(
        uint32_t id,
        uint32_t nq1,
        double r1,
        uint32_t nq2,
        double r2,
        uint32_t type
        ) : Switch(id, AGG_SWITCH) {
    for (uint32_t i = 0; i < nq1; i++) {
        queues.push_back(Factory::get_queue(i, r1, params.queue_size, type, 0, 3));
    }
    for (uint32_t i = 0; i < nq2; i++) {
        queues.push_back(Factory::get_queue(i, r2, params.queue_size, type, 0, 1));
    }
}

//l2switch::l2switch:port(3),switching_latency(params.queue_size0){}
