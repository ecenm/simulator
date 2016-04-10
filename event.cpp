#include "event.h"
#include "factory.h"
#include "topology.h"
#include "params.h"
#include <algorithm> 


extern Topology* topology;
//extern std::priority_queue<event*, std::vector<event*>, EventComparator> event_queue;
extern std::priority_queue<event*, std::vector<event*>, EventComparator<event> > event_queue;
extern double current_time;
extern std::deque<event*> packet_arrivals;
extern std::deque<Packet*> packets_to_schedule;
extern std::vector<Packet*> packets_for_rx_stats;
extern std::vector<Packet*> packets_for_tx_stats;
std::vector<Queue::typenid > typenidvector;

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
	Packet *packet,
    Queue *queue
	): event(PACKET_PUSHING_EVENT, time) {
    this->packet = packet;
    this->queue = queue;
	// Have to write my code here

	}

PacketPushingEvent::~PacketPushingEvent(){}


void PacketPushingEvent::process_event(){

	// Write code here
    uint32_t current_packets_in_queue;
    uint32_t time_to_deque;

   double new_arrival_event = packet->starting_time;
   uint32_t node_type = queue->node_details.my_type;
   uint32_t node_id = queue->node_details.my_id;
   uint32_t q_id = queue->node_details.my_sub_id;
   //std::cout << queue->node_details.my_type <<"\t"<< node_id << "\t" << q_id << std::endl;
   uint32_t size = packets_to_switch[q_id].size();

   // Input side
   double input_packet_size_in_time = (1.0/double(queue->input_work_rate))* ceil(double(packet->size)/double(queue->input_bus_width));
   double arrival_time_unit = input_packet_size_in_time;
   //std::cout << "arrival_time_unit : "<< arrival_time_unit << std::endl;
    // Output side
   double output_packet_size_in_time = (1.0/double(queue->output_work_rate))* ceil(double(packet->size)/double(queue->output_bus_width));
   double service_time_unit = output_packet_size_in_time;
   //std::cout << "service_time_unit : "<< service_time_unit << std::endl;

/*    std::cout << "Inside packet creation event" << std::endl;
    std::cout<< topology->hosts[0]->queue->node_details.src_type << std::endl;
    std::cout<< topology->hosts[0]->queue->node_details.src_id   << std::endl;
    std::cout<< topology->hosts[0]->queue->node_details.dst_type << std::endl ;
    std::cout<< topology->hosts[0]->queue->node_details.dst_id   << std::endl;
    std::cout<< topology->hosts[0]->queue->node_details.my_type  << std::endl;
    std::cout<< topology->hosts[0]->queue->node_details.my_id    << std::endl;


    std::cout<< topology->hosts[1]->queue->node_details.src_type << std::endl;
    std::cout<< topology->hosts[1]->queue->node_details.src_id   << std::endl;
    std::cout<< topology->hosts[1]->queue->node_details.dst_type << std::endl ;
    std::cout<< topology->hosts[1]->queue->node_details.dst_id   << std::endl;
    std::cout<< topology->hosts[1]->queue->node_details.my_type  << std::endl;
    std::cout<< topology->hosts[1]->queue->node_details.my_id    << std::endl;


    std::cout<< topology->slinks[0]->node_details.src_type << std::endl;
    std::cout<< topology->slinks[0]->node_details.src_id   << std::endl ;
    std::cout<< topology->slinks[0]->node_details.dst_type << std::endl;
    std::cout<< topology->slinks[0]->node_details.dst_id   << std::endl;
    std::cout<< topology->slinks[0]->node_details.my_type  << std::endl;
    std::cout<< topology->slinks[0]->node_details.my_id   << std::endl;

    std::cout << "Exiting packet creation event" << std::endl;*/


 
   packet->start_time = get_current_time(); //TODO Start time 
   packet->fbe_time = get_current_time();
   packet->lbe_time = packet->fbe_time + input_packet_size_in_time;
   packet->qsize_we = queue->packets_in_queue; //TODO Start time 
   packet->dropped_pkts_we = queue->packets_dropped; //TODO Start time 

    //std::cout << packet->src->id << std::endl;
    //std::cout << queue->node_details.src_type << std::endl;
  
  if((queue->node_details.my_id == packet->src->id) && (queue->node_details.src_type == HOST)){
   //std::cout<<"Packet Pushing Event"<<std::endl; 
    //std::cout << "---------------------------------------->" << std::endl;
   packet->m_fbe_time = get_current_time();
   packet->m_lbe_time = packet->m_fbe_time + input_packet_size_in_time;
  // std::cout<<"\t"<<packet->m_fbe_time << "\t" << packet->m_lbe_time << std::endl;
 


   //std::cout << packet->fbe_time << "\t" << packet->lbe_time << std::endl;
//std::cout<< packet->start_time << std::endl;  //TODO Printing Start time 
	}
   else if((queue->node_details.my_id == packet->dst->id) && (queue->node_details.src_type == HOST)) {
	std::cout << "Approaching destination "<< std::endl;

	}

	    queue->enque(packet);
	    packet_pushed[q_id] ++;
   std::cout<< "a\t" << node_type << node_id << q_id << "\t" << get_current_time() <<"\t" << queue->bytes_in_queue << "\t" << queue->packets_in_queue<<"\t" << queue->bytes_dropped<<"\t" << queue->packets_dropped <<std::endl;
   //std::cout<< "a\t" << node_type << "\t" << node_id << "\t" << q_id << "\t" << get_current_time() <<"\t" << queue->bytes_in_queue << "\t" << queue->packets_in_queue<<"\t" << queue->bytes_dropped<<"\t" << queue->packets_dropped <<std::endl;
 
//std::cout <<"q_id "<<q_id << " packets to switch " << packets_to_switch[q_id].size() << std::endl;
    if (packets_to_switch[q_id].size() == 0){
          double first_packet_leaving = packet->lbe_time + output_packet_size_in_time ;
        // std::cout<< "\t"<< "****" <<first_packet_leaving << "=" << packet->lbe_time << "+" << output_packet_size_in_time << std::endl ;

           packet->departure_time = first_packet_leaving;
           packet->lbl_time = first_packet_leaving;
           packets_to_switch[q_id].push_back(packet); 
           no_of_packets_tracker[q_id]++;
           new_token_event = first_packet_leaving; 
           add_to_event_queue(new PacketServiceEvent(first_packet_leaving, packet, queue));
    }
   
    else 
        
    {
        if (packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time > packet->fbe_time){
      
        //std::cout << packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time << ">"<< packet->fbe_time << std::endl;
	if(params.output_work_rate>=params.input_work_rate){
	//std::cout<<"Cunningggggggggggggggggggggggggggggggggggggggg"<<std::endl;
        new_token_event = packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time + (packet->lbe_time - packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time) + output_packet_size_in_time; 
		}
	else
	{
	//std::cout<<"Normallllllllllllllllllllllllllllllllllllllllll"<<std::endl;
	 new_token_event = packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->lbl_time + output_packet_size_in_time ;
	//std::cout << new_token_event << std::endl;
	}
        //std::cout<<"*************************************************>" << std::endl;
            // std::cout<<"past "<<packets_to_switch[q_id][no_of_packets_tracker[q_id]-1]->departure_time << " > " << "get_current_time " << get_current_time() << std::endl; 
          }
     else{
              //std::cout<<"=============================================>" << std::endl;
              new_token_event = packet->fbe_time+ input_packet_size_in_time + output_packet_size_in_time ;
              //new_token_event = packet->fbe_time +  input_packet_size_in_time ;
		std::cout << new_token_event << std::endl;
          }

           packet->lbl_time = new_token_event ;
           //std::cout<< "\t"<<"new_token_event "<< new_token_event<< std::endl;
           packets_to_switch[q_id].push_back(packet);
             
             
           no_of_packets_tracker[q_id]++;

           add_to_event_queue(new PacketServiceEvent(new_token_event, packet, queue)); 
 

    }
 }

