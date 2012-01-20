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
using namespace std;

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
	bool foreignKeyvalidate(refrencedtabletype refrencedtable)
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




struct tableDeleter
{
	
	
	
};



struct tableDeletable
{
	

	virtual bool OnDeleteinRefered(tableDeleter * deleter,void * fk)
	{
		return true;
		
	}
	



};


template <typename rowtype2,typename refrencedtabletype>
struct table2:map<typename rowtype2::keytype,rowtype2> ,tableDeletable
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
		
	virtual bool OnDeleteinRefered(tableDeleter * deleter,void * fk)
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
struct table3:map<typename rowtype2::keytype,rowtype2>,tableDeleter
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



template <typename rowtype2,typename refreningtabletype,typename refrencedtabletype>
struct table4:map<typename rowtype2::keytype,rowtype2>,tableDeleter
{
	
	typedef map<typename rowtype2::keytype,rowtype2> tableype ;
	
	typedef typename rowtype2::keytype  pkeytype ;
	
	refreningtabletype * refreningtable;
	refrencedtabletype & refrencedtable;
	
	table4(refrencedtabletype & inrefrencedtable)
	:refrencedtable(inrefrencedtable)
	{}
	
	
	void SetupReferencing(refreningtabletype * inrefreningtabletype)
	{
		refreningtable = inrefreningtabletype;
	}
	
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
	
	virtual bool OnDeleteinRefered(tableDeleter * deleter,void * fk)
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


#define SHOULD_PASS(x) assert((x) == SHOULD_WORK);
#define SHOULD_FAIL(x) assert((x) == SHOULDNOT_WORK);





int main()
{
	
	
	
	
	table3<Player,tableDeletable> players;
	
	table2<Score,table3<Player,tableDeletable> > scores(players);
	
players.SetupReferencing(&scores);
//	tablebase. ;//referenced  by other
//	tabledepedent//refrences other

//	tablemix. ;
//	tabledepedent
	
	
	
	
//	table<Score,table<Player>> scores(players);
	
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