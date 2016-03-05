#include "event.h"
#include "factory.h"
#include "topology.h"
#include "params.h"
#include <algorithm> 


extern Topology* topology;
extern std::priority_queue<event*, std::vector<event*>, EventComparator> event_queue;
extern double current_time;
extern std::deque<event*> packet_arrivals;
extern std::deque<Packet*> packets_to_schedule;
extern std::vector<Packet*> packets_for_rx_stats;
extern std::vector<Packet*> packets_for_tx_stats;

// Parametrized data structures that are fetched from experiment.cpp

extern std::vector< std::vector<Packet*> > packets_for_rx_stat;
extern std::vector< std::vector<Packet*> > packets_for_tx_stat;
extern std::vector< std::vector<Packet*> > packets_to_switch;
extern std::vector <uint32_t> packet_pushed;
extern std::vector <uint32_t> no_of_packets_tracker;
extern std::vector <uint32_t> tokens;



//extern std::vector<Packet*> packets_to_switch;
extern DCExpParams params;
extern Queue* myqueue;
extern vector<Packet*>::iterator it;
//extern vector<Packet*>::iterator itt;

//double latency = 0;
//double accumulated_size = 0;
//double accumulated_latency = 0; 
std::vector<int> found(params.num_packets_to_run, 0); // found Matrix TODO   

extern double get_current_time();
extern void add_to_event_queue(event *);
extern int get_event_queue_size();
//uint32_t no_of_packets_tracker = 0;
uint32_t county = 0;
uint32_t event::instance_count=0;
/*double tfirst;
double schedule_time;*/

// Stats counters 

extern uint32_t packets_in_the_system ;
extern uint32_t packets_in_the_queue ;

double new_service_event = 0;
double max_val = 0;
double new_arrival_event = 0 ; 
double new_token_event = 0 ; 
 
int irand (int n)
{
    static int seq[] = {
        //0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
    };

    std::random_shuffle (seq, seq + sizeof seq / sizeof *seq);

    const int rnd =
        ((seq [0] << 11) | (seq [1] << 8) | (seq [2] << 4)) + seq [3];

    return rnd % n;
}
event::event(uint32_t type, double time){
    this->type = type;
    this->time = time;
    this->cancelled = false;
    this->unique_id = event::instance_count++;


}

event::~event() {

}


PacketCreationForInitializationEvent::PacketCreationForInitializationEvent(
        double time,
        Host *src,
        Host *dst,
        RandomVariable *nv_bytes,
        RandomVariable *nv_intarr
    ) : event(PACKET_CREATION_EVENT, time) {
    this->src = src;
    this->dst = dst;
    this->nv_bytes = nv_bytes;
    this->nv_intarr = nv_intarr;
}

PacketCreationForInitializationEvent::~PacketCreationForInitializationEvent() {}

void PacketCreationForInitializationEvent::process_event() {
    uint32_t id = packets_to_schedule.size();
    uint32_t size = nv_bytes->value() * 1460;
   // assert(size != 0);
    
    packets_to_schedule.push_back(Factory::get_packet(time, id, 0 , size, src, dst));

    if(params.debug == 1){
    std::cout << "event.cpp: PacketCreation:" << 1000000.0 * time << " Generating new packet" << id << " of size "
     << size << " between " << src->id << " " << dst->id << "\n";
    }
    double tnext = time + nv_intarr->value();
    add_to_event_queue(
            new PacketCreationForInitializationEvent(
                tnext,
                src,
                dst,
                nv_bytes,
                nv_intarr
                )
            );
}

//---- Constructors and methods for PacketCreationEvent ------------

PacketCreationEvent::PacketCreationEvent(
	double time, 
	Packet *packet
	): event(PACKET_CREATION_EVENT, time) {
    this->packet = packet;
	// Have to write my code here

	}

PacketCreationEvent::~PacketCreationEvent(){}


