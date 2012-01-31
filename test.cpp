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

template <typename _keytype>
struct row {
	
	
	typedef _keytype keytype;
	
	keytype primarykey;
	bool operator < (const row & u)const
	{
		return primarykey < u.primarykey;
	}
	
	
	row(keytype k)
	:primarykey(k)
	{
	
	}
	
	
	bool validate()
	{
		return true;
	}
	
	
	
	
	bool foreignKeyvalidate()
	{
		return true;
	}
	
	
};






template <typename _keytype,typename _foriegnrowtype>
struct row2 {
	
	
	typedef _keytype keytype;
	typedef typename _foriegnrowtype::keytype foriegnkeytype;
	
	keytype primarykey;
	foriegnkeytype foriegnkey;
	bool operator < (const row2 & u)const
	{
		return primarykey < u.primarykey;
	}
	
	
	row2(keytype k,foriegnkeytype fk)
	:primarykey(k),foriegnkey(fk)
	{
		
	}
	
	
	bool validate()
	{
		return true;
	}
	
	

	
	template <typename  refrencedtabletype>
	bool foreignKeyvalidate(refrencedtabletype & refrencedtable)
	{
		return (refrencedtable.contains(foriegnkey));
	}
	
	
	
	
};






struct Player:row<int> {
	int playerid()
	{
		return primarykey;
	}
	
	Player(int playerid,string name)
	:row<int>(playerid),name(name)
	{}
	string name;
};







struct Score:row2<int,Player> {
	int score()
	{
		return primarykey;
	}
	
	bool validate()
	{
		return score()>0; 
	}
	
	Score(int score,int plyerid)
	:row2<int,Player>(score,plyerid)
	{
	
	}
	
	
};	


template <typename rowtype>
struct table:map<typename rowtype::keytype,rowtype> 
{
	
	typedef map<typename rowtype::keytype,rowtype> tableype ;

	typedef typename tableype::iterator tablerowiter ;
	bool insert(rowtype inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
		pair<tablerowiter,bool  > a = tableype::insert (make_pair(inrow.primarykey, inrow));
		return a.second;
	}
	
	
	bool update(rowtype inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
		tablerowiter iter =find(inrow.primarykey);
		
		if(iter == tableype::end())
			return false;
			
		*iter = inrow;
	}
	
	
	bool insertorupdate(rowtype inrow)
	{
		if(update(inrow))
			return true;
		 
		if(insert(inrow))
			return true;
		
		return false;
	}
	
		
	tablerowiter findrow(typename rowtype::keytype key)
	{
		return tableype::find(key);
	}

	
	
	bool contains(typename rowtype::keytype key)
	{
		return findrow(key) != tableype::end();
	}
	
	
	
	//
//
//	
//	bool onDelete(rowiter iter)
//	{
//		return false;
//	}
//	
//	
//	bool onUpdate(rowiter iter)
//	{
//		return false;
//	}
//	
	
};


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
	typedef typename row::keytype pkeytype;	
	std::set<tableReferedByP * > referset;
	
	virtual bool contains(pkeytype fk)=0;
};



template <class row>
struct tableReferedBy
{
	
	typedef row referingKey;
	virtual bool OnDeleteinRefered(void * deleter,void * fk)=0;
//	{
//		return true;
//		
//	}
	



};




template <typename _keytype,typename foriegnrowtype>
struct rowf {
	
	
	typedef _keytype keytype;
	typedef foriegnrowtype  foriegnkeytuple; 
	
	keytype primarykey;
	foriegnkeytuple foriegnkey;
	
	bool operator < (const rowf & u)const
	{
		return primarykey < u.primarykey;
	}
	
	
	rowf(keytype k,const foriegnkeytuple & fk)
	:primarykey(k),foriegnkey(fk)
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
			
			typename std::tr1::tuple_element<index,tupleforiegntype>::type foriengtable = get<index>(intupleforiegn);
			
			
			typedef typename std::tr1::tuple_element<index,foriegnrowtype>::type fk1type;
			
