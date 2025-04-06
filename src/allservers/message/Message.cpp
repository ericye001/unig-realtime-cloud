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

#include "message/Message.h"


BasePacket_T::BasePacket_T(bool bindOn)
    : _length(0),
    _data(0), 
    _rindexIn(0), 
    _rindexOut(0),
    _bindOn(bindOn)
{

}

BasePacket_T::BasePacket_T(Connection_T* pConn, bool bindOn)
    : _length(0),
    _data(0), 
    _rindexIn(0), 
    _rindexOut(0),
    _bindOn(bindOn)
{
    char* data = (char*)pConn->cursor();
    memcpy(&_length, data, sizeof(_length));
    data += sizeof(_length);
    _data = data;
}



BasePacket_T::BasePacket_T(void* pData, uint32_t length, bool bindOn)
    : _length(length),
    _data((char*)pData), 
    _rindexIn(0), 
    _rindexOut(0),
    _bindOn(bindOn)
{

}

BasePacket_T::~BasePacket_T()
{
    reset();   
}







bool BasePacket_T::getInputPacket(NonBlockConnection_T* pConn, bool bindOn)

{
  _length=0;
  _data=0; 
  _rindexIn=0; 
  _rindexOut=0;
  _bindOn=bindOn;
  _length= pConn->getInputBuffer()->getMsgLen();
  //cout<<"pConn->getInputBuffer()->getMsgLen()="<< _length<<endl;
  if(_length<sizeof(uint16_t)) return false;
  reserve(_length);
  if(pConn->getInputBuffer()->getComeleteMsg(_data,_length))
    {
      //cout<<" getComeleteMsg true "<<endl;
      return true;
    }
  else
    {
      // cout<<" getComeleteMsg false "<<endl;
      return false;

    }
}




bool BasePacket_T::getOutputPacket(NonBlockConnection_T* pConn, bool bindOn)

{
  _length=0;
  _data=0; 
  _rindexIn=0; 
  _rindexOut=0;
  _bindOn=bindOn;
  _length= pConn->getOutputBuffer()->getCompeleteBuf(_data);
  if(_length==0) return false;
  else return true;
}


bool BasePacket_T::putOutputBuffer(NonBlockConnection_T* pConn)
{

 
  pConn->getOutputBuffer()->alignLeft();
  // cout<<" A :BasePacket_T::putOutputBuffer(NonBlockConnection_T* pConn)  _length+sizeof(_length) :"<< _length+sizeof(_length) <<endl;
  // cout<<" A :pConn->getOutputBuffer()->remaining()"<< pConn->getOutputBuffer()->remaining() <<endl;
  if((pConn->getOutputBuffer()->remaining()) < int(_length+sizeof(_length)))
    {        
      return false;
    }
  else{
   
    pConn->getOutputBuffer()->put(&_length, sizeof(_length));
    //cout<<" pConn->getOutputBuffer() "<< _length <<endl;
     
    pConn->getOutputBuffer()->put(_data,_length);

    //cout<<" pConn->getOutputBuffer() "<<  pConn->getOutputBuffer()->clength()  <<endl;
    return true;
  }
  
  
}





bool BasePacket_T::containsOne(Connection_T* pConn)
{
    if (pConn->length() < sizeof(uint32_t))
    {
        return false;
    }
    uint32_t dataLen;
    memcpy(&dataLen, pConn->cursor(), sizeof(dataLen));
    return pConn->length() >= dataLen+sizeof(uint32_t);
}

