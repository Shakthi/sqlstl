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



struct tableReferedByP
{
	
	virtual bool OnDeleteinRefered(tableForiegnBase * deleter,void * fk)=0;
	
};



template <class row>

struct tableForiegn:tableForiegnBase
{
	typedef typename row::primaryKeyType pprimaryKeyType;	
	std::set<tableReferedByP * > referset;
	
	virtual bool contains(pprimaryKeyType fk)=0;
	
	 bool primaryprimaryKeyType(pprimaryKeyType *)
	{
		return true;
	}
};





template <typename _primaryKeyType,typename _foreignKeytuple>
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
	
	
//	template<typename tupleforiegntype,int index,typename ftype>
//	struct checkhelper;	
	
	
	template<typename tupleforiegntype,int index,typename fktype>
	struct checkhelper {
		static bool foreignKeyvalidatef(tupleforiegntype intupleforiegn,fktype fk)
		{
			
			typename std::tr1::tuple_element<index-1,tupleforiegntype>::type foriengtable = get<index-1>(intupleforiegn);
			
			
			typedef typename std::tr1::tuple_element<index-1,foreignKeysType>::type fk1type;
			
			const fk1type  fk1= get<index-1>(fk);
			
			
			if(foriengtable->contains(fk1) == false)
		    	return false;
			
			return checkhelper<tupleforiegntype,index-1,fktype>::foreignKeyvalidatef(intupleforiegn,fk);

			
		}
		
		static bool foreignKeyvalidatefType(tupleforiegntype intupleforiegn)
		{
			
			typename std::tr1::tuple_element<index-1,tupleforiegntype>::type foriengtable = get<index-1>(intupleforiegn);
			
			
			typedef typename std::tr1::tuple_element<index-1,tupleforiegntype>::type ffsff;
			
//			typedef typename dref<ffsff>::type ttt; 
//			
//			
			typedef typename std::tr1::tuple_element<index-1,foreignKeysType>::type fk1type;
			
			
			return foriengtable->primaryprimaryKeyType((fk1type*)0);
			
			
	//		const fk1type  fk1= get<index-1>(fk);
//			
//			
//			if(foriengtable->contains(fk1) == false)
//		    	return false;
			
	//		return checkhelper<tupleforiegntype,index-1,fktype>::foreignKeyvalidatef(intupleforiegn,fk);
			
			
		}
		
		
		
	};
	
	
	
	
	template<typename tupleforiegntype,typename fktype>
	struct checkhelper<tupleforiegntype,0, fktype> {
		static bool foreignKeyvalidatef(tupleforiegntype intupleforiegn,fktype fk)
		{
			return true;
			
		}
		
		
		static bool foreignKeyvalidatefType(tupleforiegntype intupleforiegn)
		{
			return true;
			
		}
		
		
	};
	
	
	
	template<int index>
	typename tuple_element<index,foreignKeysType>::type	getForiegKeyForIndex()
	{
		return  get<index>(foreignKeys);

	}
	
	
	
	
	
	template <typename  refrencedtabletype >
	bool foreignKeyvalidate(refrencedtabletype & refrencedtable)
	{
		const size_t tuplesize =tuple_size<refrencedtabletype>::value;
		return checkhelper<refrencedtabletype,tuplesize,foreignKeysType>::foreignKeyvalidatef(refrencedtable,foreignKeys);
	}

	
	
	template <typename  refrencedtabletype >
	static bool foreignKeyvalidateType(refrencedtabletype & refrencedtable)
	{
		const size_t tuplesize =tuple_size<refrencedtabletype>::value;
		return checkhelper<refrencedtabletype,tuplesize,foreignKeysType>::foreignKeyvalidatefType(refrencedtable);
	}
	
	
	
	
};





template <typename tableTupleType,typename tupletypeforiegntype>
struct table:map<typename tableTupleType::primaryKeyType,tableTupleType> ,tableReferedByP,tableForiegn<tableTupleType>
{
	
	tupletypeforiegntype tupleforiegn;
	typedef table<tableTupleType,tupletypeforiegntype> selftype;
	typedef typename map<typename tableTupleType::primaryKeyType,tableTupleType>::iterator tablerowiter;
	typedef map<typename tableTupleType::primaryKeyType,tableTupleType> tableype;
	
	template<typename tupleforiegntype,size_t i,typename selftypet>
	struct setTupleReferencehelper;
	
	template<int index>
	struct fkatindex {
		typedef typename tuple_element<index,typename tableTupleType::foreignKeysType>::type type;
		
		static type getfk(tablerowiter i)
		{
			tableTupleType row = i->second;
			
			
			return get<index>(row.foreignKeys);
		}
	};
	
	template<typename tupleforiegntype,typename selftypet>
	struct setTupleReferencehelper<tupleforiegntype,0,selftypet> {
		static void setTupleReference(tupleforiegntype intupleforiegn,selftypet * inself)
		{
			
			
		}
		
	};
	
	
	
	template<typename tupleforiegntype,size_t i,typename selftypet>
	struct setTupleReferencehelper {
		static void setTupleReference(tupleforiegntype intupleforiegn,selftypet * inself)
		{
			
			typename std::tr1::tuple_element<i-1,tupleforiegntype>::type foriengtable = get<i-1>(inself->tupleforiegn);
			
			foriengtable->referset.insert(inself);
			
			
			typedef typename tableTupleType::foreignKeysType fkt;
			setTupleReferencehelper<tupleforiegntype,i-1,selftypet>::setTupleReference(intupleforiegn,inself);
			
			

			
		}
		
	};
	
	

		
	
	
	
	
	
