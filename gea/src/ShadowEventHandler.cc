
#include <streambuf>

#include <cstdlib>
#include <gea/posix/ShadowEventHandler.h>


class NullBuf : public std::basic_streambuf<char> {
public:
    NullBuf() : std::basic_streambuf<char>() {};
};



gea::ShadowEventHandler::ShadowEventHandler() :
     nullOut(new NullBuf())
{

    char *dbg_lvl_string = getenv("GEA_DBG");
    this->dbgLevel = bool(dbg_lvl_string) ? atoi(dbg_lvl_string) : 0xFFFF;
}

gea::ShadowEventHandler::~ShadowEventHandler() {

}


/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

