-- LFG queue announcer strings
DELETE FROM `trinity_string` WHERE `entry` IN (40000, 40001);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(40000, '|cffff0000[LFG Queue Announcer]:|r %s (Level %u %s - %s, %s) queued for %s via %s|r'),
(40001, '|cffff0000[LFG Queue Announcer]:|r %s (Level %u %s - %s, %s) queued for %s via %s|r')
ON DUPLICATE KEY UPDATE
`content_default` = VALUES(`content_default`);
