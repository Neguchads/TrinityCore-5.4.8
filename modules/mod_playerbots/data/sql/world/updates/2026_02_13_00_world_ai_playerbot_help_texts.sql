-- CMaNGOS playerbots parity: help text storage
-- Schema-only (no inserts). CMaNGOS ships large expansion-specific content sets.

CREATE TABLE IF NOT EXISTS `ai_playerbot_help_texts` (
  `id` smallint(20) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL COMMENT 'name - type:subject',
  `template_changed` tinyint(4) NOT NULL DEFAULT '0' COMMENT 'template_changed - Has the template text changed after text?',
  `template_text` text NOT NULL COMMENT 'generated text',
  `text` text NOT NULL COMMENT 'text',
  `text_loc1` text NOT NULL,
  `text_loc2` text NOT NULL,
  `text_loc3` text NOT NULL,
  `text_loc4` text NOT NULL,
  `text_loc5` text NOT NULL,
  `text_loc6` text NOT NULL,
  `text_loc7` text NOT NULL,
  `text_loc8` text NOT NULL,
  `locs_updated` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'locs_updated - Have the loc texts been updated?',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

