#pragma once
#include <string>
using namespace std; 

class Question {
    // ====================
    // Data Members
    // ====================
public:
    string header;
    string answer;
    int from;
    int to;
    string datetime;
private:
    int id;
	int parent_ID;
	int child_ID;
	int conversaction_id;

public:

    // ====================
    // Constructors
    // ====================

    // Copy Constructor
    Question(const Question& other);

    // parameterized constructor
    Question(

    );


    // ====================
    // Destructor
    // ====================

    ~Question();

    // ====================
    // COPY ASSIGNMENT OPERATOR
    // ====================

    Question& operator=(const Question& other);




	//getters 



	//setters
};