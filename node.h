#ifndef DEFINED_NODE
#define DEFINED_NODE

#include <iostream>
#include <string>
#include "queue.h"


#define HOST 0
#define SWITCH 1
#define LINK 2

#define CORE_SWITCH 10
#define AGG_SWITCH 11

#define COPPER_LINK 20
#define FIBER_LINK 21

using namespace std;

class Node {
    public:
        Node(uint32_t id, uint32_t type);
        uint32_t id;
        uint32_t type;
};

class Host : public Node {
    public:
        Host(uint32_t id, double rate, uint32_t queue_type, uint32_t host_type);
        Queue *queue;
        int host_type;
};

class Link : public Node {
    public:
        Link(uint32_t id, uint32_t link_type);
        Queue *queue;
        int link_type;
};

class CopperLink : public Link {
    public:
        CopperLink(uint32_t id, double rate, uint32_t queue_type);
};

class FiberLink : public Link {
    public:
        FiberLink(uint32_t id, double rate, uint32_t queue_type);
};


class Switch : public Node {
    public:
        Switch(uint32_t id, uint32_t switch_type);
        uint32_t switch_type;
        std::vector<Queue *> queues;
};


class CoreSwitch: public Switch 
{
	public:
		CoreSwitch(uint32_t id, uint32_t nq, double rate, uint32_t queue_type);
};

class AggSwitch: public Switch 
{
	public:
		AggSwitch(uint32_t id, uint32_t nq1, double r1, uint32_t nq2, double r2, uint32_t queue_type);
};

// My definitions 

class l2switch
{
    private:
        int id;
    public:
        int in_port;
        int out_port;
        int switching_latency;
        l2switch():switching_latency(50),in_port(4),out_port(4){};
        Queue *queue;
//		string name;
//		void recv_pkt(packet pkt);

};


class l2switch_optical: public l2switch
{
	private:
		int id;
	public:
		int in_port,out_port;
		int switching_latency;
		l2switch_optical():switching_latency(100),in_port(4),out_port(4){}
		Queue *queue;

//		string name;
//		packet pkt;
//		int number_of_ports;
//		void set_l2switch(int id, string name, int number_of_ports, int switching_latency);
//		void send_pkt(packet pkt, int port);
//		void display_send_pkt(packet pkt);
//		void display_recv_pkt(packet pkt);
//		void recv_pkt(packet pkt);

};

class l2switch_hybrid: public l2switch
{
	private:
		int id;
	public:
		int in_port;
		int out_port;
		int switching_latency;
		l2switch_hybrid():switching_latency(150),in_port(4),out_port(4){}
		Queue *queue;

//		string name;
//		packet pkt;
//		int number_of_ports;
//		void set_l2switch(int id, string name, int number_of_ports, int switching_latency);
//		void send_pkt(packet pkt, int port);
//		void display_send_pkt(packet pkt);
//		void display_recv_pkt(packet pkt);
//		void recv_pkt(packet pkt);

};


#endif
