#include "DatabaseManager.h"
#include "sqlite3.h"
#include <cstdlib>
#include <iostream>

// ====================
// CONSTRUCTOR & DESTRUCTOR
// ====================

DatabaseManager::DatabaseManager()
    : db(nullptr), lastError("")
{}

DatabaseManager::~DatabaseManager()
{
    close();
}

// ====================
// ERROR HANDLING
// ====================

void DatabaseManager::setError(const std::string& error) 
{
    lastError = error;
}

void DatabaseManager::clearError()
{
    lastError.clear();
}

std::string DatabaseManager::getLastError() const
{
    return lastError;
}

// ====================
// CONNECTION MANAGEMENT
// ====================

bool DatabaseManager::initialize()
{
    const char* dbPath = std::getenv("ASKME_DB_PATH");

    if (dbPath == nullptr || *dbPath == '\0')
    {
        setError("ASKME_DB_PATH environment variable is not configured.");
        return false;
    }

    return open(dbPath);
}

bool DatabaseManager::open(const std::string& databasePath)
{
    if (isOpen())
    {
        clearError();
        return true;
    }

    int result = sqlite3_open(databasePath.c_str(), &db);

    if (result != SQLITE_OK)
    {
        setError(
            db ? sqlite3_errmsg(db)
            : "Failed to allocate database connection."
        );

        if (db != nullptr)
        {
            sqlite3_close_v2(db);
            db = nullptr;
        }

        return false;
    }

    // Wait up to 5 seconds if database is locked
    result = sqlite3_busy_timeout(db, 5000);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        close();
        return false;
    }

    // Enable foreign key constraints
    if (!execute("PRAGMA foreign_keys = ON;"))
    {
        close();
        return false;
    }

    clearError();

    return true;
}

void DatabaseManager::close()
{
    if (!isOpen())
    {
        clearError();
        return;
    }

    int result = sqlite3_close_v2(db);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return;
    }

    db = nullptr;

    clearError();
}

bool DatabaseManager::isOpen() const
{
    return db != nullptr;
}

// ====================
// SQL EXECUTION
// ====================

bool DatabaseManager::execute(const std::string& sql)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        sql.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK)
    {
        setError(
            errorMessage ? errorMessage : sqlite3_errmsg(db)
        );

        sqlite3_free(errorMessage);

        return false;
    }

    clearError();

    return true;
}

// =================================================================
//              =======USER OPERATIONS========
// ==================================================================


// ====================
// UsernameExists
// ====================
bool DatabaseManager::usernameExists(const std::string& username)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        SELECT EXISTS(
            SELECT 1 FROM Users WHERE username = ?
        );
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_bind_text(
        stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_step(stmt);

    if (result != SQLITE_ROW)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    bool exists = sqlite3_column_int(stmt, 0) == 1;

    sqlite3_finalize(stmt);

    clearError();

    return exists;
}


// ====================
//  emailExists
// ====================
bool DatabaseManager::emailExists(const std::string& email)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        SELECT EXISTS(
            SELECT 1 FROM Users WHERE email = ?
        );
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_bind_text(
        stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_step(stmt);

    if (result != SQLITE_ROW)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    bool exists = sqlite3_column_int(stmt, 0) == 1;

    sqlite3_finalize(stmt);

    clearError();

    return exists;
}

// ====================
// ADD USER
// ====================

bool DatabaseManager::addUser(const User& user)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        INSERT INTO Users (username, email, password)
        VALUES (?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // Bind username
    result = sqlite3_bind_text(
        stmt, 1, user.get_username().c_str(), -1, SQLITE_TRANSIENT
    );

    // Bind email
    if (result == SQLITE_OK)
    {
        result = sqlite3_bind_text(
            stmt, 2, user.get_email().c_str(), -1, SQLITE_TRANSIENT
        );
    }

    // Bind password
    if (result == SQLITE_OK)
    {
        result = sqlite3_bind_text(
            stmt, 3, user.get_password().c_str(), -1, SQLITE_TRANSIENT
        );
    }

    // Execute statement
    if (result == SQLITE_OK)
    {
        result = sqlite3_step(stmt);
    }

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    clearError();

    return true;
}