			const fk1type  fk1= get<index>(fk);
			
			
			return foriengtable->contains(fk1);
			
		}
		
	};
	
	
	
	template<int index>
	typename tuple_element<index,foriegnrowtype>::type	getForiegKeyForIndex()
	{
		return  get<index>(foriegnkey);

	}
	
	
	template<typename tupleforiegntype,typename fktype>
	struct checkhelper<tupleforiegntype,0,fktype> {
		static bool foreignKeyvalidatef(tupleforiegntype intupleforiegn,fktype fk)
		{
			
			typename std::tr1::tuple_element<0,tupleforiegntype>::type foriengtable = get<0>(intupleforiegn);
			
			
			typedef typename std::tr1::tuple_element<0,foriegnrowtype>::type fk1type;
			
			const fk1type  fk1= get<0>(fk);
			
			
			return foriengtable->contains(fk1);
			
		}
		
	};
	
	
	
	
	template <typename  refrencedtabletype >
	bool foreignKeyvalidate(refrencedtabletype & refrencedtable)
	{
		const size_t tuplesize =tuple_size<refrencedtabletype>::value;
		return checkhelper<refrencedtabletype,tuplesize-1,foriegnkeytuple>::foreignKeyvalidatef(refrencedtable,foriegnkey);
	}
	
	
	
	
};



typedef tuple<int> rowfi;

struct Player2:rowf<int,rowfi > {
	int playerid()
	{
		return primarykey;
	}
	
	Player2(int playerid,string name)
	:rowf<int,rowfi>(playerid,make_tuple(playerid)),name(name)
	{}
	string name;
};




template <typename rowtype,typename tupletypeforiegntype>
struct table2plus:map<typename rowtype::keytype,rowtype> ,tableReferedByP,tableForiegn<rowtype>
{
	
	tupletypeforiegntype tupleforiegn;
	typedef table2plus<rowtype,tupletypeforiegntype> selftype;
	typedef typename map<typename rowtype::keytype,rowtype>::iterator tablerowiter;
	typedef map<typename rowtype::keytype,rowtype> tableype;
	
	template<typename tupleforiegntype,size_t i,typename selftypet>
	struct setTupleReferencehelper;
	
	template<typename tupleforiegntype,typename selftypet>
	struct setTupleReferencehelper<tupleforiegntype,0,selftypet> {
		static void setTupleReference(tupleforiegntype intupleforiegn,selftypet * inself)
		{
			
			typename std::tr1::tuple_element<0,tupleforiegntype>::type foriengtable = get<0>(inself->tupleforiegn);
			
			foriengtable->referset.insert(inself);
			
		}
		
	};
	
	
	
	template<typename tupleforiegntype,size_t i,typename selftypet>
	struct setTupleReferencehelper {
		static void setTupleReference(tupleforiegntype intupleforiegn,selftypet * inself)
		{
			
			typename std::tr1::tuple_element<i,tupleforiegntype>::type foriengtable = get<i>(inself->tupleforiegn);
			
			foriengtable->referset.insert(inself);
			
			setTupleReferencehelper<tupleforiegntype,i-1,selftypet>::setTupleReference(intupleforiegn,inself);
		}
		
	};
	
	

		
	
	
	
	
	
	table2plus(tupletypeforiegntype intupleforiegn)
	:tupleforiegn(intupleforiegn)
	{
		const size_t size=  tuple_size<tupletypeforiegntype>::value;
		if(size>0)
		setTupleReferencehelper<tupletypeforiegntype,size-1,selftype >::setTupleReference(intupleforiegn,this);
	
	}
	
	tablerowiter findrow(typename rowtype::keytype key)
	{
		return tableype::find(key);
	}
	
	
	
	bool contains(typename rowtype::keytype key)
	{
		return findrow(key) != tableype::end();
	}
	
	
	
	
	
	template<class T>
	void OnDelete(typename T::pkeytype fk,const T &treftable,tablerowiter i)
	{
		tableype::erase (i);
	} 
	
	
//	template<int index>
//	typename tuple_element<index,foriegnrowtype>::type	getForiegKeyForIndex()


	template<int index>
	struct fkatindex {
		typedef typename tuple_element<index,typename rowtype::foriegnkeytuple>::type type;
		