//---- Constructors and methods for PacketServiceEvent ------------

PacketServiceEvent::PacketServiceEvent(
	double time, 
	Packet *packet,
    Queue *queue
	): event(PACKET_SERVICE_EVENT, time) {
    this->packet = packet;
    this->queue = queue;
	// Have to write my code here

	}

PacketServiceEvent::~PacketServiceEvent(){}


void PacketServiceEvent::process_event(){

    uint32_t node_type = queue->node_details.my_type;
    uint32_t node_id = queue->node_details.my_id;
    uint32_t q_id = queue->node_details.my_sub_id; 
    uint32_t finish = 0;
    Queue::typenid hostdetails ;
// std::cout << "Packet Service Event"<< std::endl;
   double output_packet_size_in_time = (1.0/double(params.output_work_rate))* ceil(double(params.packet_size)/double(params.output_bus_width));
    if(params.debug==1){
    }
    
    if(queue->bytes_in_queue !=0) //&& tokens[q_id] != 0)
		{
			Packet* dequed_pkt = queue->deque();
		
		   if(packet->dst->id == queue->node_details.my_id) {
                     packet->m_lbl_time = get_current_time();
                     packet->m_fbl_time = get_current_time() - output_packet_size_in_time;
		     finish = 1;
			}

                     packet->end_time = get_current_time(); //TODO Start time
                     packet->lbl_time = get_current_time(); //TODO Start time
                     packet->fbl_time = get_current_time() - output_packet_size_in_time; 
	                 packet->qsize_wl =  topology->hosts[node_id]->queue[q_id]->packets_in_queue;
                     packet->dropped_pkts_wl =topology->hosts[node_id]->queue[q_id]->packets_dropped;  
		     packets_for_rx_stat[q_id].push_back(packet);

   std::cout<< "d\t" << node_type << node_id << q_id << "\t"<< get_current_time()<<"\t" << queue->bytes_in_queue << "\t" << queue->packets_in_queue<<"\t" << queue->bytes_dropped<<"\t" << queue->packets_dropped <<std::endl;
   //std::cout<< "d\t" << node_type << "\t" << node_id << "\t" << q_id << "\t"<< get_current_time()<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_in_queue << "\t" << topology->hosts[node_id]->queue[q_id]->packets_in_queue<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_dropped<<"\t" << topology->hosts[node_id]->queue[q_id]->packets_dropped <<std::endl;

  // std::cout << queue->node_details.my_type <<"\t"<< node_id << "\t" << q_id << std::endl;
   //std::cout << node_id << "\t" << q_id << std::endl;
                       //std::cout << "\t" << "packet->dst->id : " << packet->dst->id <<" hostdetails.dst_id : "<< hostdetails.dst_id << " hostdetails.dst_type : "<< hostdetails.dst_type << std::endl;
		    //std::cout <<"\t" <<dequed_pkt->dst->id << "\t"<< queue->id<< std::endl;
		   if (finish == 0){
                        if(node_type == HOST){
                            //std::cout << "############### Host ###################" << std::endl; 
                     	hostdetails = topology->hosts[node_id]->queue[q_id]->node_details;
                        //std::cout <<" Host "<< hostdetails.my_type <<"\t"<< hostdetails.my_id << "\t" << hostdetails.my_sub_id << std::endl;
                        add_to_event_queue(new FindNextHopEvent(get_current_time(),dequed_pkt, hostdetails));
                       //typenidvector.push_back(hostdetails); 
                        }
                        else if(node_type == EPS) {
                            //std::cout << "############### EPS ###################" << std::endl; 
                     	hostdetails = topology->switches[node_id]->queues[q_id]->node_details;
                        //std::cout <<"EPS " <<hostdetails.my_type <<"\t"<< hostdetails.my_id << "\t" << hostdetails.my_sub_id << std::endl;
                    add_to_event_queue(new FindNextHopEvent(get_current_time(),dequed_pkt, hostdetails));
                        }
    
                        else if(node_type == LINK) {
                            std::cout << "############## Link ##################" << std::endl;
                        }
                }
         }
}
                    /*std::cout<< "\t" << "Packet destined for different destinatione"<< std::endl;
                         std::cout << "\t" << "hostdetails.my_id" << hostdetails.my_id << std::endl;
                       std::cout << "\t" << "hostdetails.my_type" << hostdetails.my_type << std::endl;
                       std::cout << "\t" << "hostdetails.src_type" << hostdetails.src_type << std::endl;
                       std::cout << "\t" << "hostdetails.src_id" << hostdetails.src_id << std::endl;
                       std::cout << "\t" << "hostdetails.dst_type" << hostdetails.dst_type << std::endl;
                       std::cout << "\t" << "hostdetails.dst_id" << hostdetails.dst_id << std::endl;*/

                     /*  std::cout << "\t" << "typenidvector[0].my_id"   << typenidvector[0].my_id << std::endl;
                       std::cout << "\t" << "typenidvector[0].my_type" << typenidvector[0].my_type << std::endl;
                       std::cout << "\t" << "typenidvector[0].src_type"<< typenidvector[0].src_type << std::endl;
                       std::cout << "\t" << "typenidvector[0].src_id"  << typenidvector[0].src_id << std::endl;
                       std::cout << "\t" << "typenidvector[0].dst_type"<< typenidvector[0].dst_type << std::endl;
                       std::cout << "\t" << "typenidvector[0].dst_id"  << typenidvector[0].dst_id << std::endl;*/

              /*     if((dequed_pkt->dst->id != queue->src->id)&& (queue->dst->type != HOST)) 
                {
                    std::cout<<"Packet destined for different destinatione"<< std::endl;
		            uint32_t next_hop = topology->get_next_hop(packet,queue);
                    
                    std::cout << "next_hop @ service event is " << next_hop << std::endl;
                    add_to_event_queue(new PacketEnteringLinkEvent(get_current_time(),dequed_pkt,next_hop));
                    //add_to_event_queue(new PacketPushingEvent(get_current_time()+pd+td,dequed_pkt, topology->slinks[next_hop]));
		   
             
               if((dequed_pkt->dst->id == queue->id) && (queue->dst->type == HOST)){ 
                     dequed_pkt->m_lbl_time = get_current_time();
                     dequed_pkt->m_fbl_time = get_current_time() - output_packet_size_in_time;
                     }
                }
                //if((dequed_pkt->dst->id != queue->id) && (queue->dst->type != HOST))
               else

                {

                tokens[q_id]--;
                //std::cout <<" : <--- Check " << std::endl;
                dequed_pkt->end_time = get_current_time(); //TODO Start time
                dequed_pkt->lbl_time = get_current_time(); //TODO Start time
                dequed_pkt->fbl_time = get_current_time() - output_packet_size_in_time; //TODO Start time
	            dequed_pkt->qsize_wl =  queue->packets_in_queue;
                dequed_pkt->dropped_pkts_wl = queue->packets_dropped; //TODO Start time 
                dequed_pkt->m_lbl_time = get_current_time();
                dequed_pkt->m_fbl_time = get_current_time() - output_packet_size_in_time;

               if((dequed_pkt->dst->id == queue->id) && (queue->dst->type == HOST)){ 
                     dequed_pkt->m_lbl_time = get_current_time();
                     dequed_pkt->m_fbl_time = get_current_time() - output_packet_size_in_time;
                     }
    
                 packets_for_rx_stat[q_id].push_back(dequed_pkt);
                    std::cout<< get_current_time() <<" : <--- Packet dequed "<< dequed_pkt->seq_no << std::endl;
                if(params.debug==1){
                    std::cout<< get_current_time() <<" : <--- Packet dequed "<< dequed_pkt->seq_no << std::endl;
                     }

                    }
        }

		else{	
			std::cout<<"Queue is empty or no schedule tokens"<< std::endl;
            }*/
  



