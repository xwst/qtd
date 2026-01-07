-------------------------------------------------
---------------------- Tags ---------------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS tags (
      uuid          VARCHAR(36)  PRIMARY KEY
    , name          VARCHAR(128) NOT NULL
    , color         VARCHAR(9)
    , parent_uuid   VARCHAR(36)
    , last_modified VARCHAR(30)
    , FOREIGN KEY (parent_uuid) REFERENCES tags(uuid) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS tags_insert_last_modified
AFTER INSERT ON tags
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE tags
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE uuid = NEW.uuid;
    END;

CREATE TRIGGER IF NOT EXISTS tags_update_last_modified
AFTER UPDATE ON tags
FOR EACH ROW
    WHEN
        OLD.last_modified = NEW.last_modified
        AND (
               OLD.name        != NEW.name
            OR OLD.color       != NEW.color
            OR OLD.parent_uuid != NEW.parent_uuid
        )
    BEGIN
        UPDATE tags
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE uuid = NEW.uuid;
    END;

-------------------------------------------------
--------------------- Tasks ---------------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS tasks (
      uuid             VARCHAR(36)   PRIMARY KEY
    , title            VARCHAR(1024) NOT NULL
    , status           VARCHAR(10)   NOT NULL DEFAULT 'open'
                                     CHECK(status IN ('open', 'closed'))
    , start_datetime   VARCHAR(30)
    , due_datetime     VARCHAR(30)
    , resolve_datetime VARCHAR(30)
    , content_text     TEXT
    , last_modified    VARCHAR(30)
);

CREATE INDEX IF NOT EXISTS index_tasks_status_due_datetime
    ON tasks (status, due_datetime);

CREATE TRIGGER IF NOT EXISTS tasks_insert_last_modified
AFTER INSERT ON tasks
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE tasks
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE uuid = NEW.uuid;
    END;

CREATE TRIGGER IF NOT EXISTS tasks_update_last_modified
AFTER UPDATE ON tasks
FOR EACH ROW
    WHEN
        OLD.last_modified = NEW.last_modified
        AND (
               OLD.title            != NEW.title
            OR OLD.status           != NEW.status
            OR OLD.start_datetime   != NEW.start_datetime
            OR OLD.due_datetime     != NEW.due_datetime
            OR OLD.resolve_datetime != NEW.resolve_datetime
            OR OLD.content_text     != NEW.content_text
        )
    BEGIN
        UPDATE tasks
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE uuid = NEW.uuid;
    END;

-------------------------------------------------
--------------------- Media ---------------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS media (
      media_hash    VARCHAR(64) PRIMARY KEY
    , media_data    BLOB
    , last_modified VARCHAR(30)
);

CREATE TRIGGER IF NOT EXISTS media_insert_last_modified
AFTER INSERT ON media
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE media
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE media_hash = NEW.media_hash;
    END;

-------------------------------------------------
-------------- Media-Task Relation --------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS task_media (
      task_uuid     VARCHAR(36)
    , media_hash    VARCHAR(64)
    , last_modified VARCHAR(30)
    , PRIMARY KEY (task_uuid, media_hash)
    , FOREIGN KEY (task_uuid)  REFERENCES atasks (uuid)       ON DELETE CASCADE
    , FOREIGN KEY (media_hash) REFERENCES media (media_hash) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS task_media_insert_last_modified
AFTER INSERT ON task_media
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE task_media
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE task_uuid  = NEW.task_uuid
          AND media_hash = NEW.media_hash;
    END;

-------------------------------------------------
---------------- Tag Assignments ----------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS tag_assignments (
      task_uuid     VARCHAR(36)
    , tag_uuid      VARCHAR(36)
    , last_modified VARCHAR(30)
    , PRIMARY KEY (task_uuid, tag_uuid)
    , FOREIGN KEY (task_uuid)           REFERENCES tasks (uuid) ON DELETE CASCADE
    , FOREIGN KEY (tag_uuid)            REFERENCES tags  (uuid) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS tag_assignments_insert_last_modified
AFTER INSERT ON tag_assignments
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE tag_assignments
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE task_uuid = NEW.task_uuid
          AND tag_uuid  = NEW.tag_uuid;
    END;

-------------------------------------------------
--------------- Task Dependencies ---------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS dependencies (
      dependent_uuid    VARCHAR(36)
    , prerequisite_uuid VARCHAR(36)
        NOT NULL
        CHECK(prerequisite_uuid != dependent_uuid)
    , last_modified VARCHAR(30)
    , PRIMARY KEY (dependent_uuid, prerequisite_uuid)
    , FOREIGN KEY (dependent_uuid)    REFERENCES tasks(uuid) ON DELETE CASCADE
    , FOREIGN KEY (prerequisite_uuid) REFERENCES tasks(uuid) ON DELETE CASCADE
);

CREATE TRIGGER IF NOT EXISTS dependencies_insert_last_modified
AFTER INSERT ON dependencies
FOR EACH ROW
    WHEN NEW.last_modified IS NULL
    BEGIN
        UPDATE dependencies
        SET last_modified = strftime('%Y-%m-%dT%H:%M:%SZ', 'now', 'utc')
        WHERE dependent_uuid    = NEW.dependent_uuid
          AND prerequisite_uuid = NEW.prerequisite_uuid;
    END;

-------------------------------------------------
----------------- Configuration -----------------
-------------------------------------------------
CREATE TABLE IF NOT EXISTS config (
      "key"   TEXT PRIMARY KEY
    , "value" TEXT
);
