WITH RECURSIVE cte AS (
    SELECT 0 AS lvl, tags.*
        FROM tags
        WHERE parent_uuid = '' OR parent_uuid IS NULL
    UNION ALL
    SELECT cte.lvl + 1, tags.*
        FROM tags
        INNER JOIN cte ON cte.uuid = tags.parent_uuid
)
SELECT uuid, name, color, parent_uuid FROM cte
ORDER BY lvl;
