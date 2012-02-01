/*
 *  test.cpp
 *  sqlstl
 *
 *  Created by Shakthi Prasad G S on 09/01/12.
 *  Copyright 2012 Shakthi. All rights reserved.
 *
 */

#include "test.h"
#include "sqlstl.h"
#include <string>
#include <set>
#include <map>
#include <cassert>
#include <tr1/tuple>
using namespace std;

using namespace std::tr1;

struct tableForiegnBase
{

};



struct tableReferedBy
{
	virtual bool OnDelete(tableForiegnBase * deleter,void * foreignKeyPtr)=0;
};



template <class tableTupleType>
struct tableForiegn:tableForiegnBase
{
	typedef typename tableTupleType::primaryKeyType primaryKeyType;	
	std::set<tableReferedBy * > referedTables;
	
	virtual bool contains(primaryKeyType fk)=0;
	
	 bool hasPrimaryKeyType(primaryKeyType *)
	{
		return true;
	}
};





template <typename _primaryKeyType,typename _foreignKeytuple=void>
struct tableTuple {
	
	
	typedef _primaryKeyType primaryKeyType;
	typedef _foreignKeytuple  foreignKeysType; 
	
	primaryKeyType primaryKey;
	foreignKeysType foreignKeys;
	
	bool operator < (const tableTuple & u)const
	{
		return primaryKey < u.primaryKey;
	}
	
	
	tableTuple(primaryKeyType k,const foreignKeysType & fk)
	:primaryKey(k),foreignKeys(fk)
	{
		
	}
	
	
	bool validate()
	{
		return true;
	}

	
	template<typename foreignTablesType,int index>
	struct validateForeignKeyHelper 
	{
		static bool validateForeignKey(foreignTablesType inForeignTable,foreignKeysType inForeignKeys)
		{
			
			typename std::tr1::tuple_element<index-1,foreignTablesType>::type foreignTable = get<index-1>(inForeignTable);
			
			
			typedef typename std::tr1::tuple_element<index-1,foreignKeysType>::type foreignKeyType;
			
			const foreignKeyType foreignKey = get<index-1>(inForeignKeys);
			
			
			if(foreignTable->contains(foreignKey) == false)
		    	return false;
			
			return validateForeignKeyHelper<foreignTablesType,index-1>::validateForeignKey(inForeignTable,inForeignKeys);

			
		}
		
		static bool validateForeignKeyType(foreignTablesType inForeignTables)
		{
			typedef typename std::tr1::tuple_element<index-1,foreignTablesType>::type foreignTableType;
			foreignTableType foreignTable = get<index-1>(inForeignTables);
			
			
			typedef typename std::tr1::tuple_element<index-1,foreignKeysType>::type foreignKey;
			
			
			return foreignTable->hasPrimaryKeyType((foreignKey*)0);
		}
		
		
		
	};
	
	
	
	
	template<typename foreignTablesType>
	struct validateForeignKeyHelper<foreignTablesType,0>
	{
		static bool validateForeignKey(foreignTablesType foreignTables,foreignKeysType inForeignKeys)
		{
			return true;
			
		}
		
		
		static bool validateForeignKeyType(foreignTablesType inForeignTable)
		{
			return true;
			
		}
		
		
	};
	
	
	
	template<int index>
	typename tuple_element<index,foreignKeysType>::type	getForiegKeyForIndex()
	{
		return  get<index>(foreignKeys);

	}
	
	
	
	
	
	template <typename  foreignTablesType >
	bool validateForeignKey(foreignTablesType & foreignTables)
	{
		const size_t tuplesize =tuple_size<foreignTablesType>::value;
		return validateForeignKeyHelper<foreignTablesType,tuplesize>::validateForeignKey(foreignTables,foreignKeys);
	}

	
	
	template <typename  foreignTablesType >
	static bool validateForeignKeyType(foreignTablesType & foreignTables)
	{
		const size_t tuplesize =tuple_size<foreignTablesType>::value;
		return validateForeignKeyHelper<foreignTablesType,tuplesize>::validateForeignKeyType(foreignTables);
	}
	
	
	
	
};




template <typename _primaryKeyType>
struct tableTuple <_primaryKeyType,void>{
	
	
	typedef _primaryKeyType primaryKeyType;
	
	primaryKeyType primaryKey;
	
	bool operator < (const tableTuple & u)const
	{
		return primaryKey < u.primaryKey;
	}
	
	
	tableTuple(primaryKeyType k)
	:primaryKey(k)
	{
		
	}
	
	
	bool validate()
	{
		return true;
	}
	