void BasePacket_T::push(void* pData, uint32_t length)
{
    if (_rindexIn+length > _length)
    {
        throw StrException_T(
            string("Memory overflow\n"
                "\tat BasePacket_T::push().")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    
    memcpy(_data+_rindexIn, pData, length);
    _rindexIn += length;
}


void BasePacket_T::pop(void* pData, uint32_t length)
{
    if (_rindexOut+length > _length)
    {
        throw StrException_T(
            string("Memory overflow\n"
                "\tat BasePacket_T::pop().")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    memcpy(pData, _data+_rindexOut, length);
    _rindexOut += length;
}


void BasePacket_T::write(SocketStream_T* pStream)
{    
   if( !_length )
    cout<<" BasePacket_T::write(SocketStream_T* pStream) length "<< _length <<endl;
  pStream->writeBytes(&_length, sizeof(_length));
  pStream->writeBytes(_data, _length);    
}
void BasePacket_T::write(Connection_T* pConn)
{    
    SocketStream_T stream = pConn->stream();
    write(&stream);
}

void BasePacket_T::read(SocketStream_T* pStream)
{
    pStream->readBytes(&_length, sizeof(_length));   
    if(_length)
      {
        // cout<<"BasePacket_T::read(SocketStream_T* pStream) leng "<<_length <<endl;  
        reserve(_length);
        pStream->readBytes(_data, _length);
      }
    else
      {
        cout<<"BasePacket_T::read(SocketStream_T* pStream) length "<<_length <<endl;
      }
}

void BasePacket_T::read(Connection_T* pConn)
{
    SocketStream_T stream = pConn->stream();
    read(&stream);
}


void BasePacket_T::read(IOStream_T* pStream)
{
 
    pStream->readBytes(&_length, sizeof(_length));
    reserve(_length);
    pStream->readBytes(_data, _length);
}

void BasePacket_T::write(IOStream_T* pStream)
{    
    pStream->writeBytes(&_length, sizeof(_length));
    pStream->writeBytes(_data, _length);    
}

void BasePacket_T::reserve(uint32_t length)
{
    if (_data)
    {
        throw StrException_T(
            string("Not NULL data ptr found, reset requested first\n"
                "\tat BasePacket_T::reserve")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    _length = length;
    _data = new char[_length];
    if (!_data)
    {
        throw StrException_T(
            string("Fail to malloc char[]\n"
                "\tat BasePacket_T::reserve")+
            SrcLocation_T(__FILE__, __LINE__));
    }
}

uint32_t BasePacket_T::length()
{
    return _length;
}

char* BasePacket_T::data()
{
    return _data;
}

char* BasePacket_T::data(uint32_t& length)
{
    length = _length;
    return _data;
}

void BasePacket_T::data(void* data, uint32_t length)
{
    if (_data)
    {
        throw StrException_T(
            string("Not NULL data ptr found, reset requested first\n"
                "\tat BasePacket_T::data")+
            SrcLocation_T(__FILE__, __LINE__));
    }
    _data = (char*)data;
    _length = length;
}

void BasePacket_T::bind(bool bindOn)
{
    _bindOn = bindOn;
}

void BasePacket_T::reset()
{
    if (_bindOn)
    {
        if (_data)
        {
            delete [] _data;
        }
        _data = 0;
        _length = 0;
    }
}

//------------------------------------------------------------------------
BaseMessage_T::BaseMessage_T()
    : _type(0)
{
    
}

BaseMessage_T::~BaseMessage_T()
{
 
}

void BaseMessage_T::unpack(BasePacket_T* pkt)
{
    pkt->pop(&_type, sizeof(_type));

}

void BaseMessage_T::pack(BasePacket_T* pkt)
{
    pkt->reserve(size());
    pkt->push(&_type, sizeof(_type));
 
}

uint32_t BaseMessage_T::size()
{
    return sizeof(_type);
}

uint16_t BaseMessage_T:: type(BasePacket_T* pkt)
{
    uint16_t type;
    memcpy(&type, pkt->data(), sizeof(type));
    return type;
}



void BaseMessage_T::write(Connection_T* pConn)
{
  //cout<<"void BaseMessage_T::write(Connection_T* pConn) "<<endl;
  SocketStream_T stream = pConn->stream();
  write(&stream);
}

void BaseMessage_T::read(Connection_T* pConn)
{
    SocketStream_T stream = pConn->stream();
    read(&stream);
}

void BaseMessage_T::write(NonBlockConnection_T* pConn)
{
  // IOStream_T* stream = pConn->ioStream();
  //  write(stream);
  //cout<<"void BaseMessage_T::write(NonBlockConnection_T* pConn) "<<endl;
  BasePacket_T pkt;
  pack(&pkt);
  pkt.putOutputBuffer(pConn);

}


void BaseMessage_T::write(SocketStream_T* pStream)
{
    BasePacket_T pkt;
    pack(&pkt);
    pkt.write(pStream);
}

void BaseMessage_T::read(SocketStream_T* pStream)
{
    BasePacket_T pkt;
    pkt.read(pStream);
    unpack(&pkt);
}

void BaseMessage_T::write(IOStream_T* ioStream)
{
    BasePacket_T pkt;
    pack(&pkt);
    pkt.write(ioStream);
}

void BaseMessage_T::read(IOStream_T* ioStream)
{
    BasePacket_T pkt;
    pkt.read(ioStream);
    unpack(&pkt);
}


