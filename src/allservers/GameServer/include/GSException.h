/************************************************* 
Copyright(C),2006,
Uone Net .Co., Ltd. 
Filename: GSException.h
Author: Abone
Version: 2.0
Date: 2006-12-15
Description: GameServer��һЩ�쳣��

History:
1.
Date: 
Author: 
Modification: 
**************************************************/ 

#ifndef _GS_EXCPETION_H__
#define _GS_EXCPETION_H__

/**
 * ��peer serverͨѶʱ�������쳣
 */
class ServerContextException : public exception
{
public:
    ServerContextException(const char* what) throw();
    ServerContextException(const string& what) throw();
    ~ServerContextException()throw(){};

    virtual const char* what() const throw();

private:
    string mWhat;   
};
    
#endif

