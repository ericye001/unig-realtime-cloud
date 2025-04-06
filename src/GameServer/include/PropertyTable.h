#ifndef _PROPERTY_TABLE_H__
#define _PROPERTY_TABLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct __PropertyInfo
{
	int			nByte;
	const char* pName;
	int			nInd;
	int			nOffset;

	static int& GetCurCount()
	{
		static int nCurCount = 0;
		return nCurCount;
	}
	static int& GetCurOffset()
	{
		static int nCurOffset = 0;
		return nCurOffset;
	}
	
	static int FindItemIndex(  __PropertyInfo* first, int nInfo, const char* pszName )
	{
		for( int i = 0; i < nInfo; ++i )
		{
			if( strcmp(first[i].pName, pszName) == 0 )
				return first[i].nInd;
		}
		return -1;
	}

	static const __PropertyInfo* FindItem( __PropertyInfo* first, int nInfo, const char* pszName )
	{
		for( int i = 0; i < nInfo; ++i )
		{
			if( strcmp(first[i].pName, pszName) == 0 )
				return & first[i];
		}

		return 0;
	}

	static const __PropertyInfo* FindItem( __PropertyInfo* first, int nInfo, int nInd )
	{
		for( int i = 0; i < nInfo; ++i )
		{
			if( first[i].nInd == nInd)
				return &first[i];
		}
		return 0;
	}
};

template<class T>
struct __RegisterInfo
{
	__RegisterInfo(__PropertyInfo& pi, const char* pName, int& nBufLen)
	{
		pi.nByte = sizeof(T);
		nBufLen += pi.nByte + 1/*dirty bit*/;

		pi.nInd = __PropertyInfo::GetCurCount();
		pi.nOffset = __PropertyInfo::GetCurOffset() + 1/*dirty bit*/;
		pi.pName = pName;

		__PropertyInfo::GetCurCount() += 1;
		__PropertyInfo::GetCurOffset() += pi.nByte + 1/*dirty bit*/;
	}

	__RegisterInfo(__PropertyInfo& pi, int count, const char* pName, int& nBufLen)
	{
		pi.nByte = sizeof(T) * count;
		nBufLen += pi.nByte + 1/*dirty bit*/;

		pi.nInd = __PropertyInfo::GetCurCount();
		pi.nOffset = __PropertyInfo::GetCurOffset() + 1/*dirty bit*/;
		pi.pName = pName;

		__PropertyInfo::GetCurCount() += 1;
		__PropertyInfo::GetCurOffset() += pi.nByte + 1/*dirty bit*/;
	}
};

struct __PropertyTableReady
{
	__PropertyTableReady()
	{
		__PropertyInfo::GetCurCount() = 0;	
		__PropertyInfo::GetCurOffset() = 0;
	}
};

class CPropertyTable
{
protected:
	char* m_Property;
private:
    void SetDirtyBitValue(const __PropertyInfo* pInfo, char nc)
    {
        m_Property[pInfo->nOffset - 1] = nc;
    }
    void CleanDirty()
    {
        for( int i = 0; i < GetPropertyCount(); ++i )
        {
		    const __PropertyInfo* pInfo = GetPropertyByInd( i );
            SetDirtyBitValue(pInfo, 0);
        }
    }
public:
    //nDirty,{[nInd,Bytes]....}
    char* GetDirtyData(uint16& nSize, bool bCleanDirty = true)       // 返回值：Dirty属性项数目
    {
        int nDirty = 0;
        nSize = sizeof(nDirty);
        for( int i = 0; i < GetPropertyCount(); ++i )
        {
		    const __PropertyInfo* pInfo = GetPropertyByInd( i );
            if( m_Property[pInfo->nOffset - 1] == 1 )      // dirty
            {
                nDirty++;
                nSize += sizeof(pInfo->nInd);
                nSize += pInfo->nByte;
            }
        }        
        
        char* pBuf = new char[nSize];
        char* pData = pBuf;
        memcpy(pData, &nDirty, sizeof(nDirty));
        pData += sizeof(nDirty);
        
        for( int i = 0; i < GetPropertyCount(); ++i )
        {
		    const __PropertyInfo* pInfo = GetPropertyByInd( i );
            if( m_Property[pInfo->nOffset - 1] == 1 )      // dirty
            {
                memcpy(pData, &(pInfo->nInd), sizeof(pInfo->nInd));
                pData += sizeof(pInfo->nInd);
                memcpy(pData, &(m_Property[pInfo->nOffset]), pInfo->nByte);
                pData += pInfo->nByte;

                if( bCleanDirty )
                    SetDirtyBitValue(pInfo, 0);
            }
        }        
        return pBuf;
    }
    