//---- Constructors and methods for FindNextHopEvent ------------

FindNextHopEvent::FindNextHopEvent(
	double time, 
	Packet *packet,
	Queue::typenid node_details
	): event(NEXT_HOP_EVENT, time) {
    this->packet = packet;
    local_node_details = node_details;
    //this->node_details = node_details;
    //std::cout <<"Nodeeeeeeeeeeeeeeeeeeeee details " << node_details.dst_type << std::endl ;
//    Queue::typenid dup = node_details ;
    //std::cout <<"Nodeeeeeeeeeeeeeeeeeeeee details local " << local_node_details.dst_type << std::endl ;
//    this->node_details = node_details;
	}

FindNextHopEvent::~FindNextHopEvent(){}

void FindNextHopEvent::printdetails(){
    //std::cout <<"Nodeeeeeeeeeeeeeeeeeeeee details local " << local_node_details.src_type << std::endl ;

}

void FindNextHopEvent::process_event(){
/*                       std::cout << "\t" << "hostdetails.my_id" << hostdetails.my_id << std::endl;
                       std::cout << "\t" << "hostdetails.my_type" << hostdetails.my_type << std::endl;
                       std::cout << "\t" << "hostdetails.src_type" << hostdetails.src_type << std::endl;
                       std::cout << "\t" << "hostdetails.src_id" << hostdetails.src_id << std::endl;
                       std::cout << "\t" << "hostdetails.dst_type" << hostdetails.dst_type << std::endl;
                       std::cout << "\t" << "hostdetails.dst_id" << hostdetails.dst_id << std::endl;
*/


	// Write code here
   uint32_t q_id = packet->qid;
   //FindNextHopEvent::printdetails(local_node_details);
   //std::cout << "Find Next Hop Event" << std::endl;
    //std::cout << "\t" << "packet->dst->id : " << packet->dst->id <<" local_node_details.dst_id : "<< local_node_details.dst_id << " local_node_details.dst_type : "<< local_node_details.dst_type << " local_node_details.src_id : "<< local_node_details.src_id << " local_node_details.src_type : "<< local_node_details.src_type << std::endl;
   double output_packet_size_in_time = (1.0/double(params.output_work_rate))* ceil(double(params.packet_size)/double(params.output_bus_width));
   // std::cout << "\t" << "packet->dst->id : " << packet->dst->id <<" node_details.dst_id : "<< local_node_details.dst_id << " node_details.dst_type : "<< local_node_details.dst_type << std::endl;
    if((packet->dst->id == local_node_details.dst_id) && (local_node_details.dst_type == HOST)){ //TODO change 
                //std::cout << "\t" << "MACHIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" << std::endl;
	add_to_event_queue(new PacketPushingEvent(get_current_time(), packet, topology->hosts[packet->dst->id]->queue[0]));
                    }  

    else 
            {
                   // std::cout<< "\t" << "Packet destined for different"<< std::endl;
		            Queue::typenid next_hop = topology->get_next_hop(get_current_time(),packet,local_node_details);
                       /* std::cout << "\t"<< "next_hop.my_id"   << next_hop.my_id << std::endl;
                       std::cout << "\t" << "next_hop.my_type" << next_hop.my_type << std::endl;
                       std::cout << "\t" << "next_hop.src_type"<< next_hop.src_type << std::endl;
                       std::cout << "\t" << "next_hop.src_id"  << next_hop.src_id << std::endl;
                       std::cout << "\t" << "next_hop.dst_type"<< next_hop.dst_type << std::endl;
                       std::cout << "\t" << "next_hop.dst_id"  << next_hop.dst_id << std::endl;*/

                   
                    //std::cout << "\t" << "next_hop @ service event is " << next_hop << std::endl;
                    if(next_hop.my_type == LINK){
                    //std::cout<<"******@@@@@@@@@@@@@@@@^^^^^^^^^^^^^^^^"<< std::endl;
                    add_to_event_queue(new PacketEnteringLinkEvent(get_current_time(),packet,next_hop));
                    }

                    else if(next_hop.my_type == EPS){
                    //std::cout<<"******++++++++++++++++^^^^^^^^^^^^^^^^"<< std::endl;
                    add_to_event_queue(new PacketEnteringSwitchEvent(get_current_time(),packet,next_hop)); 
                    }

                    //add_to_event_queue(new PacketPushingEvent(get_current_time()+pd+td,dequed_pkt, topology->slinks[next_hop]));
	              }	   
             
     //else if((packet->dst->id == queue->id) && (queue->dst->type == NULL)){ // destination 
/*               if((dequed_pkt->dst->id == queue->id) && (queue->dst->type == HOST)){ 
                     dequed_pkt->m_lbl_time = get_current_time();
                     dequed_pkt->m_fbl_time = get_current_time() - output_packet_size_in_time;
                     }
    
                 packets_for_rx_stat[q_id].push_back(dequed_pkt);
                    std::cout<< get_current_time() <<" : <--- Packet dequed "<< dequed_pkt->seq_no << std::endl;


     }*/
   //std::cout<< "d\t" << q_id << "\t"<< get_current_time() <<"\t" << queue->bytes_in_queue << "\t" << queue->packets_in_queue<<"\t" << queue->bytes_dropped<<"\t" << queue->packets_dropped <<std::endl;



}