	static bool validateForeignKeyType()
	{
		return true;
	}
	
	
	
	
};





template <typename _tableTupleType,typename __foreignTablesType=void>
struct table:map<typename _tableTupleType::primaryKeyType,_tableTupleType> ,tableReferedBy,tableForiegn<_tableTupleType>
{
	
	typedef __foreignTablesType foreignTablesType;
	typedef _tableTupleType tableTupleType;
	
	
	foreignTablesType foreignTables;
	typedef table<tableTupleType,foreignTablesType> selfType;
	typedef typename map<typename tableTupleType::primaryKeyType,tableTupleType>::iterator iterator;
	typedef map<typename tableTupleType::primaryKeyType,tableTupleType> mapType;
	
	typedef typename tableTupleType::primaryKeyType primaryKeyType;	
	template<typename _foreignTablesType,size_t index>
	struct setForeignTableReferenceHelper;
	
	template<int index>
	struct foreignKeyAt 
	{
		typedef typename tuple_element<index,typename tableTupleType::foreignKeysType>::type type;
		static type getForeignKey(iterator i)
		{
			const tableTupleType & tableTuple = i->second;
			return get<index>(tableTuple.foreignKeys);
		}
	};
	
	template<typename _foreignTablesType>
	struct setForeignTableReferenceHelper<_foreignTablesType,0> 
	{
		static void setForeignTableReference(_foreignTablesType intupleforiegn,selfType * inself)
		{
		}
		
	};
	
	
	
	template<typename _foreignTablesType,size_t i>
	struct setForeignTableReferenceHelper 
	{
		static void setForeignTableReference(_foreignTablesType intupleforiegn,selfType * inself)
		{
			typename std::tr1::tuple_element<i-1,_foreignTablesType>::type foreignTable = get<i-1>(inself->foreignTables);
			foreignTable->referedTables.insert(inself);
			setForeignTableReferenceHelper<_foreignTablesType,i-1>::setForeignTableReference(intupleforiegn,inself);
		}
		
	};
	
	

		
	
	
	
	
	
	table(foreignTablesType inForeignTables)
	:foreignTables(inForeignTables)
	{
		const size_t size=  tuple_size<foreignTablesType>::value;
		setForeignTableReferenceHelper<foreignTablesType,size >::setForeignTableReference(inForeignTables,this);
		
		tableTupleType::validateForeignKeyType(inForeignTables);

	
	}
	
	iterator findTuple(primaryKeyType key)
	{
		
#if DEBUG		
		for(iterator i =mapType::begin();  i !=mapType::end(); i++)
		{
			tableTupleType rowss=i->second;
			
			if(key == i->first)
				return i;
			
			
		}
#endif		
		
		
		return mapType::find(key);
	}
	
	
	
	bool contains(primaryKeyType key)
	{
		return findTuple(key) != mapType::end();
	}
	
	
	
		

	
	
	template<class _foreignTableType,typename _foreignKeyType>
	bool  OnDelete(_foreignKeyType fk,const _foreignTableType &treftable,iterator i)
	{
		mapType::erase (i);
		return true;
	} 
	
	
	
	
	template<typename _foreignTablesType,int index>
	struct OnDeleteHelper 
	{
		
		static bool  OnDelete(tableForiegnBase * inForeignTable,void * foreignTableKeyPtr,_foreignTablesType  foreignTables,selfType * _self)
		{
			typedef typename tuple_element<index-1,_foreignTablesType>::type foreignTableType;
			
			tableForiegnBase*  foreignTable = static_cast<tableForiegnBase*>(get<index-1>(foreignTables));
			
			
			
			if(foreignTable == inForeignTable)
			{
				
				for(iterator i =_self->mapType::begin();  i !=_self->mapType::end(); i++)
				{
					
					
					typedef typename foreignKeyAt<index-1>::type foreignKeyType;
					
					foreignKeyType foreignKey = foreignKeyAt<index-1>::getForeignKey(i);
					
					foreignKeyType foreignTableKey = *(static_cast<foreignKeyType*>(foreignTableKeyPtr)); 
					
					if(foreignTableKey == foreignKey)
					{
						
						if(_self->OnDelete(foreignTableKey,get<index-1>(foreignTables),i) ==false)
							return false;
						
						break;
					}
					
				}	
				
				
				
			}
			
			
			
			
			return OnDeleteHelper<foreignTablesType,index-1 >::OnDelete(foreignTable,foreignTableKeyPtr,foreignTables,_self);

			
		}
		
	};
	
	
	
	
	
