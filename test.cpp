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
#include <iostream>


#define SHOULD_PASS(x) assert((x) );
#define SHOULD_FAIL(x) assert(!(x) );




struct Player:tableTuple<int> {
	int playerid()const
	{
		return primaryKey;
	}
	
	Player(int playerid,string name)
	:tableTuple<int>(playerid),name(name)
	{}
	string name;
};




struct Score:tableTuple<int,tuple<int> > {
	
	Score(int score,int  playerid)
	:tableTuple<int,tuple<int> >(score,make_tuple(playerid))
	{}
};





bool  nameequaltoshakthi(const Player & invalue)
{

	return invalue.name == "shakthi";
}


int main()
{
	
	table<Player> b;
	
	SHOULD_PASS(b.insert(Player(1,"shakthi")));
	SHOULD_PASS(b.insert(Player(4,"ganesh")));
	SHOULD_PASS(b.insert(Player(6,"prasad")));
	
	SHOULD_FAIL(b.insert(Player(6,"prasad")));//primary key constaint
	
	
	
	
	table<Score,tuple<tableForiegn<Player>* > > s(make_tuple(&b));
	
	
	
	
	SHOULD_PASS(s.insert(Score(100,1)));
	
	SHOULD_FAIL(s.insert(Score(200,2)));//cause:foreign key constraint
	SHOULD_PASS(s.insert(Score(200,6)));
	
	SHOULD_PASS(b.deleteKey(6)); //cause:foreign key delete cascade
	SHOULD_FAIL(s.insert(Score(300,6)));//cause: foreign key constraint

	
	
	
	//creates view based on  the player table; iterates rows with name==shakhti
	tableview<table<Player>,bool (*)(const Player& )> view(b,nameequaltoshakthi);
	
	typedef  tableview<table<Player>,bool (*)(const Player &)>::iterator titer;
	 
	
	for (titer i= view.begin(); i!= view.end(); i++) {
		
		std::cout << "Playerid:"<<i->second.playerid() <<" name:"<<i->second.name;

	}
	
	
		

	return 0;
}