		static type getfk(tablerowiter i)
		{
			rowtype row = i->second;
			
			
			return get<index>(row.foriegnkey);
		}
	};
	
	
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
			OnDeleteHelper<tupletypeforiegntype,index-1,selftype >::OnDeleteinRefered(deleter,fk,ft,_self);

			
		}
		
	};
	
	
	
	template<typename tupleforiegntype,typename _selftype>
	struct OnDeleteHelper<tupleforiegntype,0,_selftype> {
		
		static bool  OnDeleteinRefered(tableForiegnBase * deleter,void * fk,tupleforiegntype  ft,_selftype * _self)
		{
			
			typedef typename tuple_element<0,tupleforiegntype>::type tt;
			
			tableForiegnBase*  table = static_cast<tableForiegnBase*>(get<0>(ft));
			
			//printf("sucess %x %x\n",table,deleter);
			
			
			if(table == deleter)
			{
				
				for(tablerowiter i =_self->tableype::begin();  i !=_self->tableype::end(); i++)
				{
					rowtype rowss=i->second;
					
					//printf("sucess\n");
					
					typedef typename fkatindex<0>::type t;
					
					t fk0= fkatindex<0>::getfk(i);
					
					t converted = *(static_cast<t*>(fk)); 
					
					if(converted==fk0)
					{
					
						_self->OnDelete(converted,get<0>(ft),i);
					}
		//			typedef typename tuple_element<index,rowtype>::type type;

					
					int i;
					i++;
					
				}	
			
					

			
			}
			
			
		}
		
	};
	
	
	
	virtual bool OnDeleteinRefered(tableForiegnBase * deleter,void * fk)
	{
		
//		for(tablerowiter i =tableype::begin(); i !=tableype::end(); i++)
//		{
//			rowtype2 rowss=i->second;
//			
//			if((void*)deleter == &refrencedtable)
//			{
//				typedef typename refrencedtabletype::pkeytype fkt;
//				
//				if(rowss.foriegnkey == *((fkt*)fk))
//				{
//					OnDelete(rowss.foriegnkey,refrencedtable,i);
//					//tableype::erase (i);
//					
//				}	
//			}
//		}
		
		
		
		const size_t size=  tuple_size<tupletypeforiegntype>::value;
		if(size>0)
			OnDeleteHelper<tupletypeforiegntype,size-1,selftype >::OnDeleteinRefered(deleter,fk,tupleforiegn,this);
		
		
		return true;
	}
	
	
	
	
	
	
	
	bool insert(rowtype inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
		
		
		
		
		pair<tablerowiter,bool> a = tableype::insert (make_pair(inrow.primarykey, inrow));
		
		if(a.second==false)
			return false;
		
		
		if(inrow.foreignKeyvalidate(tupleforiegn) == false)
		{
			tableype::erase(inrow.primarykey);
			return false;
		}	
		
		return true;
	}
	
	
	
	bool deleteKey(typename rowtype::keytype key)
	{
		//assert(refreningtable);
		
		assert(contains(key));
		
		typedef typename std::set<tableReferedByP * >::iterator referediter;
		
		for(referediter i= tableForiegn<rowtype>::referset.begin(); i!= tableForiegn<rowtype>::referset.end(); i++)
		{
			if((*i)->OnDeleteinRefered(this,&key) == false)
				return false;
		}
		
		tableype::erase (findrow(key));

		
	}

	
};

template <typename rowtype2,typename refrencedtabletype>
struct table2:map<typename rowtype2::keytype,rowtype2> ,tableReferedBy<rowtype2>
{
	
	typedef map<typename rowtype2::keytype,rowtype2> tableype ;
	
	 refrencedtabletype & refrencedtable;
	
	
	table2(refrencedtabletype & inrefrencedtable)
	:refrencedtable(inrefrencedtable)
	{}
	
	typedef typename tableype::iterator tablerowiter ;
	bool insert(rowtype2 inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
		if(inrow.foreignKeyvalidate(refrencedtable) == false)
			return false;
		
		pair<tablerowiter,bool> a = tableype::insert (make_pair(inrow.primarykey, inrow));
		return a.second;
	}
	

	bool update(rowtype2 inrow)
	{
		
 		if(inrow.validate() == false)
			return false;
		
		tablerowiter iter =find(inrow.primarykey);
		
		if(iter == tableype::end())
			return false;
		
		*iter = inrow;
	}
	
	
	bool insertorupdate(rowtype2 inrow)
	{
		if(update(inrow))
			return true;
		
		if(insert(inrow))
			return true;
		
		return false;
	}
	
	
	tablerowiter findrow(typename rowtype2::keytype key)
	{
		return tableype::find(key);
	}
	
	
	
	bool contains(typename rowtype2::keytype key)
	{
		return findrow(key) != tableype::end();
	}
	
	
	template<class T>
	void OnDelete(typename T::pkeytype fk,const T &treftable,tablerowiter i)
	{
		 tableype::erase (i);
	} 
		
