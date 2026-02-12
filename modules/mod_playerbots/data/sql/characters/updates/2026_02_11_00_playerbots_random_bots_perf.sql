-- Guard update for DBs where this table may not exist yet.
SET @playerbots_random_bots_exists := (
    SELECT COUNT(*)
    FROM information_schema.tables
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
);

-- Normalize rows for unique event indexing.
SET @sql := IF(
    @playerbots_random_bots_exists > 0,
    'UPDATE `playerbots_random_bots` SET `event` = '''' WHERE `event` IS NULL',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Keep only one row per (owner, bot, event), preserving the newest by id.
SET @sql := IF(
    @playerbots_random_bots_exists > 0,
    'DELETE prb1 FROM `playerbots_random_bots` prb1 INNER JOIN `playerbots_random_bots` prb2 ON prb1.`owner` = prb2.`owner` AND prb1.`bot` = prb2.`bot` AND prb1.`event` = prb2.`event` AND prb1.`id` < prb2.`id`',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Drop legacy single-column indexes only if they exist.
SET @has_owner_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'owner'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_owner_idx > 0,
    'ALTER TABLE `playerbots_random_bots` DROP INDEX `owner`',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_bot_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'bot'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_bot_idx > 0,
    'ALTER TABLE `playerbots_random_bots` DROP INDEX `bot`',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_event_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'event'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_event_idx > 0,
    'ALTER TABLE `playerbots_random_bots` DROP INDEX `event`',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Add target indexes only if they are missing.
SET @has_uniq_owner_bot_event_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'uniq_owner_bot_event'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_uniq_owner_bot_event_idx = 0,
    'ALTER TABLE `playerbots_random_bots` ADD UNIQUE KEY `uniq_owner_bot_event` (`owner`, `bot`, `event`)',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_idx_owner_event_bot_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'idx_owner_event_bot'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_idx_owner_event_bot_idx = 0,
    'ALTER TABLE `playerbots_random_bots` ADD KEY `idx_owner_event_bot` (`owner`, `event`, `bot`)',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_idx_owner_bot_idx := (
    SELECT COUNT(*)
    FROM information_schema.statistics
    WHERE table_schema = DATABASE()
      AND table_name = 'playerbots_random_bots'
      AND index_name = 'idx_owner_bot'
);
SET @sql := IF(
    @playerbots_random_bots_exists > 0 AND @has_idx_owner_bot_idx = 0,
    'ALTER TABLE `playerbots_random_bots` ADD KEY `idx_owner_bot` (`owner`, `bot`)',
    'DO 0'
);
PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;
