DROP TABLE IF EXISTS `playerbots_travelnode_path`;
CREATE TABLE IF NOT EXISTS `playerbots_travelnode_path` (
`node_id` mediumint(8) NOT NULL,
`to_node_id` mediumint(8) NOT NULL,
`nr` mediumint(8) NOT NULL,
`map_id` mediumint(8) NOT NULL,
`x` float NOT NULL,
`y` float NOT NULL,
`z` float NOT NULL,
PRIMARY KEY (`node_id`,`to_node_id`,`nr`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='PlayerbotAI Travel Node path';
