#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <deque>
#include <stdint.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <iomanip>
#include "assert.h"
#include "math.h"

#include "packet.h"
#include "node.h"
#include "event.h"
#include "topology.h"
#include "queue.h"
#include "random_variable.h"

#include "factory.h"

#include "packet_generator.h"
#include "stats.h"
#include "params.h"

extern Topology *topology;
extern double current_time;
//extern std::priority_queue<event*, std::vector<event*>, EventComparator> event_queue;
extern std::priority_queue<event*, std::vector<event*>, EventComparator<event> > event_queue;
extern std::deque<Packet*> packets_to_schedule;
//extern std::vector<Packet*> packets_to_switch; // My declaration
std::deque<Packet*> packets;
extern std::deque<event*> packet_arrivals;
//extern Queue* myqueue;
Queue *myqueue;
extern uint32_t num_outstanding_packets;
extern uint32_t max_outstanding_packets;
extern DCExpParams params;
extern void add_to_event_queue(event*);
extern void read_experiment_parameters(std::string conf_filename, uint32_t exp_type);
extern void read_flows_to_schedule(std::string filename, uint32_t num_lines, Topology *topo);
extern uint32_t duplicated_packets_received;
extern std::vector<Packet*> packets_for_tx_stats;
extern std::vector< std::vector<Packet*> > packets_to_switch; // My declaration

std::vector< std::vector<Packet*> > packets_for_rx_stat;
std::vector< std::vector<Packet*> > packets_for_tx_stat;
std::vector< std::vector<Packet*> > packets_to_switch;
std::vector <uint32_t> packet_pushed;
std::vector <uint32_t> no_of_packets_tracker;
std::vector <uint32_t> tokens;


extern uint32_t num_outstanding_packets_at_50;
extern uint32_t num_outstanding_packets_at_100;
extern uint32_t arrival_packets_at_50;
extern uint32_t arrival_packets_at_100;

extern double start_time;
extern double get_current_time();
double tnext;
double tfirst;
double schedule_time;

