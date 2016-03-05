#include "packet_generator.h"

// returns a random integer between 0 and n with n <= 32767 <= INT_MAX
/*int irand (int n)
{
    static int seq[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
    };

    std::random_shuffle (seq, seq + sizeof seq / sizeof *seq);

    const int rnd =
        ((seq [0] << 11) | (seq [1] << 8) | (seq [2] << 4)) + seq [3];

    return rnd % n;
}*/

PacketGenerator::PacketGenerator(uint32_t num_packets, Topology *topo, std::string filename) {
    this->num_packets = num_packets;
    this->topo = topo;
    this->filename = filename;
}

void PacketGenerator::write_packets_to_file(std::deque<Packet *> packets, std::string filename){
    std::cout<< "Going to write" << std::endl;
    std::ofstream output(filename);
    output.precision(20);
    if (packets.size()==0)
        output<< "EMPTY -------------" << " ";
    else{
        for (uint i = 0; i < packets.size(); i++){
            output 
                << packets[i]->sending_time << " " 
                << packets[i]->seq_no << " " 
                << packets[i]->pf_priority << " " 
                << packets[i]->size << " " 
                << packets[i]->start_time << " " 
                << packets[i]->end_time << " " 
                << packets[i]->src->id << " " 
                << packets[i]->dst->id << "\n";
            }
        }
        output.close();
    
}

// --------------------This is my function -------------------------------------
//
void PacketGenerator::write_pkt_file(std::deque<Packet *> packets, std::string filename){

    std::cout << "Going to write in a file" << std::endl;
    std::ofstream a_file (filename);
    a_file<<"This text will now be inside of example.txt";
    //Packet *P1 = new Packet(20.4, i, 0, 64, startTime, 0);
   // for (uint32_t i = 0; i < 10; i++){

        Packet *P1 = new Packet(20.4, 1, 0, 64, topo->hosts[0], topo->hosts[0]);
     //   a_file << 1 << "";
        add_to_event_queue(new PacketCreationEvent(1.00, P1));
    //} 
        a_file.close();
      //  get_event_queue_size();
}

//sample packet generation. Should be overridden.
/*void PacketGenerator::make_packets() {
    std::cout<<"-->"<<std::endl;
    //EmpiricalRandomVariable *nv_bytes = new EmpiricalRandomVariable(1460);
    ExponentialRandomVariable *nv_bytes = new ExponentialRandomVariable(4);
    ExponentialRandomVariable *nv_intarr = new ExponentialRandomVariable(0.0000001);
    add_to_event_queue(new PacketCreationForInitializationEvent(1.0, topo->hosts[0], topo->hosts[1], nv_bytes, nv_intarr));
    if(params.debug==1)
    std::cout << "Packets are sent from Host: "<< topo->hosts[0]->id << "to Host: " <<topo->hosts[1]->id<<" with "<< nv_bytes->value() << "and  interarrival time of "<< nv_intarr->value()<<  std::endl;

    while (event_queue.size() > 0) {
        event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (packets_to_schedule.size() < 5) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;
}*/

void PacketGenerator::make_packets() {
    
    std::cout<<"PacketGenerator:: make_packets -> PacketCreation"<<std::endl;

    for (uint32_t i = 0; i < params.num_packets_to_run; i++){
    Packet *p1 = new Packet(start_time, i, 0, 64, topo->hosts[0],topo->hosts[0]);
    if(params.debug==0){
    std::cout << " AT PACKETGEN <<< Packets sent from Host: "<< p1->src->id << " to Host: " <<p1->dst->id <<" with size "<< p1->size << ", sequence number " << p1->seq_no <<" priority "<< p1->pf_priority <<" and sending time of "<< p1->sending_time<<  std::endl;
    }
    packets_to_schedule.push_back(p1); 
    add_to_event_queue(new PacketCreationEvent(start_time,p1));
    }
     
    /*    //EmpiricalRandomVariable *nv_bytes = new EmpiricalRandomVariable(1460);
    ExponentialRandomVariable *nv_bytes = new ExponentialRandomVariable(4);
    ExponentialRandomVariable *nv_intarr = new ExponentialRandomVariable(0.0000001);
    add_to_event_queue(new PacketCreationForInitializationEvent(1.0, topo->hosts[0], topo->hosts[1], nv_bytes, nv_intarr));
    if(params.debug==1)
    std::cout << "Packets are sent from Host: "<< topo->hosts[0]->id << "to Host: " <<topo->hosts[1]->id<<" with "<< nv_bytes->value() << "and  interarrival time of "<< nv_intarr->value()<<  std::endl;

    while (event_queue.size() > 0) {
        event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (packets_to_schedule.size() < 5) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;*/
}



