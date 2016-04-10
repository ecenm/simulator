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

   // Create hosts with their queues
    
        hosts.push_back(Factory::get_host(0,0, params.queue_size, params.queue_type, params.host_type,0,0,32,10,32,10,params.nqph)); // TODO Parameterize Rate and drop rate
    
        hosts.push_back(Factory::get_host(1,0, params.queue_size, params.queue_type, params.host_type,0,1,32,10,32,4,params.nqph)); // TODO Parameterize Rate and drop rate

        for (uint32_t i=0; i < num_myq; i++) {
            for (uint32_t j=0; j < params.nqph; j++) { // nqph - number of queues per host
            std::cout<< "host stats  : "<< i << "\t" << j << std::endl;
            hosts[i]->queue[j]->printdetails();
            }
    }

    // Create EPS and pushing it to switch
    Eps* eps1 = new Eps(0,params.nqps,0,1); // nqps - number of queues per switch
    switches.push_back(eps1);

    std::cout << "num of queues in switches" << switches.size() << std::endl;

   for (uint32_t j=0; j < params.nqps; j++) {
    switches[0]->queues[j]->printdetails();
    //std::cout << switches[0]->queues[j]->node_details.my_sub_id << std::endl;
   }
  
	  
    // Create simple links and sinks 

   for (uint32_t i=0; i < 2; i++) { // need to parameterize number of links 

    SimpleLink* sl = new SimpleLink(i);
    slinks.push_back(sl); // TODO make generic

    
   }

   
   for (uint32_t i= 0; i< num_myq/2; i++) {
    SimpleSink* si = new SimpleSink(i); //sink
    sinks.push_back(si); 
   
   }


/*   for (uint32_t i=0; i < num_myq-1; i++) {
    SimpleLink *sl = slinks[i];
    sl->set_src_dst(hosts[i]->queue[i]->node_details, hosts[i+1]->queue[i]->node_details);

    SimpleSink *si = sinks[i];
    si->set_src_dst(hosts[i+1]->queue[i]->node_details, sinks[i]->node_details);

   }*/

    // host0 q0 ------sl0------- sw0 q0
    //SimpleLink *sl0 = slinks[0];
    slinks[0]->set_src_dst(hosts[0]->queue[0]->node_details, switches[0]->queues[0]->node_details);

    // sw0 q1 ------sl1------- host1 q0
    //SimpleLink *sl1 = slinks[1];
    slinks[1]->set_src_dst(switches[0]->queues[1]->node_details,hosts[1]->queue[0]->node_details);

    sinks[0]->set_src_dst(hosts[1]->queue[0]->node_details, sinks[0]->node_details);



	std::cout << """"""""""""""""""""""""""""""""""""""""""<< std::endl;
 
   // hosts[0]->node_details->src_type = 7 ;
   hosts[0]->queue[0]->set_src_dst(hosts[0]->queue[0]->node_details, slinks[0]->node_details);
   hosts[1]->queue[0]->set_src_dst(slinks[1]->node_details, hosts[1]->queue[0]->node_details);

   switches[0]->queues[0]->set_src_dst(slinks[0]->node_details, switches[0]->queues[1]->node_details);
   switches[0]->queues[1]->set_src_dst(switches[0]->queues[0]->node_details, slinks[1]->node_details);
/*    slinks[0]->node_details.src_type = 22 ;
std::cout << slinks[0]->node_details.src_type << std::endl ; 
    hosts[0]->queue->node_details.src_type = 7 ;
std::cout << hosts[0]->queue->node_details.src_type << std::endl ; */

    //hosts[0]->queue->node_details.src_type = 7 ;
    //hosts[0]->queue->node_details.src_id   = 0 ;
    //hosts[0]->queue->node_details.dst_type = 22 ;
    //hosts[0]->queue->node_details.dst_id   = 0 ;
    //hosts[0]->queue->node_details.my_type  = 7 ;
    //hosts[0]->queue->node_details.my_id    = 0 ;
   std::cout << "*******************\n"<< std::endl;
   std::cout << "Configuration Details\n" << std::endl;
   std::cout << "*******************\n"<< std::endl;

   std::cout<< "[Node id]" <<  "\t"<<"src_type" << "\t"<<"src_id"   <<  "\t"<<"dst_type" <<  "\t"<<"dst_id"   <<  "\t"<<"my_type"  <<  "\t"<<"my_id"    << std::endl;
