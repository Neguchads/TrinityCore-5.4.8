SET @ENTRY:=35410;
DELETE FROM `trinity_string` WHERE `entry`=@ENTRY;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(@ENTRY, 'Hello World from mod-example!');