	table(tupletypeforiegntype intupleforiegn)
	:tupleforiegn(intupleforiegn)
	{
		const size_t size=  tuple_size<tupletypeforiegntype>::value;
		setTupleReferencehelper<tupletypeforiegntype,size,selftype >::setTupleReference(intupleforiegn,this);
		
		tableTupleType::foreignKeyvalidateType(intupleforiegn);

	
	}
	
	tablerowiter findrow(typename tableTupleType::primaryKeyType key)
	{
		
#if DEBUG		
		for(tablerowiter i =tableype::begin();  i !=tableype::end(); i++)
		{
			tableTupleType rowss=i->second;
			
			if(key == i->first)
				return i;
			
			
		}
#endif		
		
		
		return tableype::find(key);
	}
	
	
	
	bool contains(typename tableTupleType::primaryKeyType key)
	{
		return findrow(key) != tableype::end();
	}
	
	
	
	
	
	template<class T>
	void OnDelete(typename T::pprimaryKeyType fk,const T &treftable,tablerowiter i)
	{
		tableype::erase (i);
	} 
	
	
//	template<int index>
//	typename tuple_element<index,foriegntableTupleType>::type	getForiegKeyForIndex()

	
	
	template<class T,typename fktype>
	bool  OnDelete(fktype fk,const T &treftable,tablerowiter i)
	{
		tableype::erase (i);
		return true;
	} 
	
	
	
	
	template<typename tupleforiegntype,int index,typename _selftype>
	struct OnDeleteHelper {
		
		static bool  OnDeleteinRefered(tableForiegnBase * deleter,void * fk,tupleforiegntype  ft,_selftype * _self)
		{
			typedef typename tuple_element<index-1,tupleforiegntype>::type tt;
			
			tableForiegnBase*  table = static_cast<tableForiegnBase*>(get<index-1>(ft));
			
			//printf("sucess %x %x\n",table,deleter);
			
			
			if(table == deleter)
			{
				
				for(tablerowiter i =_self->tableype::begin();  i !=_self->tableype::end(); i++)
				{
					tableTupleType rowss=i->second;
					
					//printf("sucess\n");
					
					typedef typename fkatindex<index-1>::type t;
					
					t fk0= fkatindex<index-1>::getfk(i);
					
					t converted = *(static_cast<t*>(fk)); 
					
					if(converted==fk0)
					{
						
						if(_self->OnDelete(converted,get<index-1>(ft),i) ==false)
							return false;
						
						break;
					}
					
				}	
				
				
				
			}
			
			
			
			
			return OnDeleteHelper<tupletypeforiegntype,index-1,selftype >::OnDeleteinRefered(deleter,fk,ft,_self);

			
		}
		
	};
	
	
	
	template<typename tupleforiegntype,typename _selftype>
	struct OnDeleteHelper<tupleforiegntype,0,_selftype> {
		
		static bool  OnDeleteinRefered(tableForiegnBase * deleter,void * fk,tupleforiegntype  ft,_selftype * _self)
		{
			return true;
		}
		
	};
	
	
	
	virtual bool OnDeleteinRefered(tableForiegnBase * deleter,void * fk)
	{
		
		
		const size_t size=  tuple_size<tupletypeforiegntype>::value;
		return OnDeleteHelper<tupletypeforiegntype,size,selftype >::OnDeleteinRefered(deleter,fk,tupleforiegn,this);
		
		
		return true;
	}
	
	
	
	
	
	
	
	bool insert(tableTupleType inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
		
		
		
		
		pair<tablerowiter,bool> a = tableype::insert (make_pair(inrow.primaryKey, inrow));
		
		if(a.second==false)
			return false;
		
		
		if(inrow.foreignKeyvalidate(tupleforiegn) == false)
		{
			tableype::erase(inrow.primaryKey);
			return false;
		}	
		
		return true;
	}
	
	
	
	bool deleteKey(typename tableTupleType::primaryKeyType key)
	{
		//assert(refreningtable);
		
		assert(contains(key));
		
		typedef typename std::set<tableReferedByP * >::iterator referediter;
		
		for(referediter i= tableForiegn<tableTupleType>::referset.begin(); i!= tableForiegn<tableTupleType>::referset.end(); i++)
		{
			if((*i)->OnDeleteinRefered(this,&key) == false)
				return false;
		}
		
		tablerowiter k=findrow(key);
		if(k!=tableype::end())
			tableype::erase (k);

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






struct Score2:tableTuple<int,tuple<int> > {
	
	Score2(int score,int  playerid)
	:tableTuple<int,tuple<int> >(score,make_tuple(playerid))
	{}
};







int main()
{

	table<Player2,tuple<> > b(make_tuple());
	
	
	table<Score2,tuple<tableForiegn<Player2>* > > s(make_tuple(&b));


	
	SHOULD_PASS(b.insert(Player2(1,"shakthi")));
	SHOULD_PASS(b.insert(Player2(4,"shakthi")));
	SHOULD_PASS(b.insert(Player2(6,"shakthi")));
	
	
	SHOULD_PASS(s.insert(Score2(100,1)));
				
	SHOULD_FAIL(s.insert(Score2(200,2)));
	SHOULD_PASS(s.insert(Score2(200,6)));
	
	SHOULD_PASS(b.deleteKey(1));
	SHOULD_PASS(s.insert(Score2(300,4)));
	
	

	return 0;
}