CREATE TABLE IF NOT EXISTS tags (
      uuid        VARCHAR(36)   PRIMARY KEY
    , name        VARCHAR(128)  NOT NULL
    , color       VARCHAR(9)
    , parent_uuid VARCHAR(36)
    , FOREIGN KEY (parent_uuid) REFERENCES tags(uuid)
);


CREATE TABLE IF NOT EXISTS tasks (
      uuid           VARCHAR(36)   PRIMARY KEY
    , title          VARCHAR(1024) NOT NULL
    , status         VARCHAR(6)    NOT NULL  DEFAULT 'open'
                                   CHECK(status IN ('open', 'closed'))
    , start_datetime VARCHAR(25)
    , due_datetime   VARCHAR(25)
    , content_text   TEXT
    , content_data   BLOB
);
CREATE INDEX IF NOT EXISTS index_tasks_status_due_datetime
    ON tasks (status, due_datetime);


CREATE TABLE IF NOT EXISTS tag_assignments (
      task_uuid VARCHAR(36)
    , tag_uuid  VARCHAR(36)
    , UNIQUE       (task_uuid, tag_uuid)
    , FOREIGN KEY  (task_uuid)           REFERENCES tasks (uuid)
    , FOREIGN KEY  (tag_uuid)            REFERENCES tags  (uuid)
);
CREATE INDEX IF NOT EXISTS index_tag_assignments_task_uuid
    ON tag_assignments (task_uuid);


CREATE TABLE IF NOT EXISTS dependencies (
      dependent_uuid    VARCHAR(36)   NOT NULL
    , prerequisite_uuid VARCHAR(36)   NOT NULL
                                      CHECK(prerequisite_uuid != dependent_uuid)
    , UNIQUE      (dependent_uuid, prerequisite_uuid)
    , FOREIGN KEY (dependent_uuid)    REFERENCES tasks(uuid)
    , FOREIGN KEY (prerequisite_uuid) REFERENCES tasks(uuid)
);