//---- Constructors and methods for PacketEnteringLinkEvent ------------

PacketEnteringLinkEvent::PacketEnteringLinkEvent(
	double time, 
	Packet *packet,
    	Queue::typenid next_hop
	): event(PACKET_ARRIVAL_AT_LINK, time) {
    this->packet = packet;
    local_node_details = next_hop;
	// Have to write my code here

    //std::cout <<"Nodeeeeee*************************** details " << next_hop.dst_type << std::endl ;
	}

PacketEnteringLinkEvent::~PacketEnteringLinkEvent(){}


void PacketEnteringLinkEvent::process_event(){

	// Write code here
   //Queue::typenid next_hop = next_hop ;
   uint32_t node_type = local_node_details.my_type;
   uint32_t node_id = local_node_details.my_id;
   uint32_t q_id = local_node_details.my_sub_id;
   /*std::cout << "Packet Entering Link Event" << std::endl;
   std::cout << "\t" << "next_hop.my_id" << local_node_details.my_id << std::endl;
   std::cout << "\t" << "next_hop.src_type" << local_node_details.src_type << std::endl;
   std::cout << "\t" << "next_hop.src_id" << local_node_details.src_id << std::endl;
   std::cout << "\t" << "next_hop.dst_type" << local_node_details.dst_type << std::endl;
   std::cout << "\t" << "next_hop.dst_id" << local_node_details.dst_id << std::endl;*/
   double linkdelay = topology->slinks[q_id]->td;

   //std::cout << "\t" << "linkdelay" << linkdelay << std::endl;

   std::cout<< "le\t" << node_type << node_id << q_id << "\t"<< get_current_time() <<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_in_queue << "\t" << topology->hosts[node_id]->queue[q_id]->packets_in_queue<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_dropped<<"\t" << topology->hosts[node_id]->queue[q_id]->packets_dropped <<std::endl;
   //std::cout<< "le\t" << node_type <<"\t" << node_id << "\t" << q_id << "\t"<< get_current_time() <<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_in_queue << "\t" << topology->hosts[node_id]->queue[q_id]->packets_in_queue<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_dropped<<"\t" << topology->hosts[node_id]->queue[q_id]->packets_dropped <<std::endl;

   // std::cout << "----------------------------------------------------------------------------" << std::endl;
   //add_to_event_queue(new PacketDepartingLinkEvent(get_current_time()+topology->slinks[next_hop.my_id]->totd, packet,next_hop ));
   add_to_event_queue(new PacketDepartingLinkEvent(get_current_time()+10, packet, local_node_details ));
}