PacketReader::PacketReader(uint32_t num_packets, Topology *topo, std::string filename) : PacketGenerator(num_packets, topo, filename) {};

void PacketReader::make_packets() {
    std::ifstream input(filename);
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        double start_time, temp;
        uint32_t size, s, d;
        uint32_t id;
        
        // <id> <start_time> blah blah <size in packets> blah blah <src> <dst>

        if (!(iss >> id >> start_time >> temp >> temp >> size >> temp >> temp >> s >> d)) {
            break;
        }
        
        //size = (uint32_t) (params.mss * size); -- Neels TODO
        size = 64;

        std::cout << "Packet " << id << " " << start_time << " " << size << " " << s << " " << d << "\n";
        packets_to_schedule.push_back(
            Factory::get_packet(start_time, id, 0, size, topo->hosts[s], topo->hosts[d])
        );
    }
    //params.num_flows_to_run = flows_to_schedule.size(); -- Neels TODO
    input.close();
}


/*
PoissonPacketGenerator::PoissonPacketGenerator(uint32_t num_packets, Topology *topo, std::string filename) : PacketGenerator(num_packets, topo, filename) {};
    
void PoissonPacketGenerator::make_packets() {
    EmpiricalRandomVariable *nv_bytes;
    if (params.smooth_cdf)
        nv_bytes = new EmpiricalRandomVariable(filename);
    else
        nv_bytes = new CDFRandomVariable(filename);

    params.mean_flow_size = nv_bytes->mean_flow_size;

    double lambda = params.bandwidth * params.load / (params.mean_flow_size * 8.0 / 1460 * 1500);
    double lambda_per_host = lambda / (topo->hosts.size() - 1);
    std::cout << "Lambda: " << lambda_per_host << std::endl;


    ExponentialRandomVariable *nv_intarr;
    if (params.burst_at_beginning)
        nv_intarr = new ExponentialRandomVariable(0.0000001);
    else
        nv_intarr = new ExponentialRandomVariable(1.0 / lambda_per_host);

    /* [expr ($link_rate*$load*1000000000)/($meanFlowSize*8.0/1460*1500)]
    for (uint32_t i = 0; i < topo->hosts.size(); i++) {
        for (uint32_t j = 0; j < topo->hosts.size(); j++) {
            if (i != j) {
                double first_flow_time = 1.0 + nv_intarr->value();
                add_to_event_queue(
                    new FlowCreationForInitializationEvent(
                        first_flow_time,
                        topo->hosts[i], 
                        topo->hosts[j],
                        nv_bytes, 
                        nv_intarr
                    )
                );
            }
        }
    }

    while (event_queue.size() > 0) {
        Event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (flows_to_schedule.size() < num_flows) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;
}
CustomCDFFlowGenerator::CustomCDFFlowGenerator(
        uint32_t num_flows, 
        Topology *topo, 
        std::string filename, 
        std::string interarrivals_cdf_filename
    ) : FlowGenerator(num_flows, topo, filename) {
    this->interarrivals_cdf_filename = interarrivals_cdf_filename;
};

std::vector<EmpiricalRandomVariable*>* CustomCDFFlowGenerator::makeCDFArray(std::string fn_template, std::string filename) {
    auto pairCDFs = new std::vector<EmpiricalRandomVariable*>(params.num_host_types * params.num_host_types);
    for (auto i = 0; i < params.num_host_types; i++) {
        for (auto j = 0; j < params.num_host_types; j++) {
            if (i == j) {
                pairCDFs->at(params.num_host_types * i + j) = NULL;
                continue;
            }
            char buffer[128];
            snprintf(buffer, 128, fn_template.c_str(), filename.c_str(), i, j);
            std::string cdf_fn = buffer;
            std::ifstream cdf_file(cdf_fn);
            if (cdf_file.good()) {
                pairCDFs->at(params.num_host_types * i + j) = new CDFRandomVariable(cdf_fn);
            }
            else {
                pairCDFs->at(params.num_host_types * i + j) = NULL;
            }
        }
    }
    return pairCDFs;
}

uint32_t* customCdfFlowGenerator_getDestinations(uint32_t num_hosts, uint32_t sender_id, uint32_t num_dests) {
    if (params.ddc_type == 0) {
        auto dests = new uint32_t[num_dests];
        for (uint32_t i = 0; i < num_dests;i++) {
            uint32_t dest = rand() % num_hosts;
            if (dest == sender_id) continue;
            dests[i] = dest;
        }
        return dests;
    }
    else {
        auto dests = new uint32_t[num_dests];
        uint32_t rack_start = sender_id / 16;
        if (sender_id % 16 < 13) {
            auto index = 0;
            for (auto i = 0; i < num_dests + 1; i++) {
                if (rack_start * 16 + i == sender_id) continue;
                dests[index++] = rack_start * 16 + i;
            }
        }
        else {
            uint32_t start = 0;
            if (sender_id >= 78) start = 78;
            auto j = 0;
            for (auto i = start; i < 144; i++) {
                if (j == num_dests) break;
                if (i % 16 < 13 || i == sender_id) continue;
                dests[j] = i;
                j++;
            }
        }

        return dests;
    }
}

void CustomCDFFlowGenerator::make_flows() {
    std::vector<EmpiricalRandomVariable*>* sizeMatrix = makeCDFArray("%s/cdf_sizes_%d-%d.txt", filename);
    std::vector<EmpiricalRandomVariable*>* interarrivalMatrix = makeCDFArray("%s/cdf_interarrivals_%d-%d.txt", filename);
    uint32_t num_hosts = topo->hosts.size();

    for (uint32_t i = 0; i < num_hosts; i++) {
        // select a sender profile randomly, then pick n-1 destinations for each dest.
        uint32_t sender_profile = i % params.num_host_types;
        uint32_t* dests = customCdfFlowGenerator_getDestinations(num_hosts, i, params.num_host_types - 1);
        if (params.ddc_type != 0) {
            for (auto d = 0; d < params.num_host_types - 2; d++) {
                if (i % 16 < 13 && (i / 16) != (dests[d] / 16)) {
                    std::cout << i << " " << d << " " << dests[d] << "  " << i / 16 << " " << dests[d] / 16 << "\n";
                    assert(false);
                }
            }
        }
        for (uint32_t j = 0; j < params.num_host_types - 2; j++) {
            EmpiricalRandomVariable* nv_bytes = sizeMatrix->at(params.num_host_types * sender_profile + j);
            EmpiricalRandomVariable* nv_intarr = interarrivalMatrix->at(params.num_host_types * sender_profile + j);
            uint32_t d = dests[j];
            // each node represents 3x of that resource.
            for (uint32_t k = 0; k < 3; k++) {
                if (nv_bytes != NULL && nv_intarr != NULL) {
                    double first_flow_time = 1.0 + nv_intarr->value();
                    add_to_event_queue(
                        new FlowCreationForInitializationEvent(
                            first_flow_time,
                            topo->hosts[i], 
                            topo->hosts[d],
                            nv_bytes, 
                            nv_intarr
                        )
                    );
                }
            }
        }
        delete dests;
    }

    delete sizeMatrix;
    delete interarrivalMatrix;

    while (event_queue.size() > 0) {
        Event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (flows_to_schedule.size() < num_flows) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;
}

PermutationTM::PermutationTM(uint32_t num_flows, Topology *topo, std::string filename) : FlowGenerator(num_flows, topo, filename) {}

void PermutationTM::make_flows() {
    EmpiricalRandomVariable *nv_bytes;
    if (params.smooth_cdf)
        nv_bytes = new EmpiricalRandomVariable(filename);
    else
        nv_bytes = new CDFRandomVariable(filename);

    params.mean_flow_size = nv_bytes->mean_flow_size;

    double lambda = params.bandwidth * params.load / (params.mean_flow_size * 8.0 / 1460 * 1500);
    std::cout << "Lambda: " << lambda << std::endl;

    auto *nv_intarr = new ExponentialRandomVariable(1.0 / lambda);

    std::set<uint32_t> dests;
    for (uint32_t i = 0; i < topo->hosts.size(); i++) {
        uint32_t j = i;
        while (j != i && dests.find(j) != dests.end()) {
            j = rand() % topo->hosts.size();
        }
        dests.insert(j);
        double first_flow_time = 1.0 + nv_intarr->value();
        add_to_event_queue(
            new FlowCreationForInitializationEvent(
                first_flow_time,
                topo->hosts[i], 
                topo->hosts[j],
                nv_bytes, 
                nv_intarr
            )
        );
    }

    while (event_queue.size() > 0) {
        Event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (flows_to_schedule.size() < num_flows) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;
}*/