// ====================
// DISPLAY ALL USERS
// ====================

bool DatabaseManager::displayAllUsers()  
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = "SELECT * FROM Users;";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    int columnCount = sqlite3_column_count(stmt);

    // Print column names
    cout << "\n========== USERS TABLE ==========\n";

    for (int i = 0; i < columnCount; i++)
    {
        cout << sqlite3_column_name(stmt, i) << "\t";
    }

    cout << "\n---------------------------------\n";

    // Print rows
    bool hasRows = false;

    while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        hasRows = true;

        for (int i = 0; i < columnCount; i++)
        {
            const unsigned char* value = sqlite3_column_text(stmt, i);

            cout << (value ? reinterpret_cast<const char*>(value) : "NULL")
                << "\t";
        }

        cout << endl;
    }

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);

    if (!hasRows)
        cout << "No users found.\n";

    cout << "=================================\n";

    clearError();

    return true;
}
// ====================
// GET USER BY USERNAME
// ====================

User DatabaseManager::getUserByUsername(const std::string& username) 
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        SELECT id, username, email, password
        FROM Users
        WHERE username = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        throw std::runtime_error(lastError);
    }

    // Bind username
    result = sqlite3_bind_text(
        stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Execute query
    result = sqlite3_step(stmt);

    if (result != SQLITE_ROW)
    {
        if (result == SQLITE_DONE)
            setError("User not found: " + username);
        else
            setError(sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Retrieve user data
    int id = sqlite3_column_int(stmt, 0);

    std::string retrievedUsername =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    std::string retrievedEmail =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

    std::string retrievedPassword =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

    // Finalize statement
    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        throw std::runtime_error(lastError);
    }

    clearError();

    // Return User object with database ID
    return User(
        retrievedUsername,
        retrievedEmail,
        retrievedPassword,
        *this,
        id,
        true
    );
}

// ====================
// GET USER BY ID
// ====================

User DatabaseManager::getUserById(int userId)  
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        SELECT id, username, email, password
        FROM Users
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    // Prepare statement
    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        throw std::runtime_error(lastError);
    }

    // Bind user ID
    result = sqlite3_bind_int(stmt, 1, userId);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Execute query
    result = sqlite3_step(stmt);

    if (result != SQLITE_ROW)
    {
        if (result == SQLITE_DONE)
            setError("User not found with ID: " + std::to_string(userId));
        else
            setError(sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Retrieve user data
    int id = sqlite3_column_int(stmt, 0);

    std::string retrievedUsername =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    std::string retrievedEmail =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

    std::string retrievedPassword =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

    // Finalize statement
    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        throw std::runtime_error(lastError);
    }

    clearError();

    // Return User object
    return User(
        retrievedUsername,
        retrievedEmail,
        retrievedPassword,
        *this,
        id,
        true
    );
}



// ====================
// GET USER BY EMAIL
// ====================

User DatabaseManager::getUserByEmail(const std::string& email)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        SELECT id, username, email, password
        FROM Users
        WHERE email = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    // Prepare statement
    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Bind email
    result = sqlite3_bind_text(
        stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Execute query
    result = sqlite3_step(stmt);

    if (result != SQLITE_ROW)
    {
        if (result == SQLITE_DONE)
            setError("User not found with email: " + email);
        else
            setError(sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        throw std::runtime_error(lastError);
    }

    // Retrieve user data
    int id = sqlite3_column_int(stmt, 0);

    const unsigned char* usernameValue =
        sqlite3_column_text(stmt, 1);

    const unsigned char* emailValue =
        sqlite3_column_text(stmt, 2);

    const unsigned char* passwordValue =
        sqlite3_column_text(stmt, 3);

    std::string retrievedUsername =
        usernameValue
        ? reinterpret_cast<const char*>(usernameValue)
        : "";

    std::string retrievedEmail =
        emailValue
        ? reinterpret_cast<const char*>(emailValue)
        : "";

    std::string retrievedPassword =
        passwordValue
        ? reinterpret_cast<const char*>(passwordValue)
        : "";

    // Finalize statement
    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        throw std::runtime_error(lastError);
    }

    clearError();

    // Return User object with database ID
    return User(
        retrievedUsername,
        retrievedEmail,
        retrievedPassword,
        *this,
        id,
        true
    );
}


