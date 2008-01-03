#ifndef _UNIXFDHANDLER_H__
#define _UNIXFDHANDLER_H__


#include <gea/posix/ShadowHandle.h>
#include <gea/Handle.h>

namespace gea {

    /** \brief handle for Unix file descriptors
     *  \ingroup PosixAPI
     *
     *  Creates a handle for a unix file descriptor.
     *
     */
    class UnixFdHandle : public Handle {

    public:

	/** \brief Constuct Handle for Unix file descriptor
	 *  \param fd   the Unix file descriptor that should be watched.
	 *  \param mode for what events should the file descriptor be monitored.
	 *
	 *  \note When you want to use a file descriptor for reading \e and writing,
	 *        you should create two UnixFdHandle objects, one for each.
	 */
	UnixFdHandle(int fd, gea::PosixHandleMode mode);

	/** \brief call write() on the file descriptor */
	virtual int write(const char *buf, int size);

	/** \brief call read() on the file descriptor */
	virtual int read (char *buf, int size);

	~UnixFdHandle();
    };

}


#endif //UNIXFDHANDLER_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
