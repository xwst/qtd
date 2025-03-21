DELETE
FROM dependencies
WHERE dependent_uuid = ?
  AND prerequisite_uuid = ?;
