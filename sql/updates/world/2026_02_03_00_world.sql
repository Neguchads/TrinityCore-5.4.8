-- Quest 26616 "It's Never Over" - Keeshan's Riverboat passengers, dialog, Messner engine, post-boat NPC positions

-- Ensure SmartAI for involved NPCs
UPDATE `creature_template` SET `AIName`='SmartAI'
WHERE `entry` IN (43443, 43445, 43446, 43447, 43448, 43449, 43458, 43459, 43460, 43461, 43462, 112962);

-- Ensure boat is a spellclick vehicle
UPDATE `creature_template`
SET `AIName`='SmartVehicleAI', `npcflag`=16777216, `VehicleId`=964,
    `unit_flags`=(`unit_flags` | 0x00000200)
WHERE `entry`=43450;


-- Vehicle accessories (boat passengers)
DELETE FROM `vehicle_template_accessory` WHERE `entry`=43450;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(43450, 43449, 0, 1, 'Keeshan', 8, 0),
(43450, 43448, 1, 1, 'Messner', 8, 0),
(43450, 43447, 2, 1, 'Jorgensen', 8, 0),
(43450, 43446, 3, 1, 'Krakauer', 8, 0),
(43450, 43445, 4, 1, 'Danforth', 8, 0);


-- Spellclick to board the boat
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=43450 AND `spell_id`=46598;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(43450, 46598, 1, 0);

-- Creature text
DELETE FROM `creature_text` WHERE `CreatureID` IN (43448, 43449, 43458, 43459, 43460, 43461, 43462);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(43448, 0, 0, 'You got it, Johnny!', 12, 0, 100, 0, 0, 0, 0, 43525, 0, 'Messner Says'),
(43449, 0, 0, 'Take us out, Messner.', 12, 0, 100, 0, 0, 0, 0, 43524, 0, 'Keeshan Says'),
(43449, 1, 0, 'We\'re here, everybody out!', 12, 0, 100, 22, 0, 0, 0, 43526, 0, 'Keeshan Says'),
(43458, 0, 0, 'Danforth, report.', 12, 0, 100, 0, 0, 0, 0, 43592, 0, 'Keeshan Says'),
(43458, 1, 0, 'Krakauer?', 12, 0, 100, 0, 0, 0, 0, 43594, 0, 'Keeshan Says'),
(43458, 2, 0, 'Jorgensen, what do you have on Render\'s Valley?', 12, 0, 100, 0, 0, 0, 0, 43597, 0, 'Keeshan Says'),
(43458, 3, 0, 'Death trap, eh? Messner, I got a plan. Gonna need some seaforium.', 12, 0, 100, 381, 0, 0, 0, 43599, 0, 'Keeshan Says'),
(43458, 4, 0, 'Not my problem, Messner. Make it happen.', 12, 0, 100, 274, 0, 0, 0, 43601, 0, 'Keeshan Says'),
(43458, 5, 0, '$N, Get your crap ready. I\'m sending you out in the field.', 12, 0, 100, 25, 0, 0, 0, 0, 0, 'Keeshan Says'),
(43459, 0, 0, 'Seaforium? Where the hell am I gonna get seaforium from, Johnny?', 12, 0, 100, 0, 0, 0, 0, 43600, 0, 'Messner Says'),
(43460, 0, 0, 'Blackrocks all over the damned place, Johnny. One main tower at the choke and two smaller huts. It\'s a death trap.', 12, 0, 100, 0, 0, 0, 0, 0, 0, 'Jorgensen Says'),
(43461, 0, 0, 'Two ways to get behind enemy lines, Johnny. Three if you count bein\' dead.', 12, 0, 100, 0, 0, 0, 0, 43595, 0, 'Krakauer Says'),
(43461, 1, 0, 'North through Alther\'s Mill, but we\'d have to cut through an army of bloodthirsty worgs or south through Render\'s Valley.', 12, 0, 100, 0, 0, 0, 0, 43596, 0, 'Krakauer Says'),
(43462, 0, 0, 'The Blackrock orcs blew up the bridge leading to Stonewatch Keep. Frontal assault is out of the question.', 12, 0, 100, 0, 0, 0, 0, 43593, 0, 'Danforth Says');

