#ifndef __OBJECT_CONTAINER_H__
#define __OBJECT_CONTAINER_H__

template<class Key, class Object, class TypeKey, int nTypeNum >
class CObjectContainer
{
public:
    typedef Key     _MyKey;
    typedef Object  _MyObject;
    typedef TypeKey _MyType;

	typedef std::map<Key, Object*>	KEY2OBJECT;
	// all type object pool
	KEY2OBJECT m_ObjectPoolByType[ nTypeNum ];

	typedef std::map< TypeKey, KEY2OBJECT* >  TYPE2POOL;

	// for index by type
	TYPE2POOL m_TypePool;

	CObjectContainer(const CObjectContainer& con);
	CObjectContainer& operator=( const CObjectContainer& con);
public:
	CObjectContainer(){}
	// op
	void InsertObject( const TypeKey& type, const Key& key, Object* pObj )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return;
		(*pool)[key] = pObj;
	}

	void RemoveObject( const TypeKey& type, const Key& key )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return;
		pool->erase( key );
	}

	Object* GetObject( const TypeKey& type, const Key& key )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return;

		typename KEY2OBJECT::iterator it = pool->find( key );
		if( it != pool->end() )
			return it->second;
		return 0;
	}

	void Clean()
	{
		for( int i = 0; i < nTypeNum; ++i )
			m_ObjectPoolByType[i].clear();
	
		m_TypePool.clear();
	}

	// like stl iterator
	typedef typename KEY2OBJECT::iterator iterator;
	typedef typename KEY2OBJECT::const_iterator const_iterator;

	KEY2OBJECT* GetTypeMap(int nInd)
	{
		if( nInd >= nTypeNum )
			return 0;
		return & m_ObjectPoolByType[nInd];
	}

	iterator find( const TypeKey& type,  const Key& key )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();

		return pool->find(key);
	}
	
	const_iterator find( const TypeKey& type,  const Key& key ) const
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();

		return pool->find(key);
	}

	iterator begin(const TypeKey& type )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();

		return pool->begin();
	}

	iterator end(const TypeKey& type )
	{
		KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();
		return pool->end();
	}
	const_iterator begin(const TypeKey& type) const 
	{
		const KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();
		return pool->begin();
	}
	const_iterator end(const TypeKey& type ) const
	{
		const KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return m_ObjectPoolByType[0].end();
		return pool->end();
	}

    uint32 size(const TypeKey& type) const
    {
        const KEY2OBJECT* pool = GetPool( type );
		if( pool == 0 )
			return 0;
		return (uint32)pool->size();
    }

    uint32 total() const
    {
        uint32 size = 0;
        for(int i = 0; i< nTypeNum ; i++)
        {
            size += (uint32)(m_ObjectPoolByType[i].size());
        }
        return size;
    }

    void Clone2List(const TypeKey& type, std::list<Object*>& objlist)
    {
        {
            KEY2OBJECT* pPool = GetPool(type);
            if( pPool )
            {
                typename KEY2OBJECT::iterator it = pPool->begin();
                while( it != pPool->end() )
                {
                    objlist.push_back( it->second );
                    it++;
                }
            }
        }
    }

    void Clone2List(std::list<Object*>& objlist)
    {
        for(int i = 0; i < nTypeNum; ++i)
        {
            typename KEY2OBJECT::iterator it = m_ObjectPoolByType[i].begin();
            while( it != m_ObjectPoolByType[i].end() )
            {
                objlist.push_back( it->second );
                it++;
            }
        }
    }
    //void Safety_For_Each(void (*Object::pfnCallback)())
    //{
    //    std::list<Object*> objlist;
    //    for(int i = 0; i < nTypeNum; ++i)
    //    {
    //        KEY2OBJECT::iterator it = m_ObjectPoolByType[i].begin();
    //        while( it != m_ObjectPoolByType[i].end() )
    //        {
    //            objlist.push_back( it->second );
    //            it++;
    //        }
    //    }

    //    std::list<Object*>::iterator itlist = objlist.begin();
    //    while( itlist != objlist.end() )
    //    {
    //        Object* pObj = *it;
    //        
    //        pObj->*pfnCallback();

    //        it++;
    //    }
    //}

protected:
	KEY2OBJECT* GetPool( const TypeKey& type )
	{
	    typename TYPE2POOL::iterator it = m_TypePool.find( type );
		KEY2OBJECT* pool;
		if( it == m_TypePool.end() )
		{
			if( m_TypePool.size() ==  nTypeNum )
				return 0;
			pool = & (m_ObjectPoolByType[ m_TypePool.size() ] );
			m_TypePool[ type ] = pool;
		}
		else
		{
			pool = it->second;
		}

		return pool;
	}

	const KEY2OBJECT* GetPool( const TypeKey& type ) const
	{
	    typename TYPE2POOL::const_iterator it = m_TypePool.find( type );
		const KEY2OBJECT* pool;
		if( it == m_TypePool.end() )
		{
			return 0;
		}
		else
		{
			pool = it->second;
		}

		return pool;
	}
};

template<int GridNum, class Vector, class Region, class ObjectPool>
class CManagerGrid
{
public:	
	Region*		m_pRegion;
	ObjectPool	m_Pools[GridNum][GridNum];
public:
	void SetParent( Region* pRegion )
	{
		m_pRegion = pRegion;
	}

    uint32 size(const typename ObjectPool::_MyType& type) const
    {
        uint32 size = 0;
        for( int i = 0; i < GridNum; ++i )
            for( int j = 0; j < GridNum; ++j )
            {
                size += m_Pools[i][j].size(type);
            }
        return size;
    }

    uint32 total() const
    {
        uint32 size = 0;
        for( int i = 0; i < GridNum; ++i )
            for( int j = 0; j < GridNum; ++j )
            {
                size += m_Pools[i][j].total();
            }
        return size;
    }

	ObjectPool* GetPool( const Vector& vPos )
	{
		float fXRegionOff = vPos.x - m_pRegion->m_X;
		float fZRegionOff = vPos.z - m_pRegion->m_Z;
		int row, col;

		row = (int)( GridNum * fZRegionOff / m_pRegion->m_ZLen );
		col = (int)( GridNum * fXRegionOff / m_pRegion->m_XLen );

		return & m_Pools[row][col];
	}

	void Clean()
	{
		m_pRegion = NULL;
		for( int i = 0; i < GridNum; ++i )
			for( int j = 0; j < GridNum; ++j )
				m_Pools[i][j].Clean();
	}

public:
};
#endif
