-- Dragon Soul: taxi vehicles for Nethestrasz/Eiendormi/Valeera (SmartAI + waypoints)

-- Ensure gossip NPCs use teleport script (summons taxi vehicles)
UPDATE `creature_template` SET `ScriptName`='npc_dragon_soul_teleport' WHERE `entry` IN (57287,57288,57289);

-- Taxi vehicle templates
DELETE FROM `creature_template` WHERE `entry` IN (572870,572880,572890);
INSERT INTO `creature_template` (
`entry`,`difficulty_entry_1`,`difficulty_entry_2`,`difficulty_entry_3`,`difficulty_entry_4`,`difficulty_entry_5`,`KillCredit1`,`KillCredit2`,
`name`,`femaleName`,`subname`,`IconName`,`gossip_menu_id`,`minlevel`,`maxlevel`,`exp`,`exp_unk`,`faction`,`npcflag`,`npcflag2`,
`speed_walk`,`speed_run`,`scale`,`rank`,`mindmg`,`maxdmg`,`dmgschool`,`attackpower`,`dmg_multiplier`,`BaseAttackTime`,`RangeAttackTime`,
`unit_class`,`unit_flags`,`unit_flags2`,`dynamicflags`,`family`,`trainer_type`,`trainer_class`,`trainer_race`,`minrangedmg`,`maxrangedmg`,
`rangedattackpower`,`type`,`type_flags`,`type_flags2`,`lootid`,`pickpocketloot`,`skinloot`,`resistance1`,`resistance2`,`resistance3`,
`resistance4`,`resistance5`,`resistance6`,`spell1`,`spell2`,`spell3`,`spell4`,`spell5`,`spell6`,`spell7`,`spell8`,`PetSpellDataId`,
`VehicleId`,`mingold`,`maxgold`,`AIName`,`MovementType`,`HoverHeight`,`Health_mod`,`Mana_mod`,`Mana_mod_extra`,`Armor_mod`,`RacialLeader`,
`questItem1`,`questItem2`,`questItem3`,`questItem4`,`questItem5`,`questItem6`,`movementId`,`RegenHealth`,`VignetteID`,`TrackingQuestID`,
`mechanic_immune_mask`,`flags_extra`,`ScriptName`,`VerifiedBuild`
) VALUES
(572870,0,0,0,0,0,0,0,'Nethestrasz','0','Defense of Wyrmrest Summit','',0,85,85,3,0,35,0,0,7,7,1,1,568,614,0,1024,1.7,2000,2000,2,768,0,0,0,0,0,0,568,614,1024,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1908,0,0,'SmartAI',0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,'',15595),
(572880,0,0,0,0,0,0,0,'Eiendormi','0','Assault on Yorsahj the Unsleeping','',0,85,85,3,0,35,0,0,7,7,1,1,568,614,0,1024,1.7,2000,2000,2,768,0,0,0,0,0,0,568,614,1024,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1908,0,0,'SmartAI',0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,'',15595),
(572890,0,0,0,0,0,0,0,'Valeera','0','Assault on Warlord Zon\'ozz','',0,85,85,3,0,35,0,0,7,7,1,1,568,614,0,1024,1.7,2000,2000,2,768,0,0,0,0,0,0,568,614,1024,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1908,0,0,'SmartAI',0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,'',15595);

-- Models (creature_template_model)
DELETE FROM `creature_template_model` WHERE `CreatureID` IN (572870,572880,572890);
INSERT INTO `creature_template_model` (`CreatureID`,`Idx`,`CreatureDisplayID`,`DisplayScale`,`Probability`,`VerifiedBuild`) VALUES
(572870,0,6371,1,1,15595),
(572880,0,8317,1,1,15595),
(572890,0,7975,1,1,15595);

-- Movement flags (flying taxi vehicles)
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (572870,572880,572890);
INSERT INTO `creature_template_movement` (`CreatureId`,`Ground`,`Swim`,`Flight`,`Rooted`,`Chase`,`Random`,`InteractionPauseTimer`) VALUES
(572870,0,0,2,0,0,0,0),
(572880,0,0,2,0,0,0,0),
(572890,0,0,2,0,0,0,0);