//---- Constructors and methods for PacketDepartingLinkEvent ------------

PacketDepartingLinkEvent::PacketDepartingLinkEvent(
	double time, 
	Packet *packet,
	Queue::typenid next_hop
	): event(PACKET_DEPARTED_FROM_LINK, time) {
	this->packet = packet;
	local_node_details = next_hop;    
	// Have to write my code here

	}

PacketDepartingLinkEvent::~PacketDepartingLinkEvent(){}


void PacketDepartingLinkEvent::process_event(){

    //Queue::typenid local_node_details = next_hop ;
     uint32_t node_type = local_node_details.my_type;
     uint32_t node_id = local_node_details.my_id;
     uint32_t q_id = local_node_details.my_sub_id;
           

   //std::cout << "Packet Departing Link Event" << std::endl;
     std::cout<< "ll\t" << node_type << node_id << q_id << "\t"<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_in_queue << "\t" << topology->hosts[node_id]->queue[q_id]->packets_in_queue<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_dropped<<"\t" << topology->hosts[node_id]->queue[q_id]->packets_dropped <<std::endl;
     //std::cout<< "ll\t" << node_type <<"\t" << node_id << "\t" << q_id << "\t"<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_in_queue << "\t" << topology->hosts[node_id]->queue[q_id]->packets_in_queue<<"\t" << topology->hosts[node_id]->queue[q_id]->bytes_dropped<<"\t" << topology->hosts[node_id]->queue[q_id]->packets_dropped <<std::endl;
  
	add_to_event_queue(new FindNextHopEvent(get_current_time(), packet, local_node_details)); 

}