void PacketCreationEvent::process_event(){

	// Write code here
    uint32_t created_flag = 0; 

    //packets_for_tx_stats.push_back(packet);

    if(params.debug==1){
    //std::cout<< get_current_time() <<" : Packet Pushing Event "<< topology->myq[0]->getsize()<<std::endl;
    std::cout<< get_current_time() <<" : Packet Creation Event "<< packet->seq_no <<std::endl;
    //topology->myq[0]->getsize();

        std::cout << " AT PACKET_CREATION <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
    }
    }

//---- Constructors and methods for PacketCreationEvent ------------

PacketPushingEvent::PacketPushingEvent(
	double time, 
	Packet *packet
	): event(PACKET_PUSHING_EVENT, time) {
    this->packet = packet;
	// Have to write my code here

	}

PacketPushingEvent::~PacketPushingEvent(){}


void PacketPushingEvent::process_event(){

	// Write code here
    uint32_t current_packets_in_queue;
    uint32_t time_to_deque;

   double new_arrival_event = packet->starting_time;
   uint32_t q_id = packet->qid;
   uint32_t size = packets_to_switch[q_id].size();

   // Input side
   double input_packet_size_in_time = (1.0/double(params.input_work_rate))* ceil(double(params.packet_size)/double(params.input_bus_width));
   double arrival_time_unit = input_packet_size_in_time;
    // Output side
   double output_packet_size_in_time = (1.0/double(params.output_work_rate))* ceil(double(params.packet_size)/double(params.output_bus_width));
   double service_time_unit = output_packet_size_in_time;

 
   packet->start_time = get_current_time(); //TODO Start time 
   packet->fbe_time = get_current_time();
   packet->lbe_time = packet->fbe_time + input_packet_size_in_time;
     
   //std::cout << packet->fbe_time << "\t" << packet->lbe_time << std::endl;
   packet->qsize_we = topology->myq[q_id]->packets_in_queue; //TODO Start time 
   packet->dropped_pkts_we = topology->myq[q_id]->packets_dropped; //TODO Start time 
//std::cout<< packet->start_time << std::endl;  //TODO Printing Start time 
	    topology->myq[q_id]->enque(packet);
	    packet_pushed[q_id] ++;
   std::cout<< "a\t" << q_id << "\t" << get_current_time() <<"\t" << topology->myq[q_id]->bytes_in_queue << "\t" << topology->myq[q_id]->packets_in_queue<<"\t" << topology->myq[q_id]->bytes_dropped<<"\t" << topology->myq[q_id]->packets_dropped <<std::endl;
 

    if (packets_to_switch[q_id].size() == 0){
          double first_packet_leaving = packet->lbe_time + output_packet_size_in_time ;
          std::cout<< "****" <<first_packet_leaving << "=" << packet->lbe_time << "+" << output_packet_size_in_time << std::endl ;

           packet->departure_time = first_packet_leaving;
           packet->lbl_time = first_packet_leaving;
           packets_to_switch[q_id].push_back(packet); 
           no_of_packets_tracker[q_id]++;
           new_token_event = first_packet_leaving; 
           add_to_event_queue(new PacketServiceEvent(first_packet_leaving, packet));
    }
   
    else 
        
    {
        if (packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time > packet->fbe_time){
      
        //std::cout << packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time << ">"<< packet->fbe_time << std::endl;
	if(params.output_work_rate>=params.input_work_rate){
	std::cout<<"Cunningggggggggggggggggggggggggggggggggggggggg"<<std::endl;
        new_token_event = packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time + (packet->lbe_time - packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time) + output_packet_size_in_time; 
		}
	else
	{
	std::cout<<"Normallllllllllllllllllllllllllllllllllllllllll"<<std::endl;
	 new_token_event = packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time + output_packet_size_in_time ;
	}
        std::cout<<"*************************************************>" << std::endl;
            // std::cout<<"past "<<packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->departure_time << " > " << "get_current_time " << get_current_time() << std::endl; 
          }
     else{
              std::cout<<"-------------------------------------->" << std::endl;
              new_token_event = packet->fbe_time+ input_packet_size_in_time + output_packet_size_in_time ;
              //new_token_event = packet->fbe_time +  input_packet_size_in_time ;
          }

           packet->lbl_time = new_token_event ;
           std::cout<<"new_token_event "<< new_token_event<< std::endl;
           packets_to_switch[q_id].push_back(packet);
             
             
           no_of_packets_tracker[q_id]++;

           add_to_event_queue(new PacketServiceEvent(new_token_event, packet)); 
 

    }
 }

