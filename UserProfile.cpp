#include "UserProfile.h"
#include <iostream>
#include "User.h"


void display_profile(const User& user)
{
    std::cout << "\n========== USER PROFILE ==========\n";

    std::cout << "ID: " << user.get_id() << '\n';
    std::cout << "Username: " << user.get_username() << '\n';
    std::cout << "Email: " << user.get_email() << '\n';

    std::cout << "==================================\n";
}