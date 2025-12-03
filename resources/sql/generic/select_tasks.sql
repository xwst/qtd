-- Sort tasks hierarchically to guarantee that a task is selected before all of its dependencies
WITH RECURSIVE tasks_with_parents AS (
        SELECT T.*, D.dependent_uuid AS parent_uuid
        FROM tasks T
        LEFT JOIN dependencies D
               ON T.uuid = D.prerequisite_uuid
),
cte AS (
        SELECT 0 AS lvl, TwP.*
        FROM tasks_with_parents TwP
        WHERE TwP.parent_uuid IS NULL
        UNION ALL
        SELECT cte.lvl + 1, TwP.*
        FROM tasks_with_parents TwP
        INNER JOIN cte
                ON cte.uuid = TwP.parent_uuid
),
hierarchical_sorted AS (
        SELECT  *,
                ROW_NUMBER() OVER (
                        PARTITION BY uuid
                        ORDER BY lvl DESC
                ) AS lvl2
        FROM cte
)
SELECT title, status, start_datetime, due_datetime, resolve_datetime, content_text, uuid, content_data
FROM hierarchical_sorted
WHERE lvl2 = 1
ORDER BY lvl;