//
// uninimplemented flow generation schemes
//

// alternate flow generations
/*
   int get_flow_size(Host* s, Host* d){

   int matrix[3][3] =
   {
   {3*1460,    3*1460, 700*1460},
   {3*1460,    0,      0},
   {700*1460,  0,      0}
   };

   assert(s->host_type >= 0);
   assert(d->host_type >= 0);
   assert(s->host_type < 3);
   assert(d->host_type < 3);

   return matrix[s->host_type][d->host_type];
   }

   int get_num_src_or_dst(Host* d){
   if(d->host_type == CPU)
   return 143;
   else if(d->host_type == MEM)
   return 144/3;
   else if(d->host_type == DISK)
   return 144/3;
   else
   assert(false);
   }


   void generate_flows_to_schedule_fd_ddc(std::string filename, uint32_t num_flows, Topology *topo) {



   for (int i = 0; i < topo->hosts.size(); i++){
   topo->hosts[i]->host_type = i%3;
   }



   for (uint32_t dst = 0; dst < topo->hosts.size(); dst++) {
   for (uint32_t src = 0; src < topo->hosts.size(); src++) {
   if (src != dst) {
   int flow_size = get_flow_size(topo->hosts[src], topo->hosts[dst]);

   if(flow_size > 0){
   int num_sd_pair;
   if(params.ddc_normalize == 0) {
   num_sd_pair = get_num_src_or_dst(topo->hosts[src]);
   }
   else if(params.ddc_normalize == 1) {
   num_sd_pair = get_num_src_or_dst(topo->hosts[dst]);
   }
   else if (params.ddc_normalize == 2) {
   if (topo->hosts[src]->host_type == CPU) {
   num_sd_pair = get_num_src_or_dst(topo->hosts[src]);
   }
   else if(topo->hosts[dst]->host_type == CPU) {
   num_sd_pair = get_num_src_or_dst(topo->hosts[dst]);
   }
   else {
   assert(false);
   }
   }
   else {
   assert(false);
   }

   double lambda_per_pair = params.bandwidth * params.load / (flow_size * 8.0 / 1460 * 1500) / num_sd_pair;
//std::cout << src << " " << dst << " " << flow_size << " " <<lambda_per_pair << "\n";
ExponentialRandomVariable *nv_intarr = new ExponentialRandomVariable(1.0 / lambda_per_pair);
double first_flow_time = 1.0 + nv_intarr->value();
EmpiricalRandomVariable *nv_bytes = new ConstantVariable(flow_size/1460);

    add_to_event_queue(
            new FlowCreationForInitializationEvent(first_flow_time, topo->hosts[src], topo->hosts[dst], nv_bytes, nv_intarr)
            );

    }
}
}
}

while (event_queue.size() > 0) {
    Event *ev = event_queue.top();
    event_queue.pop();
    current_time = ev->time;
    if (flows_to_schedule.size() < num_flows) {
        ev->process_event();
    }
    delete ev;
}
current_time = 0;
}


void generate_flows_to_schedule_fd_with_skew(std::string filename, uint32_t num_flows,
        Topology *topo) {

    EmpiricalRandomVariable *nv_bytes;
    if(params.smooth_cdf)
        nv_bytes = new EmpiricalRandomVariable(filename);
    else
        nv_bytes = new CDFRandomVariable(filename);

    params.mean_flow_size = nv_bytes->mean_flow_size;

    double lambda = params.bandwidth * params.load / (params.mean_flow_size * 8.0 / 1460 * 1500);



    GaussianRandomVariable popularity(10, params.traffic_imbalance);
    std::vector<int> sources;
    std::vector<int> destinations;

    int self_connection_count = 0;
    for(int i = 0; i < topo->hosts.size(); i++){
        int src_count = (int)(round(popularity.value()));
        int dst_count = (int)(round(popularity.value()));
        std::cout << "node:" << i << " #src:" << src_count << " #dst:" << dst_count << "\n";
        self_connection_count += src_count * dst_count;
        for(int j = 0; j < src_count; j++)
            sources.push_back(i);
        for(int j = 0; j < dst_count; j++)
            destinations.push_back(i);
    }

    double flows_per_host = (sources.size() * destinations.size() - self_connection_count) / (double)topo->hosts.size();
    double lambda_per_flow = lambda / flows_per_host;
    std::cout << "Lambda: " << lambda_per_flow << std::endl;


    ExponentialRandomVariable *nv_intarr;
    if(params.burst_at_beginning)
        nv_intarr = new ExponentialRandomVariable(0.000000001);
    else
        nv_intarr = new ExponentialRandomVariable(1.0 / lambda_per_flow);

    // [expr ($link_rate*$load*1000000000)/($meanFlowSize*8.0/1460*1500)]
    for (uint32_t i = 0; i < sources.size(); i++) {
        for (uint32_t j = 0; j < destinations.size(); j++) {
            if (sources[i] != destinations[j]) {
                double first_flow_time = 1.0 + nv_intarr->value();
                add_to_event_queue(
                        new FlowCreationForInitializationEvent(first_flow_time,
                            topo->hosts[sources[i]], topo->hosts[destinations[j]],
                            nv_bytes, nv_intarr)
                        );
            }
        }
    }


    while (event_queue.size() > 0) {
        Event *ev = event_queue.top();
        event_queue.pop();
        current_time = ev->time;
        if (flows_to_schedule.size() < num_flows) {
            ev->process_event();
        }
        delete ev;
    }
    current_time = 0;
}
*/

