WITH RECURSIVE cte AS (
    SELECT uuid
        FROM tags
        WHERE uuid = ?
    UNION ALL
    SELECT tags.uuid
        FROM tags
        INNER JOIN cte ON cte.uuid = tags.parent_uuid
)
DELETE FROM tags
WHERE uuid IN (SELECT uuid FROM cte);
