-- Ensure faction-owned capital city teleports do not route opposite-faction bots.
-- Stormwind should block Horde (67), Orgrimmar should block Alliance (469).

UPDATE `playerbot_city`
SET `TeamsDisabled` = 67
WHERE `ZoneId` = 1519;

UPDATE `playerbot_city`
SET `TeamsDisabled` = 469
WHERE `ZoneId` = 1637;