	virtual bool OnDeleteinRefered(void * deleter,void * fk)
	{
		
		for(tablerowiter i =tableype::begin(); i !=tableype::end(); i++)//XXX very slow
		{
			rowtype2 rowss=i->second;
			
			if((void*)deleter == &refrencedtable)
			{
				typedef typename refrencedtabletype::pkeytype fkt;
				
				if(rowss.foriegnkey == *((fkt*)fk))
				{
					OnDelete(rowss.foriegnkey,refrencedtable,i);
					//tableype::erase (i);
					
				}	
			}
		}
					
		return true;
	}
	
	
	
};



template <typename rowtype2,typename refreningtabletype>
struct table3:map<typename rowtype2::keytype,rowtype2>,tableForiegn< rowtype2>
{
	
	typedef map<typename rowtype2::keytype,rowtype2> tableype ;
	
	typedef typename rowtype2::keytype  pkeytype ;
	
	refreningtabletype * refreningtable;
	
	void SetupReferencing(refreningtabletype * inrefreningtabletype)
	{
		refreningtable = inrefreningtabletype;
	}
	
	typedef typename tableype::iterator tablerowiter ;
	bool insert(rowtype2 inrow)
	{
		
		if(inrow.validate() == false)
			return false;
		
//		if(inrow.foreignKeyvalidate(refrencedtable) == false)
//			return false;
		
		pair<tablerowiter,bool> a = tableype::insert (make_pair(inrow.primarykey, inrow));
		return a.second;
	}
	
	
	bool update(rowtype2 inrow)
	{
		
 		if(inrow.validate() == false)
			return false;
		
		tablerowiter iter =find(inrow.primarykey);
		
		if(iter == tableype::end())
			return false;
		
		*iter = inrow;
	}
	
	
	bool insertorupdate(rowtype2 inrow)
	{
		if(update(inrow))
			return true;
		
		if(insert(inrow))
			return true;
		
		return false;
	}
	
	
	tablerowiter findrow(typename rowtype2::keytype key)
	{
		return tableype::find(key);
	}
	
	
	
	bool contains(typename rowtype2::keytype key)
	{
		return findrow(key) != tableype::end();
	}
	
	
	
	
	
	
	bool deleteKey(typename rowtype2::keytype key)
	{
		assert(refreningtable);
		
		assert(contains(key));
		tableype::erase (findrow(key));

		
		if(refreningtable->OnDeleteinRefered(this,&key))
		{
			return true;
		}
		
			return false;
	}
	
	
};







#define SHOULD_WORK true
#define SHOULDNOT_WORK false


#define SHOULD_PASS(x) assert((x) );
#define SHOULD_FAIL(x) assert(!(x) );




int main()
{
//	sss s(s);
//	//tuple<Player> a;
	
	
	
	table3<Player,tableReferedBy<Score> > players;
	table2<Score,tableForiegn<Player > > scores(players);
	
//	table3<Player,tablereferedby<Score> > players;
//		table3<Score,tableReferes<Player> > players;
    players.SetupReferencing(&scores);
//	tablebase. ;//referenced  by other
//	tabledepedent//refrences other

//	tablemix. ;
//	tabledepedent
	
	
	//typedef tuple<tableReferedBy<Player> *> refer;
	
	
	
	table2plus<Player2,tuple<tableForiegn<Player2>* > > b(make_tuple(&b));

	//b.setRefer(&b);
	
//table2plus<Player,refer,foriegn > t(make_tuple(tmuu,yy));
	
	
	
//	tableind,tabledepend,tablebase,table
	
//	table<PLYER>
	
//	table<Score,table<Player>> scores(players);

	
	
//#if 0
	
	
	SHOULD_PASS(b.insert(Player2(1,"shakthi")));
	SHOULD_PASS(b.insert(Player2(4,"shakthi")));
	SHOULD_PASS(b.insert(Player2(6,"shakthi")));
	
	
	SHOULD_PASS(b.deleteKey(1));
	
	SHOULD_PASS(players.insert(Player(1,"shakthi")));
	SHOULD_PASS(players.insert(Player(2,"shakthi")));
	
	SHOULD_FAIL(players.insert(Player(2,"shakthi"))) ;
	

	SHOULD_PASS(scores.insert(Score(3,1)))
	SHOULD_PASS(scores.insert(Score(5,2)))
	
	SHOULD_PASS(scores.contains(5));
	
	players.deleteKey(2);
	
	SHOULD_FAIL(scores.contains(5));
	

	return 0;
}