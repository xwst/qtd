CREATE TABLE IF NOT EXISTS tags (
      uuid        VARCHAR(36)   PRIMARY KEY
    , name        VARCHAR(128)  NOT NULL
    , color       VARCHAR(9)
    , parent_uuid VARCHAR(36)
    , FOREIGN KEY (parent_uuid) REFERENCES tags(uuid) ON DELETE CASCADE
);


CREATE TABLE IF NOT EXISTS tasks (
      uuid             VARCHAR(36)   PRIMARY KEY
    , title            VARCHAR(1024) NOT NULL
    , status           VARCHAR(10)   NOT NULL DEFAULT 'open'
                                     CHECK(status IN ('open', 'closed'))
    , start_datetime   VARCHAR(30)
    , due_datetime     VARCHAR(30)
    , resolve_datetime VARCHAR(30)
    , content_text     TEXT
);
CREATE INDEX IF NOT EXISTS index_tasks_status_due_datetime
    ON tasks (status, due_datetime);

CREATE TABLE IF NOT EXISTS media (
      media_hash VARCHAR(64)
    , media_data BLOB
);
CREATE INDEX IF NOT EXISTS index_media_hash
    ON media (media_hash);

CREATE TABLE IF NOT EXISTS task_media (
      task_uuid  VARCHAR(36)
    , media_hash VARCHAR(64)
    , FOREIGN KEY (task_uuid)  REFERENCES atasks (uuid)       ON DELETE CASCADE
    , FOREIGN KEY (media_hash) REFERENCES media (media_hash) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS index_task_media_task_uuid
    ON task_media (task_uuid);

CREATE TABLE IF NOT EXISTS tag_assignments (
      task_uuid VARCHAR(36)
    , tag_uuid  VARCHAR(36)
    , UNIQUE       (task_uuid, tag_uuid)
    , FOREIGN KEY  (task_uuid)           REFERENCES tasks (uuid) ON DELETE CASCADE
    , FOREIGN KEY  (tag_uuid)            REFERENCES tags  (uuid) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS index_tag_assignments_task_uuid
    ON tag_assignments (task_uuid);


CREATE TABLE IF NOT EXISTS dependencies (
      dependent_uuid    VARCHAR(36)
    , prerequisite_uuid VARCHAR(36)   NOT NULL
                                      CHECK(prerequisite_uuid != dependent_uuid)
    , UNIQUE      (dependent_uuid, prerequisite_uuid)
    , FOREIGN KEY (dependent_uuid)    REFERENCES tasks(uuid) ON DELETE CASCADE
    , FOREIGN KEY (prerequisite_uuid) REFERENCES tasks(uuid) ON DELETE CASCADE
);
