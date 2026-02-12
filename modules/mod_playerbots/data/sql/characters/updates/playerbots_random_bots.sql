DROP TABLE IF EXISTS `playerbots_random_bots`;
CREATE TABLE `playerbots_random_bots` (
  `id` INT(11) NOT NULL AUTO_INCREMENT,
  `owner` INT(11) NOT NULL,
  `bot` INT(11) NOT NULL,
  `time` INT(11) NOT NULL,
  `validIn` INT(11) DEFAULT NULL,
  `event` varchar(45) DEFAULT NULL,
  `value` INT(11) DEFAULT NULL,
  `data` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uniq_owner_bot_event` (`owner`,`bot`,`event`),
  KEY `idx_owner_event_bot` (`owner`,`event`,`bot`),
  KEY `idx_owner_bot` (`owner`,`bot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
