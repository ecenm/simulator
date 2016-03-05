#ifndef EVENT_H
#define EVENT_H


#include <iostream>
#include <queue>
#include <vector>
#include "packet.h"
#include "random_variable.h"


#define PACKET_ARRIVAL 0
#define PACKET_DEPARTED 1
#define PACKET_ARRIVAL_AT_SWITCH 2
#define PACKET_DEPARTURE_FROM_SWITCH 3
#define PACKET_CREATION_EVENT 4
#define SWITCH_ARBITRATION 5
#define PACKET_SERVICE_EVENT 6
#define PACKET_PUSHING_EVENT 7

 
#define LOGGING 9
#define STATS 10
#define TOKEN 11


class event {
public:
    // Construct sets time of event.
    event(uint32_t type ,double t);
    virtual ~event();
    // Execute event by invoking this method.
    virtual void process_event () = 0;
    uint32_t type;
    double time;
    bool cancelled; 
    uint32_t unique_id;
    static uint32_t instance_count;  
  		
};

struct EventComparator {
    bool operator() (const event * left, const event * right) const {
        return left->time > right->time;
    }
};

class PacketCreationForInitializationEvent : public event {
    public:
        PacketCreationForInitializationEvent(
                double time,
                Host *src,
                Host *dst,
                RandomVariable *nv_bytes,
                RandomVariable *nv_intarr
                );
        ~PacketCreationForInitializationEvent();
        void process_event();
        Host *src;
        Host *dst;
        RandomVariable *nv_bytes;
        RandomVariable *nv_intarr;
};

class PacketCreationEvent : public event {
    public:
        PacketCreationEvent(double time, Packet *packet);
        ~PacketCreationEvent();
        void process_event();
        Packet *packet;
};


class PacketPushingEvent : public event {
    public:
        PacketPushingEvent(double time, Packet *packet);
        ~PacketPushingEvent();
        void process_event();
        Packet *packet;
};


class PacketServiceEvent : public event {
    public:
        PacketServiceEvent(double time, Packet *packet);
        ~PacketServiceEvent();
        void process_event();
        Packet *packet;
};



class PacketArrivalEvent : public event {
    public:
        PacketArrivalEvent(double time, Packet *packet);
        ~PacketArrivalEvent();
        void process_event();
        Packet *packet;
};

class PacketDepartureEvent : public event {
    public:
        PacketDepartureEvent(double time, Packet *packet);
        ~PacketDepartureEvent();
        void process_event();
        Packet *packet;
};

class PacketArrivalAtSwitch : public event {
    public:
        PacketArrivalAtSwitch(double time, Packet *packet);
        ~PacketArrivalAtSwitch();
        void process_event();
        Packet *packet;

};

class SwitchArbitration : public event {
    public:
        SwitchArbitration(double time, Packet *packet);
        ~SwitchArbitration();
        void process_event();
        Packet *packet;

};


class PacketDepartureFromSwitch : public event {
    public:
        PacketDepartureFromSwitch(double time, Packet *packet);
        ~PacketDepartureFromSwitch();
        void process_event();
        Packet *packet;

};


// Have to declare all my events here 
class LoggingEvent : public event {
    public:
        LoggingEvent(double time);
        LoggingEvent(double time, double ttl);
        ~LoggingEvent();
        void process_event();
        double ttl;
};

class StatsEvent : public event {
    public:
        StatsEvent(double time);
        ~StatsEvent();
        void process_event();
};

class TokenEvent : public event {
    public:
        TokenEvent(double time, Packet *packet);
        ~TokenEvent();
        void process_event();
        Packet *packet;
};



#endif