//---------- Constructor and methods for PacketEnteringSwitchEvent event -------


PacketEnteringSwitchEvent::PacketEnteringSwitchEvent(
        double time,
        Packet *packet,
        Queue::typenid next_hop
        ): event(PACKET_ARRIVAL_AT_SWITCH, time) {
        this->packet = packet;
        local_node_details = next_hop;

        }

PacketEnteringSwitchEvent::~PacketEnteringSwitchEvent(){}
void PacketEnteringSwitchEvent::process_event(){

    //std::cout<< get_current_time() <<" Packets Entering the Switch"<< std::endl; 
    uint32_t node_id = local_node_details.my_id;
    uint32_t q_id = local_node_details.my_sub_id;

    std::cout<< node_id << "\t" << q_id << std::endl; 
	add_to_event_queue(new PacketPushingEvent(get_current_time(), packet, topology->switches[node_id]->queues[q_id]));
}

//---------- Constructor and methods for PacketDepartingSwitchEvent event -------


PacketDepartingSwitchEvent::PacketDepartingSwitchEvent(
        double time,
        Packet *packet,
        Queue::typenid next_hop
        ): event(PACKET_DEPARTURE_FROM_SWITCH, time) {
        this->packet = packet;
        local_node_details = next_hop;

        }



PacketDepartingSwitchEvent::~PacketDepartingSwitchEvent(){}
void PacketDepartingSwitchEvent::process_event(){
	// Have to write my code here
	
    std::cout<< get_current_time() <<" Packet Departure from Switch"<< std::endl; 
    }



