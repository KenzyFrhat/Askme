#pragma once

#include <string>
#include <vector>
#include "User.h"
struct sqlite3;


class DatabaseManager
{
private:
    sqlite3* db;
    std::string lastError;

    // Error Handling
    void setError(const std::string& error);
    void clearError();

public:
    // Constructor & Destructor
    DatabaseManager();
    ~DatabaseManager();

    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Connection Management
    bool initialize();
    bool open(const std::string& databasePath);
    void close();
    bool isOpen() const;
 
    // SQL Execution
    bool execute(const std::string& sql);
    // Error Handling
    std::string getLastError() const;

    // User Operations
    bool addUser(const User& user) ; //done
    bool usernameExists(const std::string& username) ; //done
    bool emailExists(const std::string& email) ; //done
    bool displayAllUsers() ; //done
    User getUserById(int userId)  ; //done
    User getUserByUsername(const std::string& username) ; //done
    User getUserByEmail(const std::string& email) ;  //done
    bool updateUsername(int userId, const std::string& newUsername); //done
    bool updateEmail(int userId, const std::string& newEmail); //done
    bool updatePassword(int userId, const std::string& newPassword);
    bool deleteUser(int userId) ; //done
    


    //Question Operations
    void addQuestion(const Question& question)  const; // will be used internally in user.send_question()
    void deleteQuestion(int question_id) const; // will be used internally in user.delete_Question()
    void editQuestionHeader(int question_id, string new_header) const;
    void answerQuestion(int question_id) const;
    void editQuestionAnswer(int question_id) const;

    //Conversaction Operations
    void addConversaction(int user1_id, int user2_id) const;
    vector<Question> loadConversactionContent(int conversaction_id) const;
    void deleteConeration() const;
    void clearConversaction() const;
    

};