#include <iostream>
#include <string>

#include "User.h"
#include "DatabaseManager.h"
#include "VerificationRules.h"

using namespace std;

int passed = 0;
int failed = 0;

// ====================
// TEST HELPER
// ====================

void test(
    const string& testName,
    bool condition,
    const string& errorMessage = ""
)
{
    if (condition)
    {
        cout << "[PASS] " << testName << endl;
        passed++;
    }
    else
    {
        cout << "[FAIL] " << testName << endl;

        if (!errorMessage.empty())
            cout << "       Error: " << errorMessage << endl;

        failed++;
    }
}

// ====================
// MAIN TESTS
// ====================

int main()
{
    DatabaseManager db;

    // ====================
    // 1. CONNECTION TESTS
    // ====================

    cout << "\n===== CONNECTION TESTS =====\n";

    bool openResult = db.open(
        R"(E:\Documents\Basics\Askme_V2_Tests\Askme.db)"
    );

    test(
        "Open database",
        openResult,
        db.getLastError()
    );

    // ====================
    // 3. DISPLAY USERS
    // ====================

    bool displayResult = db.displayAllUsers();

    test(
        "Display all users",
        displayResult,
        db.getLastError()
    );

    // ====================
    // 3. ADD USER TESTS
    // ====================

    cout << "\n===== ADD USER TESTS =====\n";

    const User user1(
        "Mohamed",
        "Mohamed@gmail.com",
        "7amho123"
    );

    test(
        "Add first user",
        db.addUser(user1),
        db.getLastError()
    );

    // ====================
    // 3. DISPLAY USERS
    // ====================
    displayResult = db.displayAllUsers();

    test(
        "Display all users",
        displayResult,
        db.getLastError()
    );

    // ====================
// GET USER BY USERNAME
// ====================

    cout << "\n===== GET USER TESTS =====\n";

    try
    {
        User retrievedUser = db.getUserByUsername("kenzy");


        cout << "\nUser Retrieved Successfully!\n";

        cout << "Username: " << retrievedUser.get_username() << endl;
        cout << "Email: " << retrievedUser.get_email() << endl;
        cout << "Password: " << retrievedUser.get_password() << endl;
        cout << "ID: " << retrievedUser.get_id() << endl;

        test("Get user by username", true);
    }
    catch (const exception& e)
    {
        test("Get user by username", false, e.what());
    }



    // ====================
// DELETE USER TESTS
// ====================

    cout << "\n===== DELETE USER TESTS =====\n";

    bool deleteResult = db.deleteUser(1);

    test(
        "Delete user with ID 1",
        deleteResult,
        db.getLastError()
    );


    // ====================
// GET USER BY ID TESTS
// ====================

    cout << "\n===== GET USER BY ID TESTS =====\n";

    try
    {
        User retrievedUser = db.getUserById(2);

        cout << "ID: " << retrievedUser.get_id() << endl;
        cout << "Username: " << retrievedUser.get_username() << endl;
        cout << "Email: " << retrievedUser.get_email() << endl;

        test("Get user by ID", true);
    }
    catch (const exception& e)
    {
        test("Get user by ID", false, e.what());
    }
}


