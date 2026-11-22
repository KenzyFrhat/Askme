#pragma once
#include <String>
#include <vector>
#include <list>
using namespace std;

#include "Question.h"

class Conversation {
private:
	int id;
	int user1_id;
	int user2_id;
	//linkedlist to strore questions 
	list<Question> questions;

public:

// ====================
// Constructors
// ====================

// Copy Constructor
    Conversation(const Conversation& other);

    // parameterized constructor
    Conversation(

    ); 


    // ====================
    // Destructor
    // ====================

    ~Conversation();

    // ====================
    // COPY ASSIGNMENT OPERATOR
    // ====================

    Conversation& operator=(const Conversation& other);




    //getters 



    //setters


    //related methods

	void showConversationContent(); //will use loadConversactionContetn

};