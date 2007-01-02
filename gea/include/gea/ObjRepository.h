#ifndef _OBJREPOSITORY_H__
#define _OBJREPOSITORY_H__

#include <map>
#include <string>



/** \brief global object repository
 *  This class provides a global repository for named objects. 
 *  Pointer to objects can be stored with a given name. These can 
 *  later be retrieved by other code, especially but other GEA applications. 
 */
class ObjRepository {
    
private:

    typedef std::pair<long, std::string> RepKey;
    typedef std::pair<std::string, void *> RepValue;
    
    typedef std::map<RepKey, RepValue>Rep;

    /** only system specific funtion */
    long whichNode() const;
        
    Rep rep;
    static ObjRepository *inst;
    
public:    
    
    /** \brief get the object repository.
     *  There exists only one repository per process. That's why 
     *  the constructor is private and thus no new instances can be
     *  created. Instead this function is used for getting the one and
     *  only instance. 
     *  \returns a reference to the object repository. 
     */
    static ObjRepository& instance() {
	if (!inst) 
	    inst = new ObjRepository();
	return *inst;
    }
    
        
    /** \brief insert an object
     *  This method inserts a pointer to an object in the object
     *  repository. The given pointer must be ensured to be valid 
     *  as long a the object is registered. 
     *  \param name the unique name of this object
     *  \param type a string describing the type of the object (normally the classname)
     *  \param obj  a pointer to the object 
     *  \returns true if the registration succeeds, false otherwise.
     */
    bool insertObj(const char * name,
		   const char * type,
		   void * obj) 
    {
	int node = whichNode();
	std::string sname(name);
	std::string stype(type);
	
	RepKey          key(node,sname);
	RepValue        val(stype, obj);
	Rep::value_type item(key, val);
	
	return rep.insert(item).second;
    }

    
    /** \brief get an object by its name
     *  This methods returns a pointer to the object, stored with a name.
     *  \param name The name of the object.
     *  \returns pointer to the object, or 0 if not found.
     */
    void * getObj(const char *name) const 
    {
	int    node = whichNode();
	std::string sname(name);
	RepKey key(node,sname);
	
	Rep::const_iterator i = rep.find(key);
	if (i == rep.end()) 
	    return 0;
	else  
	    return (*i).second.second;
	
    }


    
    /** \brief get type of a stored object
     *  This method is used for retrieving the type of a stored object.
     *  It should be used to check type before casting the void *-pointer 
     *  into a specific type. 
     *  \param name the name of the object.
     *  \returns a string describing the type, of 0 if not found.
     */
    const char * getType(const char *name) const 
    {
	int    node = whichNode();
	std::string sname(name);
	RepKey key(node,sname);
	
	Rep::const_iterator i = rep.find(key);
	if (i == rep.end())
	    return 0;
	else 
	    return i->second.first.c_str();
    }   
    
    

    /** \brief remove an object from the repository. 
     *  This is used for removeing objects from the repository.
     *  \param name the name of the object
     *  \returns true, if successfully removed, false otherwise. 
     */
    bool removeObject(const char *name) {
	int    node = whichNode();
	std::string sname(name);
	RepKey key(node,sname);
		
	return ( rep.erase(key) != 0 );
    }
    
};

// ---------------------------------------- 
// helper macros:
// ---------------------------------------- 

#define REP_INSERT_OBJ(type, name, val) do {	\
	type __value = val;			\
	ObjRepository::instance().insertObj(#name, #type, (void *)__value); \
    } while(0)

#define REP_MAP_OBJ(type, name) type name = (type)ObjRepository::instance().getObj(#name)




#endif //OBJREPOSITORY_H__
/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
