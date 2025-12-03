-- Sort tags hierarchically to guarantee that a tag is selected before all of its children
WITH RECURSIVE cte AS (
    SELECT 0 AS lvl, tags.*
        FROM tags
        WHERE parent_uuid IS NULL
    UNION ALL
    SELECT cte.lvl + 1, tags.*
        FROM tags
        INNER JOIN cte ON cte.uuid = tags.parent_uuid
)
SELECT name, color, uuid, parent_uuid FROM cte
ORDER BY lvl, name;
