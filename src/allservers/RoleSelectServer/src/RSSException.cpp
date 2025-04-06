#include "UniG.h"

ServerContextException::ServerContextException(const char* what) throw()
: mWhat(what)
{
    // No body
}

ServerContextException::ServerContextException(const string& what) throw()
: mWhat(what)
{
    // No body
}

const char* ServerContextException::what() const throw()
{
    return mWhat.c_str();
}
