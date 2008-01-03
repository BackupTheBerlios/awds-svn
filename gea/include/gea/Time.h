#ifndef _GEATIME_H__
#define _GEATIME_H__

#include <cassert>

#include <gea/ApiIface.h>

namespace gea {

    class AbsTime;

    class Duration;

    /** \brief Base type for fix-point number representation
     */
    class FixNum {

    public:
	/** \brief type used for storing the fix-point number */
	typedef long long StoreType;
	StoreType v; ///< the variable the value is stored in
	static const StoreType offset = 0x10000000LL;

	/** \brief the size in bytes when stored in a memory location
	 *  \see FixNum::toArray()
	 *  \see FixNum::fromArray()
	 */
	static const unsigned size = sizeof(StoreType);

	/** \brief defaul contructor
	 *
	 *  The default constuctor will NOT initialise the object. This is
	 *  efficient and allows us to detect uninitialised values with tools like valgrind.
	 */
	FixNum() {}

	/** \brief initialise value from storage type.
	 */
	explicit FixNum(StoreType v) : v(v) {}


	/** \brief copy constructor
	 *
	 *  This implements the default semantics of a copy constructor.
	 */
	FixNum(const FixNum& fn) { this->v = fn.v; }

	/** \brief assignment operator with standard semantics
	 */
	FixNum& operator =(const FixNum& other) {this->v = other.v; return *this; }

	/** \brief macro for defining comparison operators */
#define bool_op(x) bool operator x (const FixNum& f) const {return this->v x f.v;}

	bool_op(==);
	bool_op(<);
	bool_op(<=);
	bool_op(>);
	bool_op(>=);
	bool_op(!=);
#undef bool_op // we don't need this anymore

	/** \brief store the value in a memory location
	 *  The toArray call allow to store the value in a memory location \a p
	 *  in a platform independent way.
	 *  \see FixNum::fromArray(const void *)
	 */
	void toArray(void *p) const {
	    unsigned long long temp = this->v;
	    unsigned char *buf = (unsigned char *)p;
	    buf += size;
	    for (unsigned i = 0; i != size; ++i) {
		--buf;
		*buf = temp;
		temp >>= 8;
	    }
	}

	/** \brief restore the value from a memory location
	 *  The fromArray call allow to load the value from a memory location \a p
	 *  in a platform independent way.
	 *  \see FixNum::toArray(void *)
	 */
	void fromArray(const void *p) {
	    unsigned char *buf = (unsigned char *)p;
	    v = 0;
	    for (unsigned i = 0; i != size; ++i) {
		v <<= 8;
		v |= buf[i];
	    }
	}

    };

    /** \brief Type for representing a fixed point in time
     *  \ingroup GEA_API
     */
    class AbsTime : public FixNum {

    public:
	/** \brief get constant and fixed point in time
	 *
	 *  The value returned by AbsTime::t0() is normally used to generate
	 *  Duration objects from a given AbsTime values, e.g. to print out
	 *  a timestamp for debugging purposes.
	 *  \note The actual value returned is not specified and may change between
	 *        different runs of your program.
	 */
	static inline AbsTime t0() {
	    AbsTime retval(0);
	    return retval;
	}

	/** \brief Get the current time.
	 *
	 *  An AbsTime object is returned that represents the current time.
	 *  The time is the real (wall clock) time of the system, which requires an
	 *  expensive system call.
	 *  Often, it is better to use the time the last GEA event was triggered.
	 *  Please use GEA.lastEventTime in this case.
	 *
	 *  \see GEA.lastEventTime
	 */
	static AbsTime now() { AbsTime ret; GEA_apiIface->getCurrentTime(&ret); return ret; }

    public:

	AbsTime() : FixNum() {}

	AbsTime(StoreType v) :
	    FixNum(v) {}

	/** \brief copy constructor
	 */
	AbsTime(const AbsTime& other) : FixNum(other) {}

	/** \brief assignment operator
	 */
	AbsTime& operator =(const AbsTime& other) { FixNum::operator =(other); return *this; }

    }; // end class AbsTime

    /**
     *  \brief class representing the delta of two AbsTime objects.
     *  \ingroup GEA_API
     *
     *  The duration class is used to represent time spans. It defines operators
     *  to use it in a convinient way with the class gea::AbsTime. 
     */
    class Duration : public FixNum {

    public:

	/** \brief default constructor
	 *  
	 *  The default value of the object is undefined. This way we can detect 
	 *  uninitialised values with tools like valgrind.
	 */
	Duration() :
	    FixNum()
	{}


	/** \brief copy constructor */
	Duration(const Duration& d) :
	    FixNum(d.v) {}

	/** \brief initialise from double
	 *
	 *  The duration will have the value of \a t seconds.
	 */
	Duration(double t) :
	    FixNum( static_cast<StoreType>( t * this->offset + .5 ) )
	{}


	/** \brief Initialise from a rational number
	 *
	 *  The duration will have the value of a/b seconds.
	 *  \code
	 *  Duration one_third(1,4);
	 *  \endcode
	 *  will create a duration of 0.25 second. This avoids the use of floating point operations.
	 */
	Duration(long a, long b) :
	    FixNum( static_cast<StoreType>(a) * this->offset /  static_cast<StoreType>(b) )
	{}

	/** \brief convert the Duration to a double type
	 */
	operator double() const{
	    return double(this->v) / double(this->offset);
	}

    }; // end class Duration


    /** \brief calculate the difference between two absolute times.
     *  \ingroup GEA_API
     */
    static inline Duration operator -(const AbsTime& a, const AbsTime& b) {
	Duration ret(0);
	ret.v = a.v - b.v;
	return ret;
    }

    /** \brief add a duration to an absolute time.
     *  \ingroup GEA_API
     */
    static inline AbsTime operator +(const AbsTime& a, const Duration b) {
	return AbsTime(a.v + b.v);
    }

    /** \brief substract a duration from an absolute time.
     *  \ingroup GEA_API
     *
     */
    static inline AbsTime operator -(const AbsTime& a, const Duration b) {
	return AbsTime(a.v - b.v);
    }

    /** \brief add a duration to an absolute time.
     *  \ingroup GEA_API
     */
    static inline AbsTime& operator +=(AbsTime& a, const Duration b) {
	a.v += b.v;
	return a;
    }

    /** \brief substract a duration from an absolute time.
     *  \ingroup GEA_API
     *
     */
    static inline AbsTime& operator -=(AbsTime& a, const Duration b) {
	a.v -= b.v;
	return a;
    }
}


#endif //_GEATIME_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