// ====================
// UPDATE USERNAME
// ====================
bool DatabaseManager::updateUsername(
    int userId,
    const std::string& newUsername)
{
    if (!isOpen())
    {
        if (!initialize())
        {
            setError("Failed to reconnect to database: " + lastError);
            return false;
        }
    }

    const char* sql = R"(
        UPDATE Users
        SET username = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    result = sqlite3_bind_text(
        stmt, 1, newUsername.c_str(), -1, SQLITE_TRANSIENT
    );

    if (result == SQLITE_OK)
        result = sqlite3_bind_int(stmt, 2, userId);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    int updatedRows = sqlite3_changes(db);

    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    if (updatedRows == 0)
    {
        setError("No rows were updated.");
        return false;
    }

    clearError();
    return true;
}


// ====================
// UPDATE EMAIL
// ====================

bool DatabaseManager::updateEmail(int userId, const std::string& newEmail) {
    if (!isOpen())
    {
        if (!initialize())
        {
            setError("Failed to reconnect to database: " + lastError);
            return false;
        }
    }

    const char* sql = R"(
        UPDATE Users
        SET email = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    //bind email
    result = sqlite3_bind_text(
        stmt, 1, newEmail.c_str(), -1, SQLITE_TRANSIENT
    );

    //bind id
    if (result == SQLITE_OK)
        result = sqlite3_bind_int(stmt, 2, userId);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    int updatedRows = sqlite3_changes(db);

    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    if (updatedRows == 0)
    {
        setError("No rows were updated.");
        return false;
    }

    clearError();
    return true;
}

// ====================
// UPDATE PASSWORD
// ====================

bool DatabaseManager::updatePassword(int userId, const std::string& newPassword) {
    if (!isOpen())
    {
        if (!initialize())
        {
            setError("Failed to reconnect to database: " + lastError);
            return false;
        }
    }

    const char* sql = R"(
        UPDATE Users
        SET password = ?
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    //bind newPassword
    result = sqlite3_bind_text(
        stmt, 1, newPassword.c_str(), -1, SQLITE_TRANSIENT
    );

    //bind id
    if (result == SQLITE_OK)
        result = sqlite3_bind_int(stmt, 2, userId);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    int updatedRows = sqlite3_changes(db);

    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    if (updatedRows == 0)
    {
        setError("No rows were updated.");
        return false;
    }

    clearError();
    return true;
}


// ====================
// DELETE USER
// ====================

bool DatabaseManager::deleteUser(int userId)
{
    // Reconnect if database connection is closed
    if (!isOpen())
    {
        if (!initialize())
        {
            throw std::runtime_error(
                "Failed to reconnect to database: " + lastError
            );
        }
    }

    const char* sql = R"(
        DELETE FROM Users
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;

    // Prepare statement
    int result = sqlite3_prepare_v2(
        db, sql, -1, &stmt, nullptr
    );

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    // Bind user ID
    result = sqlite3_bind_int(stmt, 1, userId);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // Execute DELETE statement
    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE)
    {
        setError(sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // Check affected rows
    int deletedRows = sqlite3_changes(db);

    result = sqlite3_finalize(stmt);

    if (result != SQLITE_OK)
    {
        setError(sqlite3_errmsg(db));
        return false;
    }

    if (deletedRows == 0)
    {
        setError("No user found with ID: " + std::to_string(userId));
        return false;
    }

    clearError();

    return true;
}