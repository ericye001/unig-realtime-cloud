/*
* Copyright (c) 2002-2006, UOneNet, All right reserved
* All rights reserved.
* 
* 文件名称：
* 文件标识：
* 摘    要：
* 
*
* 修订记录：
*       修订日期    修订人    修订内容
*/

#ifndef BUFFER_H
#define BUFFER_H

#include <inttypes.h>

#include "Utils.h"

class CharBuffer_T : public ObjectLevelLockable_T<CharBuffer_T>
{
public:
    static CharBuffer_T* allocate(int capacity);
    
    virtual ~CharBuffer_T();
    
    int capacity();
            
    int clength();    
    
    CharBuffer_T* reset();
    
    char get();
    
    CharBuffer_T* get(void* dst, int length);
       
    CharBuffer_T* put(char c);
    
    CharBuffer_T* put(void* src, int length);
    
    int remaining();
    
    CharBuffer_T* alignLeft();

    uint32_t getMsgLen();
    bool getComeleteMsg(void* dst,uint32_t);
    uint32_t getCompeleteBuf(void* dst);
    bool isCompeleteMsg();
    
protected:
    CharBuffer_T(int capacity);
    CharBuffer_T(const CharBuffer_T& rhs);
    CharBuffer_T& operator=(const CharBuffer_T& rhs);
    
protected:
    int _capacity;
    int _nextGetIndex;
    int _nextPutIndex;
    char* _cbuf;
};

#endif
