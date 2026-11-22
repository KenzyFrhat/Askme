
#include <iostream>
#include <string>

#include "DatabaseManager.h"
#include "User.h"

using namespace std;


// ==============================
// TEST HELPER
// ==============================

bool createTestUser(
    DatabaseManager& db,
    const string& username,
    const string& email,
    int& userId
) {
    User user(
        username,
        email,
        "12345678",
        db,
        -1,
        true
    );

    if (!db.addUser(user))
    {
        cerr << "Failed to create test user: "
            << db.getLastError() << '\n';
        return false;
    }

    userId = db.getUserByEmail(email).get_id();

    return true;
}


// ==============================
// UPDATE USERNAME
// ==============================

bool test_update_username(DatabaseManager& db)
{
    cout << "\n--- UPDATE USERNAME ---\n";

    int userId;

    if (!createTestUser(
        db, "old_username_test",
        "username_test@example.com", userId
    ))
        return false;

    if (!db.updateUsername(userId, "new_username_test"))
    {
        cerr << db.getLastError() << '\n';
        return false;
    }

    User updatedUser = db.getUserById(userId);

    if (updatedUser.get_username() != "new_username_test")
        return false;

    cout << "PASSED\n";
    return true;
}


// ==============================
// UPDATE EMAIL
// ==============================

bool test_update_email(DatabaseManager& db)
{
    cout << "\n--- UPDATE EMAIL ---\n";

    int userId;

    if (!createTestUser(
        db, "email_test_user",
        "old_email_test@example.com", userId
    ))
        return false;

    if (!db.updateEmail(userId, "new_email_test@example.com"))
    {
        cerr << db.getLastError() << '\n';
        return false;
    }

    User updatedUser = db.getUserById(userId);

    if (updatedUser.get_email() != "new_email_test@example.com")
        return false;

    cout << "PASSED\n";
    return true;
}


// ==============================
// UPDATE PASSWORD
// ==============================

bool test_update_password(DatabaseManager& db)
{
    cout << "\n--- UPDATE PASSWORD ---\n";

    int userId;

    if (!createTestUser(
        db, "password_test_user",
        "password_test@example.com", userId
    ))
        return false;

    if (!db.updatePassword(userId, "87654321"))
    {
        cerr << db.getLastError() << '\n';
        return false;
    }

    User updatedUser = db.getUserById(userId);

    if (updatedUser.get_password() != "87654321")
        return false;

    cout << "PASSED\n";
    return true;
}


// ==============================
// DELETE USER
// ==============================

bool test_delete_user(DatabaseManager& db)
{
    cout << "\n--- DELETE USER ---\n";

    int userId;

    if (!createTestUser(
        db, "delete_test_user",
        "delete_test@example.com", userId
    ))
        return false;

    if (!db.deleteUser(userId))
    {
        cerr << db.getLastError() << '\n';
        return false;
    }

    if (db.emailExists("delete_test@example.com"))
        return false;

    cout << "PASSED\n";
    return true;
}


// ==============================
// MAIN
// ==============================

//int main()
//{
//    DatabaseManager db;
//
//    if (!db.initialize())
//    {
//        cerr << "Database initialization failed: "
//            << db.getLastError() << '\n';
//        return 1;
//    }
//
//    if (!db.execute("BEGIN TRANSACTION;"))
//    {
//        cerr << "Failed to start transaction: "
//            << db.getLastError() << '\n';
//        return 1;
//    }
//
//    int passed = 0;
//    const int total = 4;
//
//    try
//    {
//        passed += test_update_username(db);
//        passed += test_update_email(db);
//        passed += test_update_password(db);
//        passed += test_delete_user(db);
//    }
//    catch (const exception& e)
//    {
//        cerr << "Unexpected error: " << e.what() << '\n';
//    }
//
//    if (!db.execute("ROLLBACK;"))
//    {
//        cerr << "Rollback failed: " << db.getLastError() << '\n';
//        db.close();
//        return 1;
//    }
//
//    cout << "\n============================\n";
//    cout << "Passed: " << passed << "/" << total << '\n';
//    cout << "============================\n";
//
//    db.close();
//
//    return passed == total ? 0 : 1;
//}
