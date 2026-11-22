#pragma once
#include "DatabaseManager.h"

#include <string>

class VerificationRules
{
public:
    static bool verify_username(
        const std::string& username,
        DatabaseManager& db
    );

    static bool verify_email(
        const std::string& email,
        DatabaseManager& db
    );

    static bool verify_password(
        const std::string& password
    );
};