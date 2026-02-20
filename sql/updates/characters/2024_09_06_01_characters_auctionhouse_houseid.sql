SET @has_auctioneerguid := (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'auctionhouse' AND COLUMN_NAME = 'auctioneerguid');

SET @has_houseid := (SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'auctionhouse' AND COLUMN_NAME = 'houseid');

SET @update_sql := IF(@has_auctioneerguid > 0, 'UPDATE `auctionhouse` SET `time` = 0, `auctioneerguid` = 7;', IF(@has_houseid > 0, 'UPDATE `auctionhouse` SET `time` = 0, `houseid` = 7;', 'SELECT 1;'));
PREPARE stmt FROM @update_sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @alter_sql := IF(@has_auctioneerguid > 0, 'ALTER TABLE `auctionhouse` CHANGE COLUMN `auctioneerguid` `houseid` TINYINT(3) UNSIGNED NOT NULL DEFAULT ''7'' AFTER `id`;', 'SELECT 1;');
PREPARE stmt2 FROM @alter_sql;
EXECUTE stmt2;
DEALLOCATE PREPARE stmt2;