-- Addon data
DELETE FROM `creature_template_addon` WHERE `entry` IN (572870,572880,572890);
INSERT INTO `creature_template_addon`
(`entry`,`path_id`,`mount`,`MountCreatureID`,`StandState`,`AnimTier`,`VisFlags`,`SheathState`,`PvPFlags`,`emote`,`visibilityDistanceType`,`auras`) VALUES
(572870,0,0,0,1,0,0,1,0,0,0,''),
(572880,0,0,0,1,0,0,1,0,0,0,''),
(572890,0,0,0,1,0,0,1,0,0,0,'');

-- SmartAI: start waypoint on passenger boarded, despawn on last point
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid` IN (572870,572880,572890);
INSERT INTO `smart_scripts` (
`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,
`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,
`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,
`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_param4`,
`target_x`,`target_y`,`target_z`,`target_o`,`comment`
) VALUES
(572870,0,0,0,27,0,100,0,0,0,0,0,0,53,0,572870,0,0,0,0,1,0,0,0,0,0,0,0,0,'Nethestrasz Taxi - On Passenger Boarded - Start Waypoints'),
(572870,0,1,0,40,0,100,0,12,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Nethestrasz Taxi - On WP 12 - Despawn'),
(572880,0,0,0,27,0,100,0,0,0,0,0,0,53,0,572880,0,0,0,0,1,0,0,0,0,0,0,0,0,'Eiendormi Taxi - On Passenger Boarded - Start Waypoints'),
(572880,0,1,0,40,0,100,0,9,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Eiendormi Taxi - On WP 9 - Despawn'),
(572890,0,0,0,27,0,100,0,0,0,0,0,0,53,0,572890,0,0,0,0,1,0,0,0,0,0,0,0,0,'Valeera Taxi - On Passenger Boarded - Start Waypoints'),
(572890,0,1,0,40,0,100,0,8,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,'Valeera Taxi - On WP 8 - Despawn');

-- Waypoints
DELETE FROM `waypoints` WHERE `entry` IN (572870,572880,572890);
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(572870,1,-1674.35,-2376.32,85.59,'Nethestrasz'),
(572870,2,-1711.87,-2304.65,112.09,'Nethestrasz'),
(572870,3,-1786.7,-2274.95,139.52,'Nethestrasz'),
(572870,4,-1856.91,-2304.64,161.044,'Nethestrasz'),
(572870,5,-1891.34,-2386.03,198.18,'Nethestrasz'),
(572870,6,-1853.88,-2473.34,237.63,'Nethestrasz'),
(572870,7,-1771.75,-2495.19,265.1,'Nethestrasz'),
(572870,8,-1702.32,-2405.85,277.73,'Nethestrasz'),
(572870,9,-1724.18,-2329.03,280.64,'Nethestrasz'),
(572870,10,-1808.11,-2315.46,310.39,'Nethestrasz'),
(572870,11,-1830.48,-2365.05,349.71,'Nethestrasz'),
(572870,12,-1803.86,-2387.84,343.23,'Nethestrasz'),
(572880,1,-1778.39,-2474.84,65.688,'Eiendormi'),
(572880,2,-1773.89,-2579.8,100.94,'Eiendormi'),
(572880,3,-1774.5,-2786.2,149.89,'Eiendormi'),
(572880,4,-1771.32,-2951.63,142.13,'Eiendormi'),
(572880,5,-1770.31,-3022.11,121.46,'Eiendormi'),
(572880,6,-1772.48,-3039.14,20.42,'Eiendormi'),
(572880,7,-1819.44,-3056.98,-116.16,'Eiendormi'),
(572880,8,-1819.45,-3056.98,-116.16,'Eiendormi'),
(572880,9,-1867.33,-3077.01,-172.31,'Eiendormi'),
(572890,1,-1793.22,-2314.67,68.137,'Valeera'),
(572890,2,-1787.61,-2225.13,93.603,'Valeera'),
(572890,3,-1779.31,-2125.91,131.446,'Valeera'),
(572890,4,-1778.48,-2032.53,158.28,'Valeera'),
(572890,5,-1763.04,-1922.88,90.97,'Valeera'),
(572890,6,-1760.42,-1910.05,-64.93,'Valeera'),
(572890,7,-1752.06,-1878.63,-143.133,'Valeera'),
(572890,8,-1745.85,-1846.83,-217.03,'Valeera');