//---- Constructors and methods for PacketServiceEvent ------------

PacketServiceEvent::PacketServiceEvent(
	double time, 
	Packet *packet
	): event(PACKET_SERVICE_EVENT, time) {
    this->packet = packet;
	// Have to write my code here

	}

PacketServiceEvent::~PacketServiceEvent(){}


void PacketServiceEvent::process_event(){

   double output_packet_size_in_time = (1.0/double(params.output_work_rate))* ceil(double(params.packet_size)/double(params.output_bus_width));
    if(params.debug==1){
    std::cout<< get_current_time() <<" : <--- Packet Service Event "<< packet->seq_no << std::endl;
    }

    // Write code here
//	for (uint32_t i=0;i<params.schedule_rate;i++) {
    uint32_t q_id = packet->qid;
    if(topology->myq[q_id]->bytes_in_queue !=0) //&& tokens[q_id] != 0)
		{
			Packet* dequed_pkt = topology->myq[q_id]->deque();
            tokens[q_id]--;
            //std::cout <<" : <--- Check " << std::endl;
            dequed_pkt->end_time = get_current_time(); //TODO Start time
            dequed_pkt->lbl_time = get_current_time(); //TODO Start time
            dequed_pkt->fbl_time = get_current_time() - output_packet_size_in_time; //TODO Start time
	        dequed_pkt->qsize_wl =  topology->myq[q_id]->packets_in_queue;
            dequed_pkt->dropped_pkts_wl = topology->myq[q_id]->packets_dropped; //TODO Start time 

    
            packets_for_rx_stat[q_id].push_back(dequed_pkt);
            if(params.debug==1){
            std::cout<< get_current_time() <<" : <--- Packet dequed "<< dequed_pkt->seq_no << std::endl;
            }

            }
		else{	
			//std::cout<<"Queue is empty or no schedule tokens"<< std::endl;
            }
    //std::cout<< get_current_time() <<" : Packet Service Event "<< topology->myq[0]->getsize() << std::endl;

   //std::cout<< "s\t" << get_current_time() <<"\t" << topology->myq[0]->bytes_in_queue << "\t" << topology->myq[0]->pkt_drop<< std::endl;
   

   std::cout<< "d\t" << q_id << "\t"<< get_current_time() <<"\t" << topology->myq[q_id]->bytes_in_queue << "\t" << topology->myq[q_id]->packets_in_queue<<"\t" << topology->myq[q_id]->bytes_dropped<<"\t" << topology->myq[q_id]->packets_dropped <<std::endl;


    if(params.debug==1){

        topology->myq[0]->getsize();
        std::cout << " AT PACKET_SERVICE <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
    }
//	double tnext = time + 10;
//	add_to_event_queue(new PacketArrivalEvent(tnext, packet)); 
}

//---- Constructors and methods for PacketArrivalEvent ------------

PacketArrivalEvent::PacketArrivalEvent(
	double time, 
	Packet *packet
	): event(PACKET_ARRIVAL, time) {
    this->packet = packet;
	// Have to write my code here

	}

PacketArrivalEvent::~PacketArrivalEvent(){}


void PacketArrivalEvent::process_event(){

	// Write code here

	std::cout<< get_current_time() <<" Packet Arrival Event"<< std::endl;

    if(params.debug==1){

        std::cout << " AT PACKET_ARRIVAL <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
    packet->src->queue->enque(packet);
    //packet->src->queue->getsize();

}
    double tnext = time + 10;
	add_to_event_queue(new PacketDepartureEvent(tnext, packet)); 
}


//---- Constructors and methods for PacketDepartureEvent ------------

PacketDepartureEvent::PacketDepartureEvent(
	double time, 
	Packet *packet
	): event(PACKET_DEPARTED, time) {
	this->packet = packet;    
	// Have to write my code here

	}