-- SmartAI: boat passengers + dialog
DELETE FROM `smart_scripts`
WHERE `source_type` IN (0, 9)
  AND `entryorguid` IN (43443, 43446, 43447, 43448, 43449, 43450, 4344900, 4345000, 4345001, 4344300, 4344301, 43458, 4345800, 112962);

INSERT INTO `smart_scripts` (
`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
`event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
`action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
`target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`,
`target_x`, `target_y`, `target_z`, `target_o`, `comment`
) VALUES
-- Krakauer (boat)
(43446, 0, 0, 0, 38, 0, 100, 1, 5, 5, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Krakauer - On Data Set 5 5 - Despawn'),
(43446, 0, 1, 0, 1, 0, 100, 1, 1, 2, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0.5038, 'Krakauer - On Just Summoned - Set Orientation to Bunny'),

-- Jorgensen (boat)
(43447, 0, 0, 0, 38, 0, 100, 1, 5, 5, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Jorgensen - On Data Set 5 5 - Despawn'),
(43447, 0, 1, 0, 1, 0, 100, 1, 1, 2, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0.5038, 'Jorgensen - On Just Summoned - Set Orientation to Bunny'),

-- Messner (boat)
(43448, 0, 0, 0, 1, 0, 100, 1, 1, 2, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0.5038, 'Messner - On Just Summoned - Set Orientation to Bunny'),
(43448, 0, 1, 0, 38, 0, 100, 1, 5, 5, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Messner - On Data Set 5 5 - Despawn'),
(43448, 0, 2, 0, 38, 0, 100, 1, 1, 1, 0, 0, 0, 11, 81260, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Messner - On Data Set 1 1 - Cast Boat Engine Spell'),
(43448, 0, 3, 0, 38, 0, 100, 1, 1, 2, 0, 0, 0, 28, 81260, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Messner - On Data Set 1 2 - Remove Boat Engine Spell'),

-- Keeshan (boat)
(43449, 0, 0, 0, 38, 0, 100, 1, 1, 2, 0, 0, 0, 80, 4344900, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan - On Data Set 1 2 - Run Script'),

-- Boat despawn script
(4344900, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 2000, 2000, 0, 45, 5, 5, 0, 0, 0, 0, 9, 0, 0, 10, 0, 0, 0, 0, 0, 'Script - Set Data 5 5 on Everyone'),
(4344900, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Despawn'),

-- Keeshan's Riverboat (dock click)
(43443, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan\'s Riverboat - On Gossip Hello - Close Gossip'),
(43443, 0, 1, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 80, 4344300, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan\'s Riverboat - On Gossip Hello - Run Mount Action List'),
(43443, 0, 2, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 80, 4344301, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan\'s Riverboat - On Gossip Hello - Run Summon Action List'),

-- Keeshan's Riverboat (boat creature)
(
43450, 0, 0, 0, 27, 0, 100, 0, 0, 0, 0, 0, 0,
80, 4345000, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On Passenger Boarded - Run Script'
),
(
43450, 0, 1, 0, 40, 0, 100, 0, 21, 43450, 0, 0, 0,
80, 4345001, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On WP 21 - Run Arrival Script'
),
(
43450, 0, 2, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0,
41, 1, 0, 0, 0, 0, 0,
1, 0, 0, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On Die - Despawn Instant'
),
(
43450, 0, 3, 0, 40, 0, 100, 0, 20, 43450, 0, 0, 0,
45, 1, 2, 0, 0, 0, 0,
19, 43448, 10, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On WP 20 - Stop Messner Engine'
),
(
43450, 0, 4, 0, 27, 0, 100, 0, 0, 0, 0, 0, 0,
18, 131584, 0, 0, 0, 0, 0,
7, 0, 0, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On Passenger Boarded - Set immune/pacified'
),
(
43450, 0, 5, 0, 28, 0, 100, 0, 0, 0, 0, 0, 0,
19, 131584, 0, 0, 0, 0, 0,
7, 0, 0, 0, 0,
0, 0, 0, 0, 'Keeshan\'s Riverboat - On Passenger Removed - Clear immune/pacified'
),

-- Boat start script
(4345000, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Store Target List'),
(4345000, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43449, 10, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345000, 9, 2, 0, 0, 0, 100, 0, 2500, 2500, 2500, 2500, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43448, 10, 0, 0, 0, 0, 0, 0, 'Script - Messner Says'),
(4345000, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 1000, 1000, 0, 45, 1, 1, 0, 0, 0, 0, 19, 43448, 10, 0, 0, 0, 0, 0, 0, 'Script - Set Data 1 1 on Messner'),
(4345000, 9, 4, 0, 0, 0, 100, 0, 200, 200, 200, 200, 0, 53, 1, 43450, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Run WP'),

-- Boat arrival script
(4345001, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 1000, 1000, 0, 1, 1, 1, 0, 0, 0, 0, 19, 43449, 10, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345001, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 15, 26616, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Give Quest Credit'),
(4345001, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 45, 1, 2, 0, 0, 0, 0, 19, 43449, 10, 0, 0, 0, 0, 0, 0, 'Script - Set Data 1 2 on Keeshan'),
(4345001, 9, 3, 0, 0, 0, 100, 0, 1500, 1500, 1500, 1500, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Despawn'),

-- Dock click action list (auto-mount player on existing boat)
(4344300, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 1000, 1000, 0, 86, 46598, 4, 7, 0, 0, 0, 19, 43450, 50, 0, 0, 0, 0, 0, 0, 'ActionList - Mount player on boat'),

-- Dock click action list (summon boat + auto-mount player if missing)
(4344301, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 43450, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, -9298.79, -2367.42, 55.8253, 3.6967, 'ActionList - Summon Keeshan\'s Riverboat'),
(4344301, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 1000, 1000, 0, 86, 46598, 4, 7, 0, 0, 0, 19, 43450, 50, 0, 0, 0, 0, 0, 0, 'ActionList - Mount player on boat'),

-- Keeshan (post-boat dialog)
(43458, 0, 0, 0, 10, 0, 100, 1, 1, 20, 0, 0, 0, 80, 4345800, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan - On Player With Quest Complete Near - Run Script'),
(43458, 0, 1, 0, 20, 0, 100, 0, 26616, 0, 0, 0, 0, 78, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan - On Quest Complete - Reset Script'),
(43458, 0, 2, 0, 1, 0, 100, 1, 1000, 1000, 0, 0, 0, 80, 4345800, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Keeshan - OOC Update (Quest Complete + Nearby) - Run Script'),

-- Post-boat dialog script
(4345800, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 64, 0, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Store Target List'),
(4345800, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 3000, 3000, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43462, 20, 0, 0, 0, 0, 0, 0, 'Script - Danforth Says'),
(4345800, 9, 3, 0, 0, 0, 100, 0, 4000, 4000, 4000, 4000, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 4, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43461, 20, 0, 0, 0, 0, 0, 0, 'Script - Krakauer Says'),
(4345800, 9, 5, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 1, 1, 0, 0, 0, 0, 19, 43461, 20, 0, 0, 0, 0, 0, 0, 'Script - Krakauer Says'),
(4345800, 9, 6, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 7, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43460, 20, 0, 0, 0, 0, 0, 0, 'Script - Jorgensen Says'),
(4345800, 9, 8, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 9, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 0, 1, 0, 0, 0, 0, 19, 43459, 20, 0, 0, 0, 0, 0, 0, 'Script - Messner Says'),
(4345800, 9, 10, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Remove Kneel State'),
(4345800, 9, 11, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 1, 4, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 12, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 66, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Set Orientation to Player'),
(4345800, 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 5, 1, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Keeshan Says'),
(4345800, 9, 14, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 0, 66, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Set Orientation to Default'),
(4345800, 9, 15, 0, 0, 0, 100, 0, 100, 100, 100, 100, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Set Kneel State'),
(4345800, 9, 16, 0, 0, 0, 100, 0, 10000, 10000, 10000, 10000, 0, 78, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Script - Reset Script'),

-- Bunny summon script
(112962, 0, 0, 0, 10, 0, 100, 1, 1, 80, 0, 0, 0, 12, 43450, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, -9298.79, -2367.42, 55.8253, 3.6967, 'Bunny - On player With Quest Near - Summon Keeshan Riverboat'),
(112962, 0, 1, 0, 1, 0, 100, 0, 1000, 2000, 7000, 8000, 0, 78, 0, 8, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Bunny - On Riverboat not Near - Reset Script');

-- Conditions for post-boat dialog (only if player completed 26616 and is close)
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=22 AND `SourceEntry`=43458 AND `SourceId`=0 AND `SourceGroup`=1;
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=22 AND `SourceEntry`=43458 AND `SourceId`=0 AND `SourceGroup`=3;
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=22 AND `SourceEntry`=43443 AND `SourceId`=0 AND `SourceGroup`=1;
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=22 AND `SourceEntry`=112962 AND `SourceId`=0 AND `SourceGroup`=0;
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=18 AND `SourceGroup`=43450 AND `SourceEntry`=46598;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(22, 1, 43458, 0, 1, 32, 0, 144, 0, 0, 0, 0, 0, '', 'Script only runs on Player on Distance'),
(22, 1, 43458, 0, 1, 28, 0, 26616, 0, 0, 0, 0, 0, '', 'Script Only Runs if player has completed Quest Objectives'),
(22, 3, 43458, 0, 1, 28, 0, 26616, 0, 0, 0, 0, 0, '', 'Script Only Runs if player has completed Quest Objectives'),
(22, 3, 43458, 0, 1, 35, 0, 1, 20, 4, 0, 0, 0, '', 'Script only runs if player within 20 yards'),
(22, 1, 43443, 0, 0, 9, 0, 26616, 0, 0, 0, 0, 0, '', 'Boat click only if quest is active'),
(22, 1, 43443, 0, 0, 29, 0, 43450, 90, 0, 0, 0, 0, '', 'Boat click only if boat nearby'),
(22, 2, 43443, 0, 0, 9, 0, 26616, 0, 0, 0, 0, 0, '', 'Boat summon only if quest is active'),
(22, 2, 43443, 0, 0, 29, 0, 43450, 90, 0, 1, 0, 0, '', 'Boat summon only if no boat nearby'),
(22, 0, 43450, 0, 0, 32, 0, 16, 0, 0, 0, 0, 0, '', 'Boat start only when player boards'),
(22, 0, 112962, 0, 0, 9, 0, 26616, 0, 0, 0, 0, 0, '', 'Bunny summon only if quest is active'),
(22, 0, 112962, 0, 0, 29, 0, 43450, 90, 0, 1, 0, 0, '', 'Bunny summon only if no boat nearby'),
(18, 43450, 46598, 0, 0, 9, 0, 26616, 0, 0, 0, 0, 0, '', 'Spellclick only if quest is active (players)'),
(18, 43450, 46598, 0, 1, 32, 0, 16, 0, 0, 1, 0, 0, '', 'Spellclick allow non-player (vehicle accessories)');

-- Post-boat NPC positioning (fix wrong locations + missing Jorgensen)
UPDATE `creature` SET `position_x`=-9463.95, `position_y`=-2829.49, `position_z`=65.2781, `orientation`=1.8517 WHERE `id`=43458;
UPDATE `creature` SET `position_x`=-9465.49, `position_y`=-2826.15, `position_z`=65.2776, `orientation`=5.21006 WHERE `id`=43459;
UPDATE `creature` SET `position_x`=-9463.11, `position_y`=-2825.89, `position_z`=65.2775, `orientation`=4.13406 WHERE `id`=43461;
UPDATE `creature` SET `position_x`=-9466.39, `position_y`=-2829.02, `position_z`=65.2776, `orientation`=0.505524 WHERE `id`=43462;

-- Align dock clicker with boat spawn to avoid teleport
UPDATE `creature`
SET `position_x`=-9298.79, `position_y`=-2367.42, `position_z`=55.8253, `orientation`=3.6967
WHERE `id`=43443;

DELETE FROM `creature` WHERE `id`=43460;
SET @CGUID := (SELECT MAX(`guid`) + 1 FROM `creature`);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`,
`position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`,
`curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`)
VALUES
(@CGUID, 43460, 0, 44, 5326, 1, 1, 0, 0, 0, 0, -9462.02, -2827.84, 65.2761, 2.92062, 600, 0, 0, 0, 1930, 0, 0, 0, 0, 0, 0, 0, '', 0, 0);