    void SetDirtyData(char* dd)
    {
        int nDirty;
        int nInd;
        memcpy(&nDirty, dd, sizeof(int));
        dd += sizeof(int);
        for( int i = 0; i < nDirty; ++i )
        {
            memcpy(&nInd, dd, sizeof(int));
            dd += sizeof(int);
		    const __PropertyInfo* pInfo = GetPropertyByInd( nInd );
            //SetDirtyBitValue(pInfo, 1)            // ?
            memcpy(m_Property + pInfo->nOffset, dd, pInfo->nByte);
            dd += pInfo->nByte;
        }
    }
public:
	CPropertyTable()
		:m_Property(0)
	{
	}

	virtual ~CPropertyTable()
	{
	}

	virtual void SetupPropertyTableBuf() 
    {
        CleanDirty();
    };
	virtual void ReleasePropertyTableBuf(){};

	virtual int GetPropertyIndex(const char* name) const { return -1; }
    virtual int GetPropertyCount() const { return 0; }
    virtual int GetPropertyBufLen() const{ return 0; }                       
	virtual const __PropertyInfo* GetPropertyByName( const char* name ) const { return 0; }
	virtual const __PropertyInfo* GetPropertyByInd( int nInd ) const { return 0; }

	template<class Type> void SetPropetryValue(const Type* v, const char* name)	
	{					
		const __PropertyInfo* pInfo = GetPropertyByName( name );

		if( pInfo == 0 )								
			return;									
		memcpy(m_Property + pInfo->nOffset, v, pInfo->nByte);	
        SetDirtyBitValue(pInfo, 1);
	}															
																
	template<class Type> void SetPropetryValue(const Type* v, int nInd)					
	{															
		const __PropertyInfo* pInfo = GetPropertyByInd( nInd );

		if( pInfo == 0 )								
			return;
		memcpy(m_Property + pInfo->nOffset, &v, pInfo->nByte);			
        SetDirtyBitValue(pInfo, 1);
	}

	template<class Type> Type* GetPropertyValue(const char* name, int& nBytes)	
	{													
		const __PropertyInfo* pInfo = GetPropertyByName( name );

		if( pInfo == 0 )								
			return 0;									
		nBytes = pInfo->nByte;
								
		return ((Type*)( m_Property + pInfo->nOffset ));			
	}															
																
	template<class Type> Type* GetPropertyValue(int nInd, int& nBytes)					
	{															
		const __PropertyInfo* pInfo = GetPropertyByInd( nInd );

		if( pInfo == 0 )								
			return 0;									
		nBytes = pInfo->nByte;

		return ((Type*)( m_Property + pInfo->nOffset ));					
	}

	void* GetPropertyValue(const char* name, int& nBytes)	
	{													
		const __PropertyInfo* pInfo = GetPropertyByName( name );

		if( pInfo == 0 )								
			return 0;									

		nBytes = pInfo->nByte;
								
		return ((void*)( m_Property + pInfo->nOffset ));			
	}															
																
	void* GetPropertyValue(int nInd, int& nBytes)					
	{															
		const __PropertyInfo* pInfo = GetPropertyByInd( nInd );

		if( pInfo == 0 )								
			return 0;										
		nBytes = pInfo->nByte;

		return ((void*)( m_Property + pInfo->nOffset ));					
	}
};

// from here...
// 在类里面声明
#define DECLARE_PROTERTY_TABLE(Class,num)				\
	static const int		m_ProCount = num;			\
	static __PropertyInfo	m_PropertyInfo[m_ProCount];	\
	static int				m_BufLen;					\
	virtual void SetupPropertyTableBuf() ;				\
	virtual void ReleasePropertyTableBuf();				\
	virtual int GetPropertyIndex(const char* name) const;		\
    virtual int GetPropertyCount() const;                       \
    virtual int GetPropertyBufLen() const;                       \
	virtual const __PropertyInfo* GetPropertyByName( const char* name ) const;	\
	virtual const __PropertyInfo* GetPropertyByInd( int nInd ) const;	\
	struct __##Class##_PropertyTableReady					\
	{														\
		__##Class##_PropertyTableReady();					\
	};


