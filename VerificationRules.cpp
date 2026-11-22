#include "VerificationRules.h"
#include <regex>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

// ====================
// USERNAME VERIFICATION
// ====================

bool VerificationRules::verify_username(
    const std::string& username,
    DatabaseManager& db)
{
    // Not empty, without spaces, and unique

    return !username.empty() &&
        std::none_of(username.begin(), username.end(),
            [](unsigned char character)
            {
                return std::isspace(character);
            }) &&
        !db.usernameExists(username);
}

// ====================
// EMAIL VERIFICATION
// ====================

bool VerificationRules::verify_email(
    const std::string& email,
    DatabaseManager& db)
{
    static const std::regex pattern(
        R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)"
    );

    return std::regex_match(email, pattern) &&
        !db.emailExists(email);
}


// ====================
// PASSWORD VERIFICATION
// ====================

bool VerificationRules::verify_password(const std::string& password)
{
    // length = 8 
    return password.length() == 8;
}