
#pragma once
#include <utility>
#include <string>
#include <optional>

class DatabaseManager;
class User;

// ==================== AUTHENTICATION ACTION ====================

enum class authentication_action
{
    Restart,
    Cancel,
    Success
};

// ==================== INPUT ====================

std::string get_input(const std::string& prompt);

bool get_authentication_input(
    const std::string& prompt,
    std::string& input,
    authentication_action& action
);

// ==================== SIGNUP ====================

authentication_action attempt_signup(DatabaseManager& db);
void sign_up(DatabaseManager& db);

// ==================== LOGIN ====================

std::pair<std::string, authentication_action> attempt_login(DatabaseManager& db);
std::optional<User> login(DatabaseManager& db);

// ==================== SIGN OUT ====================

void sign_out();