-- Remove any static boat spawns; the boat is summoned by the bunny
DELETE FROM `creature` WHERE `id`=43450;

-- Bunny spawn to summon the riverboat
DELETE FROM `creature` WHERE `id`=112962;
SET @CGUID := (SELECT MAX(`guid`) + 1 FROM `creature`);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`,
`position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`,
`curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`, `VerifiedBuild`)
VALUES
(@CGUID, 112962, 0, 44, 68, 1, 1, 2000, 0, 0, 0, -9294.55, -2373.46, 56.6769, 1.29165, 120, 0, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, '', 0, 0);

-- Phase boat content so only players with quest 26616 can see it
DELETE FROM `phase_area` WHERE `AreaId`=44 AND `PhaseId`=2000;
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES
(44, 2000, 'Quest 26616 - Keeshan Riverboat phase');

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=2000 AND `SourceEntry`=44;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(26, 2000, 44, 0, 0, 9, 0, 26616, 0, 0, 0, 0, 0, '', 'Phase 2000 only while quest 26616 is active');

UPDATE `creature` SET `phaseId`=2000 WHERE `id` IN (43443, 112962);

-- Emucoach waypoints for Keeshan's Riverboat
DELETE FROM `waypoints` WHERE `entry`=43450;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES
(43450, 1, -9323.09, -2389.46, 56.0628, 'Keeshan Riverboat WP'),
(43450, 2, -9339.48, -2411.7, 56.0628, 'Keeshan Riverboat WP'),
(43450, 3, -9358.81, -2438.95, 56.0628, 'Keeshan Riverboat WP'),
(43450, 4, -9370.45, -2460.14, 56.0628, 'Keeshan Riverboat WP'),
(43450, 5, -9386.33, -2497.17, 56.0644, 'Keeshan Riverboat WP'),
(43450, 6, -9403.46, -2540.88, 56.0644, 'Keeshan Riverboat WP'),
(43450, 7, -9415.68, -2595.33, 56.0644, 'Keeshan Riverboat WP'),
(43450, 8, -9419.35, -2639.48, 56.0685, 'Keeshan Riverboat WP'),
(43450, 9, -9421.4, -2684.37, 56.0651, 'Keeshan Riverboat WP'),
(43450, 10, -9420.21, -2713.91, 56.0659, 'Keeshan Riverboat WP'),
(43450, 11, -9416.81, -2738.28, 56.0643, 'Keeshan Riverboat WP'),
(43450, 12, -9408.75, -2773.57, 56.0632, 'Keeshan Riverboat WP'),
(43450, 13, -9402.34, -2811.86, 56.0653, 'Keeshan Riverboat WP'),
(43450, 14, -9402.41, -2818.18, 56.0631, 'Keeshan Riverboat WP'),
(43450, 15, -9403.54, -2823.62, 56.0643, 'Keeshan Riverboat WP'),
(43450, 16, -9406.69, -2829.42, 56.0625, 'Keeshan Riverboat WP'),
(43450, 17, -9410.04, -2833.55, 56.0638, 'Keeshan Riverboat WP'),
(43450, 18, -9414.46, -2836.51, 56.0632, 'Keeshan Riverboat WP'),
(43450, 19, -9418.18, -2837.72, 56.0623, 'Keeshan Riverboat WP'),
(43450, 20, -9420.58, -2838.47, 56.0635, 'Keeshan Riverboat WP'),
(43450, 21, -9420.58, -2838.47, 56.0635, 'Keeshan Riverboat WP');
