DELETE
FROM tasks
WHERE uuid IN (
    SELECT uuid
    FROM tasks T
    LEFT JOIN dependencies D
        ON T.uuid = D.prerequisite_uuid
    WHERE D.dependent_uuid IS NULL
);
