CREATE TABLE IF NOT EXISTS users (
    name VARCHAR(128) PRIMARY KEY
);


CREATE TABLE IF NOT EXISTS tags (
    uuid VARCHAR(36) PRIMARY KEY,
    name VARCHAR(128) NOT NULL,
    color VARCHAR(9),
    parent_uuid VARCHAR(36),
    FOREIGN KEY(parent_uuid) REFERENCES tags(uuid)
);


CREATE TABLE IF NOT EXISTS tasks (
    uuid VARCHAR(36) PRIMARY KEY,
    id INTEGER UNIQUE,
    title VARCHAR(1024),
    text TEXT
);


CREATE TABLE IF NOT EXISTS tag_assignments (
    task_uuid VARCHAR(36),
    tag_uuid VARCHAR(36),
    UNIQUE (task_uuid, tag_uuid),
    FOREIGN KEY(task_uuid) REFERENCES tasks(uuid),
    FOREIGN KEY(tag_uuid) REFERENCES tags(uuid)
);


CREATE TABLE IF NOT EXISTS dependencies (
    dependent_uuid VARCHAR(36) NOT NULL,
    prerequisite_uuid VARCHAR(36) NOT NULL
        CHECK(prerequisite_uuid != dependent_uuid),
    FOREIGN KEY(dependent_uuid) REFERENCES tasks(uuid),
    FOREIGN KEY(prerequisite_uuid) REFERENCES tasks(uuid),
    PRIMARY KEY(dependent_uuid, prerequisite_uuid)
);
