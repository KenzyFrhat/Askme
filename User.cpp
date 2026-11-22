#include "User.h"
#include "VerificationRules.h"
#include "Errors.h"

#include <iostream>
#include <regex>

// ====================
// Constructors
// ====================

// Parameterized Constructor

User::User(
    const string& inp_username,
    const string& inp_email,
    const string& inp_password,
    DatabaseManager& db,
    const int inp_id,
    bool sign_up
)
{
    id = inp_id;

    if (sign_up)
    {
        // Data from sign_up is already valid
        username = inp_username;
        email = inp_email;
        password = inp_password;
    }
    else
    {
        set_username(inp_username, db);
        set_email(inp_email, db);
        set_password(inp_password);
    }
}


// ====================
// COPY CONSTRUCTOR
// ====================

User::User(const User& other)
    :
    username(other.username),
    email(other.email),
    password(other.password),
    id(other.id)
{}


// ====================
// Destructor
// ====================

User::~User()
{}

// ====================
// COPY ASSIGNMENT OPERATOR
// ====================

User& User::operator=(const User& other)
{
    if (this != &other)
    {
        username = other.username;
        email = other.email;
        password = other.password;
        id = other.id;
    }

    return *this;
}

// ====================
// Getters
// ====================

string User::get_username() const
{
    return this->username;
}

string User::get_email() const
{
    return this->email;
}

string User::get_password() const
{
    return this->password;
}

int User::get_id() const {
    return this->id;
}

// ====================
// Setters
// ====================

void User::set_username(
    const string& new_username,
    DatabaseManager& db)
{
    if (!VerificationRules::verify_username(new_username, db))
    {
        throw invalid_username("Invalid Username");
    }

    username = new_username;
}

void User::set_email(
    const string& new_email,
    DatabaseManager& db)
{
    if (!VerificationRules::verify_email(new_email, db))
    {
        throw invalid_email_format("Invalid Email");
    }

    email = new_email;
}


void User::set_password(const string& new_password)
{
    // TODO: Store hashed password

    if (!VerificationRules::verify_password(new_password))
    {
        throw invalid_password("Password doesn't follow rules");
    }

    password = new_password;
}