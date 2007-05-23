
#ifndef _GEATIME_H__
#define _GEATIME_H__

#include <cassert>

#include <gea/ApiIface.h>

namespace gea {
    
    class AbsTime;
    
    class Duration;
    
    class FixNum {
	
    public:
	
	typedef long long StoreType;
	StoreType v;
	static const StoreType offset = 0x10000000LL; 
	static const unsigned size = sizeof(StoreType);    
	
	FixNum() {}
	explicit FixNum(StoreType v) : v(v) {}
	FixNum(const FixNum& fn) { this->v = fn.v; }
	FixNum& operator =(const FixNum& other) {this->v = other.v; return *this; }
	
#define bool_op(x) bool operator x (const FixNum& f) const {return this->v x f.v;}
    
	bool_op(==);
	bool_op(<);
	bool_op(<=);
	bool_op(>);
	bool_op(>=);
	bool_op(!=);
#undef bool_op // we don't need this anymore

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
	
	void fromArray(const void *p) {
	    unsigned char *buf = (unsigned char *)p;
	    v = 0;
	    for (unsigned i = 0; i != size; ++i) {
		v <<= 8;
		v |= buf[i];
	    }
	}
	
    };

    class AbsTime : public FixNum {

	
    public:

	static inline AbsTime t0() {
            AbsTime retval(0);
            return retval;
        }
	

	static AbsTime now() { AbsTime ret; GEA_apiIface->getCurrentTime(&ret); return ret; }
	
    public:
	
	AbsTime() : FixNum() {}
	
	AbsTime(StoreType v) :
	    FixNum(v) {}
	
	AbsTime(const AbsTime& other) : FixNum(other) {}
	
	AbsTime& operator =(const AbsTime& other) { FixNum::operator =(other); return *this; }
	
    }; // end class AbsTime

    /**
     *  \brief class representing the delta of two AbsTime objects. 
     */
    class Duration : public FixNum {
	
    public:
    
	Duration() : 
	    FixNum(0) 
	{}

	Duration(const Duration& d) :
	    FixNum(d.v) {}
	
	Duration(double t) : 
	    FixNum( static_cast<StoreType>( t * this->offset + .5 ) )
	{}
	
	Duration(long a, long b) : 
	    FixNum( static_cast<StoreType>(a) * this->offset /  static_cast<StoreType>(b) ) 
	{}

	operator double() const{
	    return double(this->v) / double(this->offset);
	}
    
    }; // end class Duration

    static inline Duration operator -(const AbsTime& a, const AbsTime& b) {
	Duration ret(0);
	ret.v = a.v - b.v;
	return ret;
    }

    static inline AbsTime operator +(const AbsTime& a, const Duration b) {
	return AbsTime(a.v + b.v);
    }

    static inline AbsTime operator -(const AbsTime& a, const Duration b) {
	return AbsTime(a.v - b.v);
    }

    static inline AbsTime& operator +=(AbsTime& a, const Duration b) {
	a.v += b.v;
	return a;
    }

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