	template<typename _foreignTablesType>
	struct OnDeleteHelper<_foreignTablesType,0> 
	{
		
		static bool  OnDelete(tableForiegnBase * inForeignTable,void * foreignTableKeyPtr,_foreignTablesType  foreignTables,selfType * _self)
		{
			return true;
		}
		
	};
	
	
	
	virtual bool OnDelete(tableForiegnBase * inForeignTable,void * inForeignKeyPtr)
	{
		const size_t size=  tuple_size<foreignTablesType>::value;
		return OnDeleteHelper<foreignTablesType,size >::OnDelete(inForeignTable,inForeignKeyPtr,foreignTables,this);
		return true;
	}
	
	
	
	
	
	
	
	bool insert(tableTupleType tableTuple)
	{
		
		if(tableTuple.validate() == false)
			return false;
		
		
		pair<iterator,bool> a = mapType::insert (make_pair(tableTuple.primaryKey, tableTuple));
		
		if(a.second==false)
			return false;
		
		
		if(tableTuple.validateForeignKey(foreignTables) == false)
		{
			mapType::erase(tableTuple.primaryKey);
			return false;
		}	
		
		return true;
	}
	
	
	
	bool deleteKey(typename tableTupleType::primaryKeyType key)
	{
		
		assert(contains(key));
		
		typedef typename std::set<tableReferedBy * >::iterator referediter;
		
		for(referediter i= tableForiegn<tableTupleType>::referedTables.begin(); i!= tableForiegn<tableTupleType>::referedTables.end(); i++)
		{
			if((*i)->OnDelete(this,&key) == false)
				return false;
		}
		
		iterator k=findTuple(key);
		if(k!=mapType::end())
			mapType::erase (k);

		return true;
	}

	
};



template <typename _tableTupleType>
struct table<_tableTupleType,void>:map<typename _tableTupleType::primaryKeyType,_tableTupleType> 
,tableReferedBy,tableForiegn<_tableTupleType>
{
	
	typedef _tableTupleType tableTupleType;
	
	
	typedef table<tableTupleType> selfType;
	typedef typename map<typename tableTupleType::primaryKeyType,tableTupleType>::iterator iterator;
	typedef map<typename tableTupleType::primaryKeyType,tableTupleType> mapType;
	
	typedef typename tableTupleType::primaryKeyType primaryKeyType;	
	template<typename _foreignTablesType,size_t index>
	struct setForeignTableReferenceHelper;
	
	template<int index>
	struct foreignKeyAt 
	{
		typedef typename tuple_element<index,typename tableTupleType::foreignKeysType>::type type;
		static type getForeignKey(iterator i)
		{
			const tableTupleType & tableTuple = i->second;
			return get<index>(tableTuple.foreignKeys);
		}
	};
	
	template<typename _foreignTablesType>
	struct setForeignTableReferenceHelper<_foreignTablesType,0> 
	{
		static void setForeignTableReference(_foreignTablesType intupleforiegn,selfType * inself)
		{
		}
		
	};
	
	
	
	template<typename _foreignTablesType,size_t i>
	struct setForeignTableReferenceHelper 
	{
		static void setForeignTableReference(_foreignTablesType intupleforiegn,selfType * inself)
		{
			typename std::tr1::tuple_element<i-1,_foreignTablesType>::type foreignTable = get<i-1>(inself->foreignTables);
			foreignTable->referedTables.insert(inself);
			setForeignTableReferenceHelper<_foreignTablesType,i-1>::setForeignTableReference(intupleforiegn,inself);
		}
		
	};
	
	
	
	
	
	
	virtual bool OnDelete(tableForiegnBase * deleter,void * foreignKeyPtr)
	{
		return false;
	}

	
	
	table()
	{
		tableTupleType::validateForeignKeyType();
		
		
	}
	
	iterator findTuple(primaryKeyType key)
	{
		
#if DEBUG		
		for(iterator i =mapType::begin();  i !=mapType::end(); i++)
		{
			tableTupleType rowss=i->second;
			
			if(key == i->first)
				return i;
			
			
		}
#endif		
		
		
		return mapType::find(key);
	}
	
	
	
	bool contains(primaryKeyType key)
	{
		return findTuple(key) != mapType::end();
	}
	
	
	
		
	
	
	
	
	
	bool insert(tableTupleType tableTuple)
	{
		
		if(tableTuple.validate() == false)
			return false;
		
		
		pair<iterator,bool> a = mapType::insert (make_pair(tableTuple.primaryKey, tableTuple));
		
		if(a.second==false)
			return false;
		
		
		return true;
	}
	
	
	
	bool deleteKey(typename tableTupleType::primaryKeyType key)
	{
		
		assert(contains(key));
		
		typedef typename std::set<tableReferedBy * >::iterator referediter;
		
		for(referediter i= tableForiegn<tableTupleType>::referedTables.begin(); i!= tableForiegn<tableTupleType>::referedTables.end(); i++)
		{
			if((*i)->OnDelete(this,&key) == false)
				return false;
		}
		
		iterator k=findTuple(key);
		if(k!=mapType::end())
			mapType::erase (k);
		
		return true;
	}
	
	
};


