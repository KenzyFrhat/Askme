#pragma once

#include <string>

class DatabaseManager;

class AuthenticationService
{
public:
    static bool authenticate(
        const std::string& email,
        const std::string& password,
        DatabaseManager& db
    );
};