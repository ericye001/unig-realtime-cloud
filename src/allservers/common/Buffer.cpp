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

#include "common/Buffer.h"

CharBuffer_T* CharBuffer_T::allocate(int capacity)
{
    CharBuffer_T* p = new CharBuffer_T(capacity);
    if (!p)
    {
        throw StrException_T(string("Memory fault at CharBuffer_T::allocate")
            + SrcLocation_T(__FILE__, __LINE__));
    }
    return p;
}

CharBuffer_T::~CharBuffer_T()
{
    if (_cbuf) 
    {
        delete [] _cbuf;
    }    
}

CharBuffer_T::CharBuffer_T(int capacity)
    :
    _capacity(capacity),
    _nextGetIndex(0),
    _nextPutIndex(0)
{
    _cbuf = new char[capacity];
    if (!_cbuf)
    {
        throw StrException_T(
            string( "Memory fault at CharBuffer_T::CharBuffer_T") +
            SrcLocation_T(__FILE__, __LINE__));
    }
}

int CharBuffer_T::capacity()
{
    return _capacity;
}

int CharBuffer_T::clength()
{

  
  //  Lock_T lock(*this);
    return _nextPutIndex - _nextGetIndex;
}

CharBuffer_T* CharBuffer_T::reset()
{
    Lock_T lock(*this);
    _nextGetIndex = 0;
    _nextPutIndex = 0;
    return this;
}

char CharBuffer_T::get()
{
    if (clength() < (int)sizeof(char))    {
        throw StrException_T(string("DataUnderflow at CharBuffer_T::get")
            + SrcLocation_T(__FILE__, __LINE__));
    }
    Lock_T lock(*this);
    return _cbuf[_nextGetIndex++];
}


uint32_t CharBuffer_T::getCompeleteBuf(void* dst)
{

 
  uint32_t  length=0;
  if (clength() < sizeof(uint32_t)+ sizeof(uint16_t))    {
    throw StrException_T(string("DataUnderflow at CharBuffer_T::getcompeltebuf")
                         + SrcLocation_T(__FILE__, __LINE__));
  }
 
  if (_nextGetIndex > _capacity || _nextGetIndex +  sizeof(uint32_t) > _capacity) {
    length=0;
    throw StrException_T("AssertException at getcompeltebuf");
  }

  memcpy(&length,_cbuf+_nextGetIndex, sizeof(uint32_t) );
  _nextGetIndex +=  sizeof(uint32_t);
  // cout<<"need a length :"<<length<<endl;
  if (clength() <  length) {
  
    _nextGetIndex -=  sizeof(uint32_t);
  }
  else {

    if (_nextGetIndex > _capacity || _nextGetIndex + length > _capacity) {
      length=0;
      _nextGetIndex -=  sizeof(uint32_t);
      throw StrException_T("AssertException at getcompeltebuf");
    }

    
    // cout<<"_nextGetIndex"<< _nextGetIndex<< "\t clength()"<<clength()<<endl;
    memcpy(dst,_cbuf+_nextGetIndex, length );
    _nextGetIndex += length;    
  }  
  // cout<<"return length :"<<length<<endl;
  return length;
}

bool CharBuffer_T::isCompeleteMsg()
{

 
  if(clength()<getMsgLen()+sizeof(uint32_t ))
    {
      return false;
    }
  else
    {
      // cout<<" Compelete Message Len = "<< getMsgLen() <<endl;
      return true;
    }



}


uint32_t CharBuffer_T::getMsgLen()
{

 
  uint32_t  length=0;

  if (clength() ==0) return length;

  if (clength() <sizeof(uint16_t)+ sizeof(uint32_t))    {

    return 0;
    throw StrException_T(string("DataUnderflow at CharBuffer_T::getMsgLen")
                         + SrcLocation_T(__FILE__, __LINE__));
  }
 
  if (_nextGetIndex > _capacity || _nextGetIndex + sizeof(uint32_t) > _capacity) {
    length=0;
    throw StrException_T("AssertException at getcompeltebuf");
  }

  memcpy(&length,_cbuf+_nextGetIndex, sizeof(uint32_t) );
  // _nextGetIndex +=  sizeof(uint32_t);
  return length;
}


bool CharBuffer_T::getComeleteMsg(void* dst,uint32_t length)
{

  
  if (clength() < sizeof(length)+ length) {
    return false;
    // cout<<" clength() "<<clength() <<" || length "<<length<<endl;
    //throw StrException_T("AssertException at getComeleteMsg");
  }

  _nextGetIndex +=  sizeof(uint32_t);
  memcpy(dst,_cbuf+_nextGetIndex, length );
  _nextGetIndex += length;    

  return true;
}  











CharBuffer_T* CharBuffer_T::get(void* dst, int length)
{
    if (clength() < length || length < 0)
    {
        throw StrException_T(string("DataUnderflow at CharBuffer_T::get")
            + SrcLocation_T(__FILE__, __LINE__));
    }
    Lock_T lock(*this);
    if (_nextGetIndex > _capacity || _nextGetIndex + length > _capacity) {
        throw StrException_T("AssertException at get");
    }
    
    memcpy(dst, _cbuf+_nextGetIndex, length);
    _nextGetIndex += length;
    return this;
}

CharBuffer_T* CharBuffer_T::put(char c)
{        
    if (remaining() < (int)sizeof(c))    {
        throw StrException_T(string("DataOverflow at CharBuffer_T::put")
            + SrcLocation_T(__FILE__, __LINE__));
    }
    Lock_T lock(*this);
    if (_nextPutIndex > _capacity || _nextPutIndex + (int)sizeof(c) > _capacity) {
        throw StrException_T("AssertException at put");
    }
    
    _cbuf[_nextPutIndex++] = c;
    return this;
}

CharBuffer_T* CharBuffer_T::put(void* src, int length)
{    
    if (remaining() < length || length < 0)    {
      //   cout << length << " > " << remaining() << '\t' << clength() << '\t' << capacity() << endl;
        throw StrException_T(string("DataOverflow at CharBuffer_T::put")
            + SrcLocation_T(__FILE__, __LINE__));
    }
    Lock_T lock(*this);
    if (_nextPutIndex > _capacity || _nextPutIndex + length > _capacity) {
        throw StrException_T("AssertException at put");
    }
    
    memcpy(_cbuf+_nextPutIndex, src, length);
    _nextPutIndex += length;
    return this;
}

int CharBuffer_T::remaining()
{
    Lock_T lock(*this);
    return _capacity - _nextPutIndex;
}

CharBuffer_T* CharBuffer_T::alignLeft()
{
    Lock_T lock(*this);
    if (!_nextGetIndex) {
        return this;
    }    
    if (_nextPutIndex != _nextGetIndex) {
        memcpy(_cbuf, _cbuf+_nextGetIndex, _nextPutIndex - _nextGetIndex);
    } 
    _nextPutIndex = _nextPutIndex - _nextGetIndex;
    _nextGetIndex = 0;
        
    return this;
}