std::cout<<"[Host 0]" <<  "\t"<<hosts[0]->queue[0]->node_details.src_type <<  "\t"<<hosts[0]->queue[0]->node_details.src_id << "\t"<<hosts[0]->queue[0]->node_details.src_sub_id  <<  "\t"<<hosts[0]->queue[0]->node_details.dst_type <<  "\t"<<hosts[0]->queue[0]->node_details.dst_id   << "\t"<<hosts[0]->queue[0]->node_details.dst_sub_id   << "\t"<<hosts[0]->queue[0]->node_details.my_type  <<  "\t"<<hosts[0]->queue[0]->node_details.my_id    <<   "\t"<<hosts[0]->queue[0]->node_details.my_sub_id  <<  std::endl;

    // slinks[0]->node_details.src_type = 7 ; 
   // slinks[0]->node_details.src_id   = 0 ;
   // slinks[0]->node_details.dst_type = 7 ;
   // slinks[0]->node_details.dst_id   = 1 ;
   // slinks[0]->node_details.my_type  = 22 ;
   // slinks[0]->node_details.my_id    = 0 ;

    std::cout<< "[Link 0]" <<  "\t"<< slinks[0]->node_details.src_type <<  "\t"<<slinks[0]->node_details.src_id  <<  "\t" << slinks[0]->node_details.src_sub_id <<  "\t"<<slinks[0]->node_details.dst_type <<  "\t"<<slinks[0]->node_details.dst_id  << "\t"<< slinks[0]->node_details.dst_sub_id  <<  "\t"<<slinks[0]->node_details.my_type  <<  "\t"<<slinks[0]->node_details.my_id  << "\t"<<slinks[0]->node_details.my_sub_id << std::endl;


    std::cout<< "[Swit 0]" <<  "\t"<< switches[0]->queues[0]->node_details.src_type <<  "\t"<<switches[0]->queues[0]->node_details.src_id  <<  "\t" << switches[0]->queues[0]->node_details.src_sub_id <<  "\t"<<switches[0]->queues[0]->node_details.dst_type <<  "\t"<<switches[0]->queues[0]->node_details.dst_id  << "\t"<< switches[0]->queues[0]->node_details.dst_sub_id  <<  "\t"<<switches[0]->queues[0]->node_details.my_type  <<  "\t"<<switches[0]->queues[0]->node_details.my_id  << "\t"<<switches[0]->queues[0]->node_details.my_sub_id << std::endl;


    std::cout<< "[Swit 1]" <<  "\t"<< switches[0]->queues[1]->node_details.src_type <<  "\t"<<switches[0]->queues[1]->node_details.src_id  <<  "\t" << switches[0]->queues[1]->node_details.src_sub_id <<  "\t"<<switches[0]->queues[1]->node_details.dst_type <<  "\t"<<switches[0]->queues[1]->node_details.dst_id  << "\t"<< switches[0]->queues[1]->node_details.dst_sub_id  <<  "\t"<<switches[0]->queues[1]->node_details.my_type  <<  "\t"<<switches[0]->queues[1]->node_details.my_id  << "\t"<<switches[0]->queues[1]->node_details.my_sub_id << std::endl;
    //hosts[1]->queue->node_details.src_type = 22 ;
    //hosts[1]->queue->node_details.src_id   = 0 ;
    //hosts[1]->queue->node_details.dst_type = 7 ;
    //hosts[1]->queue->node_details.dst_id   = 1 ;
   // hosts[1]->queue->node_details.my_type  = 7 ;
   // hosts[1]->queue->node_details.my_id    = 1 ;

    std::cout<< "[Host 1]" <<  "\t"<<hosts[1]->queue[0]->node_details.src_type <<  "\t"<<hosts[1]->queue[0]->node_details.src_id   << "\t"<<hosts[1]->queue[0]->node_details.src_sub_id  << "\t"<<hosts[1]->queue[0]->node_details.dst_type <<  "\t"<<hosts[1]->queue[0]->node_details.dst_id   << "\t"<<hosts[1]->queue[0]->node_details.dst_sub_id << "\t"<<hosts[1]->queue[0]->node_details.my_type  <<  "\t"<<hosts[1]->queue[0]->node_details.my_id << "\t"<<hosts[1]->queue[0]->node_details.my_sub_id   << std::endl;