#define SHOULD_PASS(x) assert((x) );
#define SHOULD_FAIL(x) assert(!(x) );



typedef tuple<> tableTuplei;

struct Player2:tableTuple<int,tableTuplei > {
	int playerid()
	{
		return primaryKey;
	}
	
	Player2(int playerid,string name)
	:tableTuple<int,tableTuplei>(playerid,make_tuple()),name(name)
	{}
	string name;
};




struct Player:tableTuple<int> {
	int playerid()
	{
		return primaryKey;
	}
	
	Player(int playerid,string name)
	:tableTuple<int>(playerid),name(name)
	{}
	string name;
};






struct Score2:tableTuple<int,tuple<int> > {
	
	Score2(int score,int  playerid)
	:tableTuple<int,tuple<int> >(score,make_tuple(playerid))
	{}
};



template<typename tabletype,typename _Function>
struct tableview 
{
	 const tabletype & atable;
	
	_Function fun;
	
	tableview ( tabletype & inTable,_Function infun )
	:atable(inTable),fun(infun)
	{
	
	}
	
	struct tableview_iterator:tabletype::const_iterator {
		typedef tableview_iterator                _Self;
		typedef typename tabletype::const_iterator baseIterator;
		typedef std::forward_iterator_tag    iterator_category;
			
		typedef typename baseIterator::difference_type   difference_type;
		typedef typename baseIterator::pointer   pointer;
		typedef typename baseIterator::reference   reference;	
		baseIterator baseend;	
		
		_Function _fun;
		
			tableview_iterator()
			: baseIterator() { }
			
			
			tableview_iterator(baseIterator __x,baseIterator enditer,_Function inFun)
			: baseIterator(__x),baseend(enditer),_fun(inFun) { }
			
		
			bool valid()
			{
				return _fun((*this)->second);
			}

		
			void advanceOver()
			{
				while(*this != baseend && valid()== false  )
				{
					baseIterator::operator++();
				}	

			
			}
			
		
			_Self&
			operator++()
			{
				baseIterator::operator++();
				advanceOver();	
				
				return *this;
			}
		
			
			_Self
			operator++(int)
			{
				_Self __tmp = *this;
				_Self::operator++();
				return __tmp;
			}
//			
	
	};
	
	
	typedef  tableview_iterator iterator;

	iterator begin()const
	{
		tableview_iterator i = tableview_iterator(atable.begin(),atable.end(),fun);
		i.advanceOver();
		return i;
	
	}
	
	
	iterator end()const
	{
		return tableview_iterator(atable.end(),atable.end(),fun);
		
	}
	

};






bool  nameequaltoshakthi(const Player2 & invalue)
{

	return invalue.name == "shakthi";
}


int main()
{
	
	table<Player2,tuple<> > b(make_tuple());
	
	
	table<Score2,tuple<tableForiegn<Player2>* > > s(make_tuple(&b));
	
	SHOULD_PASS(b.insert(Player2(1,"shakthi")));
	SHOULD_PASS(b.insert(Player2(4,"ganesh")));
	SHOULD_PASS(b.insert(Player2(6,"prasad")));
	
	
	tableview<table<Player2,tuple<> >,bool (*)(const Player2& )> view(b,nameequaltoshakthi);
	
	typedef  tableview<table<Player2,tuple<> >,bool (*)(const Player2 &)>::iterator titer;
	 
	
	for (titer i= view.begin(); i!= view.end(); i++) {

	}
	
	
	
	SHOULD_PASS(s.insert(Score2(100,1)));
				
	SHOULD_FAIL(s.insert(Score2(200,2)));
	SHOULD_PASS(s.insert(Score2(200,6)));
	
	SHOULD_PASS(b.deleteKey(1));
	SHOULD_PASS(s.insert(Score2(300,4)));
	
	

	return 0;
}