//---------- Constructor and methods for Switch Arbitration event -------


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
//    add_to_event_queue(new PacketDepartingSwitchEvent(tnext, packet));
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

    std::cout << "events_stop 1" << std::endl; 
     
    std::cout << "\n************************************"<< std::endl;
    std::cout << "--- Received Packet log (MAIN) ---"<< std::endl;
    std::cout << "***********************************\n"<< std::endl;
    
   std::cout<<"sn : serial number"<<"\n"<< "size : packet size "<<"\n" <<"fbe : first bit of packet entering time"<<"\n" <<"lbe  : last bit of packet entering time "<<"\n" <<"fbl  : first bit of packet leaving time "<<"\n" <<"lbl : last bit of packet leaving time "<<"\n" <<"latency: lbl - lbe "<<"\n" <<std::endl;
    std::cout<< "sn" << "\t" << "size" <<"\t" << "fbe" << "\t" << "lbe" << "\t" << "fbl" << "\t" << "lbl"<< "\t"<< "latency"<< std::endl;

   //for (int i=0;i<params.num_hosts;i++){
   for (int i=0;i<2;i++){
    std::cout << "log_rx_packets_start_queue "<<i <<std::endl;
         for (uint32_t j = 0; j < packets_for_rx_stat[i].size(); j++){
            latency =  packets_for_rx_stat[i][j]->m_lbl_time - packets_for_rx_stat[i][j]->m_lbe_time;
            //std::cout << latency <<  packets_for_rx_stat[i][j]->m_lbl_time << packets_for_rx_stat[i][j]->m_lbe_time<< std::endl;
            accumulated_latency += latency;
            accumulated_size += packets_for_rx_stat[i][j]->size;

            std::cout<< packets_for_rx_stat[i][j]->seq_no <<"\t"<< packets_for_rx_stat[i][j]->size << "\t"<< packets_for_rx_stat[i][j]->m_fbe_time << "\t" << packets_for_rx_stat[i][j]->m_lbe_time << "\t" << packets_for_rx_stat[i][j]->m_fbl_time << "\t" << packets_for_rx_stat[i][j]->m_lbl_time <<"\t" << latency << "\t"<< packets_for_rx_stat[i][j]->qsize_we <<"\t"<<packets_for_rx_stat[i][j]->qsize_wl << "\t"<< packets_for_rx_stat[i][j]->dropped_pkts_we<< "\t" << packets_for_rx_stat[i][j]->dropped_pkts_wl<<std::endl;

//std::cout<< packets_for_rx_stat[i][j]->seq_no <<"\t"<< packets_for_rx_stat[i][j]->size << "\t"<< packets_for_rx_stat[i][j]->fbe_time << "\t" << packets_for_rx_stat[i][j]->lbe_time << "\t" << packets_for_rx_stat[i][j]->fbl_time << "\t" << packets_for_rx_stat[i][j]->lbl_time <<"\t" << latency <<"\t"<< packets_for_rx_stat[i][j]->qsize_we <<"\t"<<packets_for_rx_stat[i][j]->qsize_wl << "\t"<< packets_for_rx_stat[i][j]->dropped_pkts_we<< "\t" << packets_for_rx_stat[i][j]->dropped_pkts_wl<<std::endl;
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
}

//double min_qsize_we = 0;
    //double min_qsize_wl = 0;

    //double max_qsize_we = 0;
    //double max_qsize_wl = 0;

    /*std::cout << "\n************************************"<< std::endl;
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

         }*/


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

std::cout << "events_start 1" << std::endl;
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