// cpp文件中实现
#define IMPL_START_DEF_PROPERTY(Class,num)			\
	int				Class::m_BufLen	= 0;			\
	__PropertyInfo	Class::m_PropertyInfo[m_ProCount];		\
	Class::__##Class##_PropertyTableReady::__##Class##_PropertyTableReady()	\
		{													\
			__PropertyInfo::GetCurCount() = 0;				\
			__PropertyInfo::GetCurOffset() = 0;	



// 注册任意类型的属性[int, float, char[10]...]
#define DEF_COMMON_PROPERTY(Class,Type, name)				__RegisterInfo<Type> __RI_##name(Class::m_PropertyInfo[ __PropertyInfo::GetCurCount() ], #name, Class::m_BufLen);
#define DEF_COMMON_ARRAY_PROPERTY(Class,Type, num, name)	__RegisterInfo<Type> __RI_##name(Class::m_PropertyInfo[ __PropertyInfo::GetCurCount() ], num, #name, Class::m_BufLen);


#define IMPL_END_DEF_PROPERTY(Class)														\
	};																						\
static Class::__##Class##_PropertyTableReady __##Class##_PT__;								\
void Class::SetupPropertyTableBuf()															\
{																							\
	m_Property = new char[Class::m_BufLen];													\
    CPropertyTable::SetupPropertyTableBuf();                                                \
}																							\
void Class::ReleasePropertyTableBuf()														\
{																							\
	if( m_Property )																		\
	{																						\
		delete []m_Property;																\
		m_Property = 0;																		\
	}																						\
}																							\
int Class::GetPropertyIndex(const char* name) const											\
{																							\
	return __PropertyInfo::FindItemIndex(Class::m_PropertyInfo, Class::m_ProCount, name);	\
}																							\
int Class::GetPropertyCount() const                                                         \
{                                                                                           \
    return Class::m_ProCount;                                                                      \
}                                                                                           \
int Class::GetPropertyBufLen() const                                                         \
{                                                                                           \
    return Class::m_BufLen;                                                                      \
}                                                                                           \
const __PropertyInfo* Class::GetPropertyByName( const char* name ) const					\
{																							\
	return __PropertyInfo::FindItem(Class::m_PropertyInfo, Class::m_ProCount, name);		\
}																							\
const __PropertyInfo* Class::GetPropertyByInd( int nInd ) const								\
{																							\
    if( nInd >= 0 && nInd < m_ProCount && nInd == m_PropertyInfo[nInd].nInd)                \
        return &m_PropertyInfo[nInd];                                                       \
	return __PropertyInfo::FindItem(Class::m_PropertyInfo, Class::m_ProCount, nInd);		\
}


// 如果父类使用属性表，必须使用一下方式声明

#define DECLARE_SUB_PROTERTY_TABLE(Class, parentclass, num)		\
	static const int m_ProCount = num+parentclass::m_ProCount;	\
	static __PropertyInfo	m_PropertyInfo[m_ProCount];			\
	static int				m_BufLen;							\
	virtual void SetupPropertyTableBuf() ;						\
	virtual void ReleasePropertyTableBuf();						\
	virtual int GetPropertyIndex(const char* name) const;		\
    virtual int GetPropertyCount() const;                       \
    virtual int GetPropertyBufLen() const;                       \
	virtual const __PropertyInfo* GetPropertyByName( const char* name ) const;	\
	virtual const __PropertyInfo* GetPropertyByInd( int nInd ) const;	\
	struct __##Class##_PropertyTableReady						\
	{															\
		__##Class##_PropertyTableReady();						\
	};

#define IMPL_START_SUB_DEF_PROPERTY(Class, parenClass,num)		\
	int				Class::m_BufLen = 0;						\
	__PropertyInfo	Class::m_PropertyInfo[Class::m_ProCount];	\
	Class::__##Class##_PropertyTableReady::__##Class##_PropertyTableReady()	\
		{														\
			__PropertyInfo::GetCurCount() = 0;					\
			__PropertyInfo::GetCurOffset() = 0;					\
			parenClass::__##parenClass##_PropertyTableReady __GenParentProTable; \
			memcpy(Class::m_PropertyInfo, parenClass::m_PropertyInfo, parenClass::m_ProCount * sizeof(__PropertyInfo) ); \
			Class::m_BufLen = parenClass::m_BufLen;

#endif
