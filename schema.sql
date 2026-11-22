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
-- CONVERSATIONS
-- =========================

CREATE TABLE IF NOT EXISTS Conversations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    creation_date TEXT DEFAULT CURRENT_TIMESTAMP,

    User1_ID INTEGER NOT NULL
        REFERENCES Users(id),

    User2_ID INTEGER NOT NULL
        REFERENCES Users(id),

    CHECK(User1_ID != User2_ID)
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

    owner_ID INTEGER NOT NULL
        REFERENCES Users(id),

    conversation_ID INTEGER NOT NULL
        REFERENCES Conversations(id)
        ON DELETE CASCADE,

    datetime TEXT DEFAULT CURRENT_TIMESTAMP,

    parent_ID INTEGER,
    child_ID INTEGER,

    UNIQUE(parent_ID),
    UNIQUE(child_ID),

    CHECK(parent_ID IS NULL OR parent_ID != id),
    CHECK(child_ID IS NULL OR child_ID != id),

    FOREIGN KEY (parent_ID)
        REFERENCES Questions(id)
        ON DELETE SET NULL,

    FOREIGN KEY (child_ID)
        REFERENCES Questions(id)
        ON DELETE SET NULL
);

-- =========================
-- TRIGGERS
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


CREATE TRIGGER IF NOT EXISTS prevent_duplicate_conversation
BEFORE INSERT ON Conversations
FOR EACH ROW
BEGIN
    SELECT RAISE(ABORT, 'Conversation already exists')
    WHERE EXISTS (
        SELECT 1
        FROM Conversations
        WHERE User1_ID = NEW.User1_ID
          AND User2_ID = NEW.User2_ID
    )
    OR EXISTS (
        SELECT 1
        FROM Conversations
        WHERE User1_ID = NEW.User2_ID
          AND User2_ID = NEW.User1_ID
    );
END;