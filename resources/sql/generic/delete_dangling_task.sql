-- Remove a task and its dangling subtree (no connection to other parts of the complete task tree).
-- If the task has remaining dependents, it is not deleted.
WITH RECURSIVE
tasks_with_parents AS (
        SELECT
                T.uuid,
                D.dependent_uuid AS parent_uuid
        FROM tasks T
        LEFT JOIN dependencies D
               ON T.uuid = D.prerequisite_uuid
),
tasks_with_ultimate_parent AS (
        SELECT
                uuid,
                parent_uuid,
                uuid AS "ultimate_parent",
                CASE
                        WHEN uuid IN (#tasks_to_delete#) THEN TRUE
                        ELSE FALSE
                END AS part_of_subtree
        FROM tasks_with_parents TwP
        WHERE TwP.parent_uuid IS NULL
        UNION ALL
        SELECT
                TwP.uuid,
                TwP.parent_uuid,
                TwuP.ultimate_parent,
                TwuP.part_of_subtree
        FROM tasks_with_parents TwP
        INNER JOIN tasks_with_ultimate_parent TwuP
           ON TwP.parent_uuid = TwuP.uuid
),
dangling AS (
        SELECT uuid
        FROM tasks_with_ultimate_parent
        GROUP BY uuid
        HAVING COUNT(DISTINCT ultimate_parent) = 1
           AND part_of_subtree
)
DELETE FROM tasks WHERE uuid IN dangling;
