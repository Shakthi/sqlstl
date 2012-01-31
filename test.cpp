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
			
			typename std::tr1::tuple_element<index-1,tupleforiegntype>::type foriengtable = get<index-1>(intupleforiegn);
			
			
			typedef typename std::tr1::tuple_element<index-1,foriegnrowtype>::type fk1type;
			
			const fk1type  fk1= get<index-1>(fk);
			
			
			if(foriengtable->contains(fk1) == false)
		    	return false;
			
			return checkhelper<tupleforiegntype,index-1,fktype>::foreignKeyvalidatef(intupleforiegn,fk);

			
		}
		
	};
	
	
	
	
	template<typename tupleforiegntype,typename fktype>
	struct checkhelper<tupleforiegntype,0, fktype> {
		static bool foreignKeyvalidatef(tupleforiegntype intupleforiegn,fktype fk)
		{
			return true;
			
		}
		
	};
	
	
	
	template<int index>
	typename tuple_element<index,foriegnrowtype>::type	getForiegKeyForIndex()
	{
		return  get<index>(foriegnkey);

	}
	
	
	
	
	
	template <typename  refrencedtabletype >
	bool foreignKeyvalidate(refrencedtabletype & refrencedtable)
	{
		const size_t tuplesize =tuple_size<refrencedtabletype>::value;
		return checkhelper<refrencedtabletype,tuplesize,foriegnkeytuple>::foreignKeyvalidatef(refrencedtable,foriegnkey);
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






struct Score2:rowf<int,tuple<int> > {
		
	Score2(int score,int  playerid)
	:rowf<int,tuple<int> >(score,make_tuple(playerid))
	{}
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
	
	template<int index>
	struct fkatindex {
		typedef typename tuple_element<index,typename rowtype::foriegnkeytuple>::type type;
		
		static type getfk(tablerowiter i)
		{
			rowtype row = i->second;
			
			
			return get<index>(row.foriegnkey);
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
			
			setTupleReferencehelper<tupleforiegntype,i-1,selftypet>::setTupleReference(intupleforiegn,inself);
		}
		
	};
	
	

		
	
	
	
	
	
	table2plus(tupletypeforiegntype intupleforiegn)
	:tupleforiegn(intupleforiegn)
	{
		const size_t size=  tuple_size<tupletypeforiegntype>::value;
		setTupleReferencehelper<tupletypeforiegntype,size,selftype >::setTupleReference(intupleforiegn,this);
	
	}
	
	tablerowiter findrow(typename rowtype::keytype key)
	{
		
		
//		for(tablerowiter i =tableype::begin();  i !=tableype::end(); i++)
//		{
//			rowtype rowss=i->second;
//			
//			if(key == i->first)
//				return i;
//			
//			
//		}
		
		
		
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
					rowtype rowss=i->second;
					
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
		
		tablerowiter k=findrow(key);
		if(k!=tableype::end())
			tableype::erase (k);

		return true;
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

	table2plus<Player2,tuple<tableForiegn<Player2>* > > b(make_tuple(&b));
	
	
	table2plus<Score2,tuple<tableForiegn<Player2>* > > s(make_tuple(&b));


	//b.setRefer(&b);
	
//table2plus<Player,refer,foriegn > t(make_tuple(tmuu,yy));
	
	
	
//	tableind,tabledepend,tablebase,table
	
//	table<PLYER>
	
//	table<Score,table<Player>> scores(players);

	
	
//#if 0
	
	Player2 P(3,"shakthi");
	
	b.insert(P); 
	
	
	SHOULD_PASS(b.insert(Player2(1,"shakthi")));
	SHOULD_PASS(b.insert(Player2(4,"shakthi")));
	SHOULD_PASS(b.insert(Player2(6,"shakthi")));
	
	
	SHOULD_PASS(s.insert(Score2(100,1)));
				
	SHOULD_FAIL(s.insert(Score2(200,2)));
	SHOULD_PASS(s.insert(Score2(200,6)));
	
	SHOULD_PASS(b.deleteKey(1));
	SHOULD_FAIL(s.insert(Score2(300,4)));
	
	
//	//SHOULD_PASS(players.insert(Player(1,"shakthi")));
//	SHOULD_PASS(players.insert(Player(2,"shakthi")));
//	
//	SHOULD_FAIL(players.insert(Player(2,"shakthi"))) ;
//	
//
//	//SHOULD_PASS(scores.insert(Score(3,1)))
//	SHOULD_PASS(scores.insert(Score(5,2)))
//	
//	SHOULD_PASS(scores.contains(5));
//	
//	players.deleteKey(2);
//	
//	SHOULD_FAIL(scores.contains(5));
	

	return 0;
}