extern void run_scenario();
void run_experiment(int argc, char **argv, uint32_t exp_type) {
    if (argc < 3) {
        std::cout << "Usage: <exe> exp_type conf_file" << std::endl;
        return;
    }
   
    std::string conf_filename(argv[2]);
    read_experiment_parameters(conf_filename, exp_type);
    //params.num_hosts = 144;
    params.num_agg_switches = 9;
    params.num_core_switches = 4;

    if (params.simple_topology == 1) {
        topology = new SimpleTopology(params.num_hosts, params.bandwidth, params.queue_type, params.num_hosts);
    }    

    //std::cout << "Number of Hosts is : " << topology->num_hosts<<std::endl;
    for(int i=0;i<params.num_hosts;i++){
            packets_for_rx_stat.push_back(vector<Packet*>());  
            packets_for_tx_stat.push_back(vector<Packet*>());  
            packets_to_switch.push_back(vector<Packet*>());
            packet_pushed.push_back(uint32_t());
            no_of_packets_tracker.push_back(uint32_t());
            tokens.push_back(uint32_t());
 
            std::cout << packets_for_rx_stat.size()<< std::endl;          
    }


    uint32_t num_packets = params.num_packets_to_run;

   // input
    double input_packet_size_in_time = (1.0/double(params.input_work_rate))* ceil(double(params.packet_size)/double(params.input_bus_width));
   //double arrival_time_unit = input_packet_size_in_time;


   double input_clocks_per_packet = ceil(double(params.packet_size)/double(params.input_bus_width));

   double derived_arrival_rate = double(params.input_work_rate)/input_clocks_per_packet;
   double arrival_time_unit = 1.0/derived_arrival_rate;

   // output

   double output_packet_size_in_time = (1.0/double(params.output_work_rate))* ceil(double(params.packet_size)/double(params.output_bus_width));
   //double arrival_time_unit = input_packet_size_in_time;


   double output_clocks_per_packet = ceil(double(params.packet_size)/double(params.output_bus_width));

   double derived_schedule_rate = double(params.output_work_rate)/output_clocks_per_packet;
   double schedule_time_unit = 1.0/derived_schedule_rate;





    std::cout << "***********************************************************"<< std::endl;
    std::cout << "--------------------- Config details   --------------------"<< std::endl;
    std::cout << "***********************************************************"<< std::endl;
   std::cout<<" "<< std::endl;
   std::cout<<"queue_size "<< params.queue_size << std::endl;
   std::cout<<"num_packets_to_run "<< params.num_packets_to_run << std::endl;
   std::cout<<"packet_size "<< params.packet_size << std::endl;
   std::cout<<"Input parameters: "<< std::endl;
   std::cout<<"\t"<<"input_bus_width "<< params.input_bus_width << std::endl;
   std::cout<<"\t"<<"input_work_rate "<< params.input_work_rate << std::endl;
   std::cout<<"\t"<<"input_clocks_per_packet "<< input_clocks_per_packet << std::endl;
   std::cout<<"\t"<<"derived_arrival_rate "<< derived_arrival_rate << std::endl;
   std::cout<<"\t"<<"arrival_time_unit "<< arrival_time_unit << std::endl;
   std::cout<<"Output parameters: "<< std::endl;
   std::cout<<"\t"<<"output_bus_width "<< params.output_bus_width << std::endl;
   std::cout<<"\t"<<"output_work_rate "<< params.output_work_rate << std::endl;
   std::cout<<"\t"<<"output_clocks_per_packet "<< output_clocks_per_packet << std::endl;
   std::cout<<"\t"<<"derived_schedule_rate "<< derived_schedule_rate << std::endl;
   std::cout<<"\t"<<"schedule_time_unit "<< schedule_time_unit << std::endl;
   


   std::cout<<" "<< std::endl;
   //double arrival_time_unit = 1.0/params.arrival_rate; old way

    add_to_event_queue(new StatsEvent (0));
 
// for :(iterates over queues)
  for (int j=0;j<1;j++){ // testing for single queue chaining
  //for (int j=0;j<params.num_hosts;j++){ 
   for (uint32_t i = 0; i < params.num_packets_to_run; i++){

    double new_arrival_event = new_arrival_event+arrival_time_unit;

//   std::cout<<"-------------------------------------------> "<< std::endl;
    Packet *p1 = new Packet(start_time, i, 0, params.packet_size, topology->hosts[0], topology->hosts[1]);
    p1->start_time = 0 ;
    p1->end_time =0 ;
    p1->qid=j; // encoding the identification of queue for packet
//    std::cout << p1->qid << std::cout;	
    //packets_for_tx_stats.push_back(p1);// array for stats
    packets_for_tx_stat[j].push_back(p1);// array for stats
    
    //std::cout<< "PRINTINGGGGGGGG "<< params.num_hosts <<  std::endl;
    //std::cout<< packets_for_tx_stats[i]->seq_no << packets_for_tx_stats[i]->start_time << "=======" << packets_for_tx_stats[i]->end_time << std::endl;

    add_to_event_queue(new PacketCreationEvent(start_time, p1)); 
    p1->sending_time = new_arrival_event;
    //tnext = tnext;
    

    add_to_event_queue(new PacketPushingEvent(new_arrival_event, p1, topology->hosts[j]->queue[0]));
    //std::cout << topology->hosts[j]->queue[0]->node_details.my_type << std::endl;
    
    
    if(params.debug==1){
    std::cout << " AT PACKETGEN <<< Packets sent from Host: "<< p1->src->id << " to Host: " <<p1->dst->id <<" with size "<< p1->size << ", sequence number " << p1->seq_no <<" start_time "<< p1->start_time <<" and sending time of "<< p1->sending_time<<  std::endl;
    		}
	
	}
}
    add_to_event_queue(new LoggingEvent (200000000));
   run_scenario();

}


