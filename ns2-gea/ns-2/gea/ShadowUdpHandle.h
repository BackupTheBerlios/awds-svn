/* -*-	Mode:C++; c-basic-offset:4; tab-width:8; indent-tabs-mode:t -*- */
#ifndef _SHADOWUDPHANDLE_H__
#define _SHADOWUDPHANDLE_H__

#include <gea/EventHandler.h>
#include <gea/Time.h>
#include <gea/UdpHandle.h>
#include <gea/Handle.h>
#include <common/agent.h>


class Node;
//class GeaUdpApp;


namespace gea {

    
    class ShadowUdpAddress {
	
    public:	
	int port;
	int ip;
	
	friend class UdpHandle;
	
	ShadowUdpAddress(int port, const char *ip);
	ShadowUdpAddress(const ShadowUdpAddress& a);
    };
    

    class ShadowUdpHandle : public SubUdpHandle, public ::Application {

	friend class ShadowEventHandler;    
	friend class UdpHandle;

    protected:
	
	class GeaUdpTimer : public TimerHandler {
	protected:
	    gea::ShadowUdpHandle *h;
	    virtual void expire (::Event *) { h->do_timeout(); }
	public:
	    GeaUdpTimer(gea::ShadowUdpHandle *h) : h(h) {};
	    friend class gea::ShadowUdpHandle;
	};



	GeaUdpTimer timeout_timer;
	class ::Agent *agent;
	
	
    public:
	static gea::UdpHandle      *currentUdpHandle;
	static gea::UdpHandle::Mode currentMode;
	static gea::UdpAddress      currentUdpAddress; 
	

	/* all required infos */
	gea::UdpHandle *handle;
	gea::EventHandler::Event e;
	void *data;
	gea::AbsTime timeout;
	

	void setup_event(gea::EventHandler::Event e, void*data, AbsTime timeout);
	
	class ::PacketData* curData;
		
	void do_timeout();
	friend class GeaUdpTimer;
	
	void init();
	
    public:

	const gea::UdpHandle::Mode mode;	

	gea::UdpAddress addr;
	gea::UdpAddress src;
	
	ShadowUdpHandle(gea::UdpHandle::Mode mode, const gea::UdpAddress& addr);
	

	virtual int setSrc(const UdpAddress& src_addr);
	virtual void setDest(const UdpAddress& dest_addr);
	
	virtual int write(const char *buf, int size);
	virtual int read (char *buf, int size);
	
	virtual UdpAddress getSrc() const;
	virtual UdpAddress getDest() const;
	
	virtual void process_data(int size, ::AppData* data);
	int send(int size,const  char *data);
	virtual ~ShadowUdpHandle();
    
	
    };
};


#endif //UDPHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
