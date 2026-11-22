#pragma once
#include <stdexcept>
using namespace std;

//----------------Email Errors-------------------
class email_error : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class invalid_email_format : public email_error {
public:
    using email_error::email_error;
};

class email_already_exists : public email_error {
public:
    using email_error::email_error;
};
//----------------Username Errors-----------------

class username_error : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class invalid_username : public username_error {
public:
    using username_error::username_error;
};

class username_already_exists : public username_error {
public:
    using username_error::username_error;
};

//----------------Password Errors-----------------

class password_error : public runtime_error {
public:
    using runtime_error::runtime_error;

};

class invalid_password : public password_error {
public:
    using password_error::password_error;

};

class password_not_match : public password_error {
public:
    using password_error::password_error;

};

