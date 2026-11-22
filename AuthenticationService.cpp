#include "AuthenticationService.h"
#include "DatabaseManager.h"
#include "User.h"

#include <stdexcept>

bool AuthenticationService::authenticate(
    const std::string& email,
    const std::string& password,
    DatabaseManager& db
)
{
    if (password.length() != 8)
        return false;

    try
    {
        User user = db.getUserByEmail(email);

        return user.get_password() == password;
    }
    catch (const std::runtime_error&)
    {
        return false;
    }
}