std::cout<< "[Sink 0]" <<  "\t"<<sinks[0]->node_details.src_type <<  "\t"<<sinks[0]->node_details.src_id   <<  "\t"<<sinks[0]->node_details.src_sub_id << "\t"<<sinks[0]->node_details.dst_type <<  "\t"<<sinks[0]->node_details.dst_id   <<  "\t"<<sinks[0]->node_details.dst_sub_id  << "\t"<<sinks[0]->node_details.my_type  <<  "\t"<<sinks[0]->node_details.my_id <<"\t"<< sinks[0]->node_details.my_sub_id << std::endl;


    hosts[0]->queue[0]->printdetails();
    
    hosts[1]->queue[0]->printdetails();
	
    sinks[0]->printdetails();

	std::cout << ".............................."<< std::endl;

}

Queue::typenid SimpleTopology::get_next_hop(double time, Packet *p, Queue::typenid node_details) {

/*	std::cout<<"\t=================Getting next hop ===============>"<<std::endl;

                       std::cout << "\t"<<  "node_details.my_id"   << node_details.my_id << std::endl;
                       std::cout << "\t" << "node_details.my_type" << node_details.my_type << std::endl;
                       std::cout << "\t" << "node_details.src_type"<< node_details.src_type << std::endl;
                       std::cout << "\t" << "node_details.src_id"  << node_details.src_id << std::endl;
                       std::cout << "\t" << "node_details.dst_type"<< node_details.dst_type << std::endl;
                       std::cout << "\t" << "node_details.dst_id"  << node_details.dst_id << std::endl;
	     std::cout<< "\t" <<"packet_src is : "<< p->src->id << " packet_dst is : "<< p->dst->id <<std::endl;*/
    //std::cout<< "\t" <<"q_src_id : "<< q->src->id << " next hop is : " << q->dst->id << std::endl;
    //std::cout<< "\t" <<"source of type : "<< q->src->type <<" dst of type : "<< q->dst->type <<std::endl;

  /*  if (q->dst->type == HOST) {
        assert(p->dst->id == q->dst->id);
        return NULL; // Packet arriving
    }*/
	//std::cout<<"\t---------------->"<< q->src->type << std::endl;

    //std::cout << "\t" << "node_details.my_type" << node_details.my_type << std::endl;
    if (node_details.dst_type == HOST) {
	//std::cout<<"\t"<<"------Host ----->"<<std::endl;
	//return ((FiberLink *) q->dst)->queues[p->dst->id / 16];	
	//return links[p->dst->id]->queue;	
	return hosts[node_details.dst_id]->queue[node_details.dst_sub_id]->node_details;	

//	std::cout<<"\t"<<"------Host End----->"<<std::endl;
	}	
    //else if (node_details.dst_type == SIMPLE_LINK) { 
    else if (node_details.dst_type == LINK) { //Because we use NODE
	//std::cout<<"\t"<<"---- LINK ------->"<<std::endl;
	//return ((FiberLink *) q->dst)->queues[p->dst->id / 16];	
	//return links[p->dst->id]->queue;	
	return slinks[node_details.dst_id]->node_details;	

	}
	
    else if (node_details.dst_type == EPS) {
	//std::cout<< "\t"<< "------ EPS ---------" << std::endl;
	return switches[node_details.dst_id]->queues[node_details.dst_sub_id]->node_details;
	}

    else if (node_details.dst_type == SINK) {

	return sinks[node_details.dst_id]->node_details;
	}

    else if ((node_details.dst_type != HOST) || (node_details.dst_type != LINK) || (node_details.dst_type != SINK)||(node_details.dst_type != EPS))  
        {

        std::cout <<"Asserting NULL "<< std::endl;
        assert(false);
        //return NULL;
        }
	//std::cout<<"\t---- LINK ------->"<<std::endl;
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

