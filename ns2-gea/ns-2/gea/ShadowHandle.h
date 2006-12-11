#ifndef _SHADOWHANDLE_H__
#define _SHADOWHANDLE_H__

#include <gea/ShadowEventHandler.h>
#include <gea/Time.h>
#include <apps/app.h>
#include <common/node.h>

namespace gea {

    class ShadowHandle {
	
    private:
	
	friend class ShadowEventHandler;
	friend class Blocker;
    protected:
	//bool isBlockerPred;

	enum HandleType {
	    TypeBlocker,
	    TypeDepend,
	    TypeUdp,
	};
	
	enum HandleType handleType;
	friend class DependHandle; // direct access by DependHandle 
	
	gea::EventHandler::Event e;
	void *data;
	
    public:
	
	class ::Node * 	const node;
	
	enum Status {
	    Undefined,
	    Error,
	    Blocked,
	    Timeout,
	    Ready
	};
    
	enum Status status;
    
	ShadowHandle() : 
	    //	    isBlockerPred(false),
	    handleType(TypeUdp),
	    node(gea::ShadowEventHandler::currentNode),
	    status(Undefined)
	{}

	virtual ~ShadowHandle() {}
    
	bool isBlocker() const   {return this->handleType == TypeBlocker; }
	bool isDepend() const    {return this->handleType == TypeDepend; }
	bool isUdpHandle() const {return this->handleType == TypeUdp; }
	
	virtual int write(const char *buf, int size) { return -1; }
	virtual int read (char *buf,       int size) { return -1; }
	
    }; // end class Handle

}; // end namespace gea

#endif //SHADOWHANDLE_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
