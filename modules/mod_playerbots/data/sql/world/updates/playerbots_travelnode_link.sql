DROP TABLE IF EXISTS `playerbots_travelnode_link`;
CREATE TABLE IF NOT EXISTS `playerbots_travelnode_link` (
`node_id` mediumint(8) NOT NULL,
`to_node_id` mediumint(8) NOT NULL,
`type` tinyint(3) NOT NULL,
`object` mediumint(8) NOT NULL,
`distance` float NOT NULL,
`swim_distance` float NOT NULL,
`extra_cost` float NOT NULL,
`calculated` tinyint(1) NOT NULL,
`max_creature_0` tinyint(2) NOT NULL,
`max_creature_1` tinyint(2) NOT NULL,
`max_creature_2` tinyint(2) NOT NULL,
PRIMARY KEY (`node_id`,`to_node_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='PlayerbotAI Travel Node link';
