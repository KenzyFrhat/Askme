
#include <iostream>
#include <string>
#include <stdexcept>
#include "SignupLoginController.h"
#include "VerificationRules.h"
#include "User.h"
#include "DatabaseManager.h"
#include "AuthenticationService.h"

// ==================== INPUT ====================

std::string get_input(const std::string& prompt)
{
    std::string input;

    std::cout << prompt;
    std::getline(std::cin, input);

    return input;
}

// ==================== AUTHENTICATION INPUT ====================

bool get_authentication_input(
    const std::string& prompt,
    std::string& input,
    authentication_action& action)
{
    std::cout << "\n1 - Restart\n"
        << "2 - Cancel\n";

    input = get_input(prompt);

    if (input == "1")
    {
        action = authentication_action::Restart;
        return false;
    }

    if (input == "2")
    {
        action = authentication_action::Cancel;
        return false;
    }

    return true;
}

// ==================== SIGNUP ====================

authentication_action attempt_signup(DatabaseManager& db)
{
    std::string username;
    std::string email;
    std::string password;

    authentication_action action;

    // ---------- USERNAME ----------

    while (true)
    {
        if (!get_authentication_input(
            "Enter username: ", username, action))
            return action;

        try
        {
            if (VerificationRules::verify_username(username, db))
                break;

            std::cout << "Invalid username or username already exists.\n";
        }
        catch (const std::runtime_error& e)
        {
            std::cout << "Database error: " << e.what() << '\n';
            return authentication_action::Cancel;
        }
    }

    // ---------- EMAIL ----------

    while (true)
    {
        if (!get_authentication_input(
            "Enter email: ", email, action))
            return action;

        try
        {
            if (VerificationRules::verify_email(email, db))
                break;

            std::cout << "Invalid email format or email already exists.\n";
        }
        catch (const std::runtime_error& e)
        {
            std::cout << "Database error: " << e.what() << '\n';
            return authentication_action::Cancel;
        }
    }

    // ---------- PASSWORD ----------

    while (true)
    {
        if (!get_authentication_input(
            "Enter password: ", password, action))
            return action;

        if (VerificationRules::verify_password(password))
            break;

        std::cout << "Invalid password. Password must contain exactly 8 characters.\n";
    }

    // ---------- REGISTRATION ----------

    try
    {
        constexpr bool is_signup = true;

        const User user(
            username,
            email,
            password,
            db,
            -1,
            is_signup
        );

        if (!db.addUser(user))
        {
            std::cout << "Failed to create account: "
                << db.getLastError() << '\n';

            return authentication_action::Cancel;
        }
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Registration error: " << e.what() << '\n';
        return authentication_action::Cancel;
    }

    return authentication_action::Success;
}

// ==================== SIGNUP CONTROL FLOW ====================

void sign_up(DatabaseManager& db)
{
    while (true)
    {
        authentication_action action = attempt_signup(db);

        if (action == authentication_action::Restart)
            continue;

        if (action == authentication_action::Cancel)
        {
            std::cout << "Signup canceled.\n";
            return;
        }

        if (action == authentication_action::Success)
        {
            std::cout << "Signed up successfully!\n";
            return;
        }
    }
}

// ==================== LOGIN ====================

pair<string, authentication_action>  attempt_login(DatabaseManager& db) //return userEmail and result flag
{
    std::string email;
    std::string password;

    authentication_action action;

    // ---------- EMAIL ----------

    if (!get_authentication_input(
        "Enter email: ", email, action))
        return { email, action };

    // ---------- PASSWORD ----------

    while (true)
    {
        if (!get_authentication_input(
            "Enter password: ", password, action))
            return { email, action };

        if (AuthenticationService::authenticate(email, password, db))
            break;

           cout << "Invalid email or password.\n";
    }

    return { email, authentication_action::Success };
}

// ==================== LOGIN CONTROL FLOW ====================


std::optional<User> login(DatabaseManager& db)
{
    while (true)
    {
        auto result = attempt_login(db);
        authentication_action action = result.second;

        if (action == authentication_action::Restart)
        {
           continue;
        }
           

        if (action == authentication_action::Cancel)
        {
            std::cout << "Login canceled.\n";
            return std::nullopt;
           
        }

        if (action == authentication_action::Success)
        {
            std::cout << "Logged in successfully!\n";
            return db.getUserByEmail(result.first);

        }

        
        
    }
}

// ==================== SIGN OUT ====================

void sign_out()
{
    // Session termination will be implemented later.
}
