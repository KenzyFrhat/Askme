#pragma once

#include "Question.h"
#include <string>
#include <vector>

using namespace std;
class DatabaseManager;

//TODO: load user data from database after login 

class User
{

// ====================
// Data Members
// ====================
public:
    string username ;
    string email;
private:
    string password;
    int id ;
    vector<int> conversations_ids;
    vector<int> users_sent_qestions_from_ids;
    vector<int> users_sent_qestions_to_ids;


public:
    // ====================
    // Constructors
    // ====================

    // Copy Constructor
    User(const User& other);

    User(
        const string& inp_username,
        const string& inp_email,
        const string& inp_password,
        DatabaseManager& db,
        const int inp_id = -1,  // -1 means it is not from database files 
        bool sign_up = false
    );


    // ====================
    // Destructor
    // ====================

    ~User();

    // ====================
    // COPY ASSIGNMENT OPERATOR
    // ====================

    User& operator=(const User& other);


    // ====================
    // Getters
    // ====================

    string get_username() const;
    string get_email() const;
    string get_password() const;
    int get_id() const;


    // ====================
    // Setters
    // ====================

    void set_username(
        const string& new_username,
        DatabaseManager& db
    );

    void set_email(
        const string& new_email,
        DatabaseManager& db
    );

    void set_password(const string& new_password) ;

    // ====================
    // Chatting
    // ====================

    void send_question(string question_header, int to) const ; //question will be created internally 
    void deleteQuestion(int question_id) const ;
    void showSentQuestions() const ; // will display conversation which is the user who asks (conversation is marked with the other username) 
    void showReceivedQuestions() const; // will display conversation which is the user who answers
    void openConversation(int conversation_id) const; // will use Conversation.showConversationContent

    //helper methods 
    static bool checkAvailableConversationId(int conversation_id); // return true if conversation id is already in user.conversations_ids 

    



};