PRAGMA foreign_keys = ON;

-- =========================
-- USERS
-- =========================

CREATE TABLE IF NOT EXISTS Users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    username TEXT NOT NULL
        CHECK(length(username) <= 50),

    email TEXT NOT NULL UNIQUE
        CHECK(length(email) <= 50),

    password TEXT NOT NULL
        CHECK(length(password) = 8)
);


-- =========================
-- QUESTIONS
-- =========================

CREATE TABLE IF NOT EXISTS Questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    question_header TEXT NOT NULL
        CHECK(length(question_header) <= 50),

    answer TEXT
        CHECK(answer IS NULL OR length(answer) <= 1000),

    from_user_id INTEGER NOT NULL,
    to_user_id INTEGER NOT NULL,

    datetime TEXT DEFAULT CURRENT_TIMESTAMP,

    parent_ID INTEGER,
    child_ID INTEGER,

    UNIQUE(parent_ID),
    UNIQUE(child_ID),

    CHECK(parent_ID IS NULL OR parent_ID != id),
    CHECK(child_ID IS NULL OR child_ID != id),

    FOREIGN KEY (from_user_id)
        REFERENCES Users(id),

    FOREIGN KEY (to_user_id)
        REFERENCES Users(id),

    FOREIGN KEY (parent_ID)
        REFERENCES Questions(id)
        ON DELETE SET NULL,

    FOREIGN KEY (child_ID)
        REFERENCES Questions(id)
        ON DELETE SET NULL
);


-- =========================
-- DELETE TRIGGER
-- =========================

CREATE TRIGGER IF NOT EXISTS reconnect_questions
AFTER DELETE ON Questions
FOR EACH ROW
BEGIN

    UPDATE Questions
    SET child_ID = OLD.child_ID
    WHERE id = OLD.parent_ID;

    UPDATE Questions
    SET parent_ID = OLD.parent_ID
    WHERE id = OLD.child_ID;

END;