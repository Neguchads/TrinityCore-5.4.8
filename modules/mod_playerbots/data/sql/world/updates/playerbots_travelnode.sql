DROP TABLE IF EXISTS `playerbots_travelnode`;
CREATE TABLE IF NOT EXISTS `playerbots_travelnode` (
`id` mediumint(8) NOT NULL,
`name` varchar(1024) NOT NULL,
`map_id` mediumint(8) NOT NULL,
`x` float NOT NULL,
`y` float NOT NULL,
`z` float NOT NULL,
`linked` tinyint(2) DEFAULT NULL,
PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='PlayerbotAI Travel Node';