PacketDepartureEvent::~PacketDepartureEvent(){}


void PacketDepartureEvent::process_event(){

	// Write code here
	std::cout<< get_current_time() <<" Packet Departure Event"<< std::endl; 
    if(params.debug==1){

        std::cout << " AT PACKET_DEPARTURE <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
       }

    Packet *popped_pkt;
    popped_pkt = packet->src->queue->deque();
    //packets_to_switch.push_back(popped_pkt);
    std::cout << "PACKETS_TO_SWITCH " <<packets_to_switch.size() << std::endl;
    //packet->src->queue->getsize();

    double tnext = time + 100;
    add_to_event_queue(new PacketArrivalAtSwitch(tnext, packet));
}

//---------- Constructor and methods for PacketArrivalAtSwitch event -------


PacketArrivalAtSwitch::PacketArrivalAtSwitch(
        double time,
        Packet *packet
        ): event(PACKET_ARRIVAL_AT_SWITCH, time) {
        this->packet = packet;
        // Have to write my code here

        }



PacketArrivalAtSwitch::~PacketArrivalAtSwitch(){}
void PacketArrivalAtSwitch::process_event(){
	// Have to write my code here

	std::cout<<get_current_time() <<" Packet Arrival at Switch  HHH "<< std::endl; 
    if(params.debug==1){

        std::cout << " AT PACKET_ARRIVAL_SWITCH <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      }


    std::cout << "PACKETS_TO_SWITCH_SIZE" << packets_to_switch.size() << std::endl;
//    for(uint32_t i=0;i<packets_to_switch.size();i++){
//    if(packets_to_switch.size() > 0){
      //  std::cout << "*" << std::endl;
      //  the_switch->queues[p->dst->id]
    Packet *popped_pkt;
    //popped_pkt = packets_to_switch[0];
    //std::cout << "MARK ---> "<< popped_pkt->size << std::endl;
    /*topology->switches[0]->queues[popped_pkt->dst->id]->enque(popped_pkt);
    std::cout <<topology->switches[0]->queues[popped_pkt->dst->id]->bytes_in_queue << std::endl;
    std::cout << "MARK ---> "<< popped_pkt->dst->id << std::endl;*/
    //}
   // }
    if(params.debug ==1){

    for(uint32_t i=0;i<10;i++){
   std::cout << "TOPOLOGY DETAILS " << topology->num_hosts << " VOQ " << i << " HAS "<< topology->switches[0]->queues[i]->bytes_in_queue << " PACKETS" << std::endl;
    }
    }
    double tnext = time + 10;
    add_to_event_queue(new SwitchArbitration(tnext, packet));
}

//---------- Constructor and methods for PacketArrivalAtSwitch event -------


SwitchArbitration::SwitchArbitration(
        double time,
        Packet *packet
        ): event(SWITCH_ARBITRATION, time) {
        this->packet = packet;
        // Have to write my code here

        }



SwitchArbitration::~SwitchArbitration(){}
void SwitchArbitration::process_event(){
	// Have to write my code here

	std::cout<<get_current_time() <<" Switch Arbitration"<< std::endl; 
    if(params.debug==1){

        std::cout << " AT SWITCH_ARBITRATION <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
}
    double tnext = time + 10;
    add_to_event_queue(new PacketDepartureFromSwitch(tnext, packet));
}




//---------- Constructor and methods for PacketDepartureFromSwitch event -------


PacketDepartureFromSwitch::PacketDepartureFromSwitch(
        double time,
        Packet *packet
        ): event(PACKET_DEPARTURE_FROM_SWITCH, time) {
        this->packet = packet;
        // Have to write my code here

        }



PacketDepartureFromSwitch::~PacketDepartureFromSwitch(){}
void PacketDepartureFromSwitch::process_event(){
	// Have to write my code here
	
    std::cout<< get_current_time() <<" Packet Departure from Switch"<< std::endl; 
    if(params.debug==1){

        std::cout << " AT PACKET_DEPARTURE_FROM_SWITCH <<< Packets sent from Host: "<< packet->src->id << " to Host: " <<packet->dst->id <<" with size "<< packet->size << ", sequence number " << packet->seq_no <<" priority "<< packet->pf_priority <<" and sending time of "<< packet->sending_time<<  std::endl;
      
    }
  
    //  double tnext = time + 10;
//    add_to_event_queue(new PacketArrivalAtSwitch(tnext, p1));

}


LoggingEvent::LoggingEvent(double time) : event(LOGGING, time){
    this->ttl = 1e10;
}

LoggingEvent::LoggingEvent(double time, double ttl) : event(LOGGING, time){
    this->ttl = ttl;
}

LoggingEvent::~LoggingEvent() {
}

void LoggingEvent::process_event() {

//std::vector <double> accumulated_latency (2);
//std::vector <double> accumulated_size (2);

    double latency = 0;
    double accumulated_size = 0;
    double accumulated_latency = 0; 
    double packets_dropped = 0;
    double accumulated_qsize_we = 0;
    double accumulated_qsize_wl = 0;

    //double min_qsize_we = 0;
    //double min_qsize_wl = 0;

    //double max_qsize_we = 0;
    //double max_qsize_wl = 0;

    std::cout << "\n************************************"<< std::endl;
    std::cout << "--- Received Packet log ---"<< std::endl;
    std::cout << "***********************************\n"<< std::endl;
    
   std::cout<<"sn : serial number"<<"\n"<< "size : packet size "<<"\n" <<"fbe : first bit of packet entering time"<<"\n" <<"lbe  : last bit of packet entering time "<<"\n" <<"fbl  : first bit of packet leaving time "<<"\n" <<"lbl : last bit of packet leaving time "<<"\n" <<"latency: lbl - lbe "<<"\n" <<std::endl;
    std::cout<< "sn" << "\t" << "size" <<"\t" << "fbe" << "\t" << "lbe" << "\t" << "fbl" << "\t" << "lbl"<< "\t"<< "latency"<< std::endl;

   for (int i=0;i<params.num_hosts;i++){
    std::cout << "log_rx_packets_start_queue "<<i <<std::endl;
         for (uint32_t j = 0; j < packets_for_rx_stat[i].size(); j++){
            latency =  packets_for_rx_stat[i][j]->lbl_time - packets_for_rx_stat[i][j]->lbe_time;
            accumulated_latency += latency;
            accumulated_size += packets_for_rx_stat[i][j]->size;

            std::cout<< packets_for_rx_stat[i][j]->seq_no <<"\t"<< packets_for_rx_stat[i][j]->size << "\t"<< packets_for_rx_stat[i][j]->fbe_time << "\t" << packets_for_rx_stat[i][j]->lbe_time << "\t" << packets_for_rx_stat[i][j]->fbl_time << "\t" << packets_for_rx_stat[i][j]->lbl_time <<"\t" << latency <<"\t"<< packets_for_rx_stat[i][j]->qsize_we <<"\t"<<packets_for_rx_stat[i][j]->qsize_wl << "\t"<< packets_for_rx_stat[i][j]->dropped_pkts_we<< "\t" << packets_for_rx_stat[i][j]->dropped_pkts_wl<<std::endl;

         }

    std::cout << "log_rx_packets_end_queue "<< i << std::endl;
    std::cout << "\n************************************"<< std::endl;
    std::cout << "--- Received Packet Statistics --- For Queue " << i << std::endl;
    std::cout << "***********************************\n"<< std::endl;
    std::cout<< "Number of packets sent : " << params.num_packets_to_run << std::endl;
    std::cout<< "Number of packets received : " << packets_for_rx_stat[i].size() << std::endl;
    std::cout<< "Average packets size : " << accumulated_size/packets_for_rx_stat[i].size() << std::endl;
    std::cout<< "Average packet latency : " << accumulated_latency/packets_for_rx_stat[i].size() << std::endl;
    std::cout<< "\n" << std::endl;

    accumulated_latency = 0;
    accumulated_size = 0;
 
    }

    
    
    /* 
    std::cout<< "sn" << "\t" << "size" <<"\t" << "start" << "\t" << "end" << "\t" << "lat" << "\t" << "qsize_we"<< "\t"<< "qsize_wl"<< std::endl;
    std::cout << "log_rx_packets_start"<< std::endl;
    for(it = packets_for_rx_stats.begin(); it != packets_for_rx_stats.end(); ++it) {
        std::cout << *it; ... 
        latency = (*it)->end_time - (*it)->start_time;
        accumulated_size += (*it)->size;
        accumulated_latency += latency;
	accumulated_qsize_we += (*it)->qsize_we;
	accumulated_qsize_wl += (*it)->qsize_wl;

        std::cout<< (*it)->seq_no << "\t"<< (*it)->size << "\t" << (*it)->start_time << "\t" << (*it)->end_time << "\t" << latency <<"\t" << (*it)->qsize_we <<"\t" << (*it)->qsize_wl <<std::endl;
    }

    std::cout << "log_rx_packets_end"<< std::endl;
	auto min_qsize_we = std::min_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end(),
						[](const Packet* a, const Packet* b )
                             {
                                 return a->qsize_we < b->qsize_we;
                             } );
	double min_entering = (*min_qsize_we)->qsize_we ;
       
	auto max_qsize_we = std::min_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end(),
						[](const Packet* a, const Packet* b )
                             {
                                 return a->qsize_we > b->qsize_we;
                             } );
	double max_entering = (*max_qsize_we)->qsize_we ;

	auto min_qsize_wl = std::min_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end(),
						[](const Packet* a, const Packet* b )
                             {
                                 return a->qsize_wl < b->qsize_wl;
                             } );
	double min_leaving = (*min_qsize_wl)->qsize_wl ;
       
	auto max_qsize_wl = std::min_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end(),
						[](const Packet* a, const Packet* b )
                             {
                                 return a->qsize_wl > b->qsize_wl;
                             } );
	double max_leaving = (*max_qsize_wl)->qsize_wl ;


	 //min_qsize_wl = std::min_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end());
        //max_qsize_we = std::max_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end());
        //max_qsize_wl = std::max_element(packets_for_rx_stats.begin(),packets_for_rx_stats.end());

   packets_dropped = params.num_packets_to_run - packets_for_rx_stats.size() ; 
    std::cout << "\n************************************"<< std::endl;
    std::cout << "--- Received Packet Statistics ---"<< std::endl;
    std::cout << "***********************************\n"<< std::endl;
    std::cout<< "Number of packets sent : " << params.num_packets_to_run << std::endl;
    std::cout<< "Number of packets received : " << packets_for_rx_stats.size() << std::endl;
    std::cout<< "Number of packets dropped : " << packets_dropped << std::endl;
    std::cout<< "Average packets size : " << accumulated_size/packets_for_rx_stats.size() << std::endl;
    std::cout<< "Average packet latency : " << accumulated_latency/packets_for_rx_stats.size() << std::endl;
    std::cout<< "Average queue size when packet enters : " << accumulated_qsize_we/packets_for_rx_stats.size() << std::endl;
     std::cout<< "Min queue size when packet enters : " << min_entering << std::endl;
     std::cout<< "Max queue size when packet enters : " << max_entering << std::endl;
    std::cout<< "Average queue size when packet leaves : " << accumulated_qsize_wl/packets_for_rx_stats.size() << std::endl;
     std::cout<< "Min queue size when packet leaves : " << min_leaving << std::endl;
     std::cout<< "Max queue size when packet leaves : " << max_leaving << std::endl;


    //std::cout<< "Min queue size when packet leaves : " << min_qsize_wl << std::endl;
    //std::cout<< "Max queue size when packet enters : " << max_qsize_we << std::endl;
    //std::cout<< "Max queue size when packet leaves : " << max_qsize_wl << std::endl;
    //std::cout<< "Average queue size when packet leaves : " << accumulated_qsize_wl/packets_for_rx_stats.size() << std::endl;
    //std::cout<< "Packets with following sequence numbers were dropped : "<< std::endl;
   // uint32_t found; */


//for(std::vector<Packet*>::iterator i = packets_for_tx_stats.begin(); i != packets_for_tx_stats.end(); ++i) {
//    for(std::vector<Packet*>::iterator j = packets_for_rx_stats.begin(); i != packets_for_rx_stats.end(); ++j) {
//    /* std::cout << *it; ... */
//    if ((*i)->seq_no ==(*j)->seq_no){
//        //found.assign((*i)->seq_no,2);
//        std::cout << "matching seq no : "<< (*i)->seq_no << std::endl ;
//        break;
//        }
//    }
//}


//    for(int i = 0; i < packets_for_tx_stats.size(); i++) {
//        for(int j = 0; j < packets_for_rx_stats.size(); j++) {
//      //  found = 0;
//            if (packets_for_rx_stats[j]->seq_no == packets_for_tx_stats[i]->seq_no){
//              //  found = 1;
//                found.assign(packets_for_rx_stats[j]->seq_no,1);
//                break;
//                 }
//            }
//        }
//    
//    for(int i = 0 ; i != found.size(); i++){
//            std::cout << "i: " << i  << " value is "<< found[i] << std::endl ;
//            //std::cout <<"*i: " << i << "value: " << found[i] << std::endl ;
//
//        }
//          // if( (j== packets_for_rx_stats.size()-1) && (i != packets_for_rx_stats[j]->seq_no)) {
          //  //std::cout<< "lost seqeuence number : " << i << std::endl;
          //  }
          //  else {
          //  std::cout<< "lost seqeuence number : " << i << std::endl;
          //  }
	//std::cout<<"Logging boss"<< std::endl; 
	// Write logging code here

}

StatsEvent::StatsEvent(double time): event(STATS, time){

}
StatsEvent::~StatsEvent(){

}
void StatsEvent::process_event() {


    std::cout << "***********************************************************"<< std::endl;
    std::cout << "----------------  Created PACKETS log  --------------------"<< std::endl;
    std::cout << "***********************************************************"<< std::endl;
    std::cout<< "sn" << "\t" << "size" <<"\t" << "start" << "\t" << "end" << "\t" << "lat" << "\t" << std::endl;
 
    //for(itt = packets_for_tx_stats.begin(); itt != packets_for_tx_stats.end(); ++itt) {
    for(uint32_t i=0;i<params.num_hosts;i++){	
    //for(uint32_t itt = 0; itt < packets_for_tx_stat[i].size(); ++itt ){
    for(uint32_t j = 0; j < packets_for_tx_stat[i].size(); j++ ){

        //std::cout<< i << j << std::endl;
        //std::cout<< packets_for_tx_stat[i][j]->seq_no << std::endl;
        std::cout<< packets_for_tx_stat[i][j]->seq_no << "\t"<< packets_for_tx_stat[i][j]->size << "\t" << packets_for_tx_stat[i][j]->start_time << "\t" << packets_for_tx_stat[i][j]->end_time << std::endl;

    }
    std::cout << "\n***************************************"<< std::endl;
    std::cout << "--- Created Packet Statistics ---"<< std::endl;
    std::cout << "***********************************\n"<< std::endl;
    std::cout<< "Number of packets created : " << packets_for_tx_stat[i].size() << std::endl;
    std::cout<< "\n" << std::endl;
 
}

}


TokenEvent::TokenEvent(
    double time, 
    Packet *packet
    ): event(TOKEN, time){
    this->packet = packet;
}

TokenEvent::~TokenEvent(){

}

void TokenEvent::process_event() {
    double q_id=packet->qid;
    tokens[q_id]++;
/*    std::cout << q_id << tokens[q_id] << std::endl;
    std::cout << "***********************************************************"<< std::endl;
    std::cout << "----------------  Token Event"<< "\t"<< time << std::endl;
    std::cout << "----------------  Token Event"<< "\t"<< get_current_time() << std::endl;
    std::cout << "***********************************************************"<< std::endl;
    for(uint32_t i=0;i<params.num_hosts;i++){
    std::cout <<tokens[i] <<"\t";

    }
    std::cout<<"\n"<< std::endl;*/
}


