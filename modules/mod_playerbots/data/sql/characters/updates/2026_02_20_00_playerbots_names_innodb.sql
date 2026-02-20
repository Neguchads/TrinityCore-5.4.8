SET @has_playerbots_names := (
    SELECT COUNT(*)
    FROM information_schema.tables
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_names'
);

SET @sql := IF(
    @has_playerbots_names > 0,
    'DO 0',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_playerbots_guild_names := (
    SELECT COUNT(*)
    FROM information_schema.tables
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_guild_names'
);

SET @sql := IF(
    @has_playerbots_guild_names > 0,
    'DO 0',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_ai_playerbot_guild_names := (
    SELECT COUNT(*)
    FROM information_schema.tables
    WHERE table_schema = DATABASE()
      AND table_name = 'ai_playerbot_guild_names'
);

SET @sql := IF(
    @has_ai_playerbot_guild_names > 0,
    'DO 0',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;
