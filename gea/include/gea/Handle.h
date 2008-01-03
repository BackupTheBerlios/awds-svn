#ifndef _EVENTS_H__
#define _EVENTS_H__


namespace gea {
    
    /** \brief Base class for all real handles
     *  \ingroup GEA_API
     */
    class Handle {
	
    public:
	
	class ShadowHandle * shadowHandle;
	
	/** \brief enum for the status of the handle.
	 */
	enum Status {
	    Undefined, ///< should never be the case.
	    Error,     ///< an error occured.
	    Blocked,   ///< the handle is blocked, we cannot read or write.
	    Timeout,   ///< a timeout occured.
	    Ready      ///< The handle is ready to read or write.
	};
	
	Handle();

	enum Status status;
    	
	/** \brief write handle 
	 *
	 *  \param buf  the memory address with the data.
	 *  \param size the number of bytes to write.
	 *  \returns the return value of the write system call. 
	 */
	virtual int write(const char *buf, int size) = 0;

	/** \brief read from handle 
	 *
	 *  \param buf  the memory for storing the data.
	 *  \param size the number of bytes to read.
	 *  \returns the return value of the readsystem call. 
	 */
	virtual int read (char *buf, int size) = 0;
	
	/** \brief destructor */
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
