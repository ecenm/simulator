#ifndef QUEUE_H
#define QUEUE_H

#include <deque>
#include <stdint.h>
#include <vector>

#define DROPTAIL_QUEUE 1

class Packet;
class Node;

class Queue {
    public:
        Queue(uint32_t id, double rate, uint32_t limit_bytes, int location);
        void set_src_dst(Node *src, Node *dst);
        virtual void enque(Packet *packet);
        virtual Packet *deque();
        virtual void drop(Packet *packet);
        double get_transmission_delay(uint32_t size);
	void getsize();
	void get_num_of_packets();
        // Members
        uint32_t bus_width;
        uint32_t work_rate;
        
        uint32_t id;
        uint32_t unique_id;
        static uint32_t instance_count;
        double rate;
        uint32_t limit_bytes;
        std::deque<Packet *> packets;
        uint32_t bytes_in_queue;
        uint32_t packets_in_queue;
        bool busy;

        Packet* packet_transmitting;

        uint64_t b_arrivals, b_departures;
        uint64_t p_arrivals, p_departures;

        double propagation_delay;
        bool interested;

        uint64_t pkt_drop;
        uint64_t bytes_dropped;
        uint64_t packets_dropped;
        uint64_t spray_counter;

        int location;
        Node *src;
        Node *dst;
	double num_of_packets;
	unsigned long long start_time;
	unsigned long long end_time;
};


#endif
