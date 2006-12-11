#ifndef _EVENTS_H__
#define _EVENTS_H__


namespace gea {

    class Handle {
	
    public:
	
	class ShadowHandle * const shadowHandle;
	
	enum Status {
	    Undefined,
	    Error,
	    Blocked,
	    Timeout,
	    Ready
	};
	
	Handle();

	enum Status status;
    	virtual int write(const char *buf, int size) = 0;
	virtual int read (char *buf,       int size) = 0;

	virtual ~Handle();
    };

} // end namespace gea

#endif //EVENTS_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
