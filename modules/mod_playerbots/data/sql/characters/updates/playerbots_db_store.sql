DROP TABLE IF EXISTS `playerbots_db_store`;
CREATE TABLE `playerbots_db_store` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `guid` bigint(20) NOT NULL,
  `preset` varchar(32) NOT NULL DEFAULT '',
  `key` varchar(32) NOT NULL,
  `value` varchar(4000) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `guid` (`guid`),
  KEY `guid_preset` (`guid`, `preset`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
