-- Update security for commands that exist in both our DB and AzerothCore
UPDATE `command` SET `security` = CASE `name`
  WHEN 'account' THEN 0
  WHEN 'account 2fa' THEN 0
  WHEN 'account 2fa remove' THEN 0
  WHEN 'account 2fa setup' THEN 0
  WHEN 'account addon' THEN 1
  WHEN 'account create' THEN 4
  WHEN 'account delete' THEN 4
  WHEN 'account lock country' THEN 0
  WHEN 'account lock ip' THEN 0
  WHEN 'account onlinelist' THEN 4
  WHEN 'account password' THEN 0
  WHEN 'account set' THEN 2
  WHEN 'account set 2fa' THEN 0
  WHEN 'account set addon' THEN 2
  WHEN 'account set email' THEN 4
  WHEN 'account set gmlevel' THEN 4
  WHEN 'account set password' THEN 4
  WHEN 'achievement' THEN 2
  WHEN 'achievement add' THEN 2
  WHEN 'achievement checkall' THEN 3
  WHEN 'additem' THEN 2
  WHEN 'additem set' THEN 2
  WHEN 'announce' THEN 2
  WHEN 'appear' THEN 1
  WHEN 'arena captain' THEN 3
  WHEN 'arena create' THEN 3
  WHEN 'arena disband' THEN 3
  WHEN 'arena info' THEN 2
  WHEN 'arena lookup' THEN 2
  WHEN 'arena rename' THEN 3
  WHEN 'arena season deleteteams' THEN 3
  WHEN 'arena season reward' THEN 3
  WHEN 'arena season set state' THEN 3
  WHEN 'arena season start' THEN 3
  WHEN 'aura' THEN 2
  WHEN 'aura stack' THEN 2
  WHEN 'bags' THEN 2
  WHEN 'bags clear' THEN 2
  WHEN 'ban' THEN 2
  WHEN 'ban account' THEN 2
  WHEN 'ban character' THEN 2
  WHEN 'ban ip' THEN 2
  WHEN 'ban playeraccount' THEN 2
  WHEN 'baninfo' THEN 2
  WHEN 'baninfo account' THEN 2
  WHEN 'baninfo character' THEN 2
  WHEN 'baninfo ip' THEN 2
  WHEN 'banlist' THEN 2
  WHEN 'banlist account' THEN 2
  WHEN 'banlist character' THEN 2
  WHEN 'banlist ip' THEN 2
  WHEN 'bf' THEN 3
  WHEN 'bf enable' THEN 3
  WHEN 'bf start' THEN 3
  WHEN 'bf stop' THEN 3
  WHEN 'bf switch' THEN 3
  WHEN 'bf timer' THEN 3
  WHEN 'bindsight' THEN 3
  WHEN 'bm' THEN 3
  WHEN 'cache' THEN 1
  WHEN 'cache delete' THEN 3
  WHEN 'cache info' THEN 1
  WHEN 'cache refresh' THEN 1
  WHEN 'cast' THEN 2
  WHEN 'cast back' THEN 2
  WHEN 'cast dest' THEN 2
  WHEN 'cast dist' THEN 2
  WHEN 'cast self' THEN 2
  WHEN 'cast target' THEN 2
  WHEN 'character' THEN 2
  WHEN 'character changeaccount' THEN 3
  WHEN 'character changefaction' THEN 2
  WHEN 'character changerace' THEN 2
  WHEN 'character check bag' THEN 2
  WHEN 'character check bank' THEN 2
  WHEN 'character check profession' THEN 2
  WHEN 'character customize' THEN 2
  WHEN 'character deleted delete' THEN 4
  WHEN 'character deleted list' THEN 3
  WHEN 'character deleted purge' THEN 4
  WHEN 'character deleted restore' THEN 3
  WHEN 'character erase' THEN 4
  WHEN 'character level' THEN 2
  WHEN 'character rename' THEN 2
  WHEN 'character reputation' THEN 2
  WHEN 'character titles' THEN 2
  WHEN 'cheat' THEN 2
  WHEN 'cheat casttime' THEN 2
  WHEN 'cheat cooldown' THEN 2
  WHEN 'cheat explore' THEN 2
  WHEN 'cheat god' THEN 2
  WHEN 'cheat power' THEN 2
  WHEN 'cheat taxi' THEN 2
  WHEN 'cheat waterwalk' THEN 2
  WHEN 'combatstop' THEN 2
  WHEN 'cometome' THEN 3
  WHEN 'commands' THEN 0
  WHEN 'commentator' THEN 2
  WHEN 'cooldown' THEN 2
  WHEN 'damage' THEN 2
  WHEN 'debug' THEN 2
  WHEN 'debug Mod32Value' THEN 3
  WHEN 'debug anim' THEN 3
  WHEN 'debug areatriggers' THEN 3
  WHEN 'debug arena' THEN 3
  WHEN 'debug bg' THEN 3
  WHEN 'debug boundary' THEN 3
  WHEN 'debug cooldown' THEN 3
  WHEN 'debug dummy' THEN 3
  WHEN 'debug entervehicle' THEN 3
  WHEN 'debug getitemstate' THEN 3
  WHEN 'debug getitemvalue' THEN 3
  WHEN 'debug getvalue' THEN 3
  WHEN 'debug hostile' THEN 1
  WHEN 'debug itemexpire' THEN 3
  WHEN 'debug lfg' THEN 3
  WHEN 'debug lootrecipient' THEN 3
  WHEN 'debug los' THEN 3
  WHEN 'debug moveflags' THEN 3
  WHEN 'debug objectcount' THEN 3
  WHEN 'debug play' THEN 1
  WHEN 'debug play cinematic' THEN 1
  WHEN 'debug play movie' THEN 1
  WHEN 'debug play music' THEN 3
  WHEN 'debug play sound' THEN 1
  WHEN 'debug play visual' THEN 3
  WHEN 'debug send' THEN 3
  WHEN 'debug send buyerror' THEN 3
  WHEN 'debug send channelnotify' THEN 3
  WHEN 'debug send chatmessage' THEN 3
  WHEN 'debug send equiperror' THEN 3
  WHEN 'debug send largepacket' THEN 3
  WHEN 'debug send opcode' THEN 3
  WHEN 'debug send qinvalidmsg' THEN 3
  WHEN 'debug send qpartymsg' THEN 3
  WHEN 'debug send sellerror' THEN 3
  WHEN 'debug send setphaseshift' THEN 3
  WHEN 'debug send spellfail' THEN 3
  WHEN 'debug setaurastate' THEN 3
  WHEN 'debug setbit' THEN 3
  WHEN 'debug setitemvalue' THEN 3
  WHEN 'debug setvalue' THEN 3
  WHEN 'debug setvid' THEN 3
  WHEN 'debug spawnvehicle' THEN 3
  WHEN 'debug threat' THEN 3
  WHEN 'debug update' THEN 3
  WHEN 'debug uws' THEN 3
  WHEN 'debug zonestats' THEN 1
  WHEN 'deserter bg add' THEN 3
  WHEN 'deserter bg remove' THEN 3
  WHEN 'deserter bg remove all' THEN 3
  WHEN 'deserter instance add' THEN 3
  WHEN 'deserter instance remove' THEN 3
  WHEN 'deserter instance remove all' THEN 3
  WHEN 'dev' THEN 3
  WHEN 'die' THEN 2
  WHEN 'disable add battleground' THEN 3
  WHEN 'disable add map' THEN 3
  WHEN 'disable add outdoorpvp' THEN 3
  WHEN 'disable add quest' THEN 3
  WHEN 'disable add spell' THEN 3
  WHEN 'disable add vmap' THEN 3
  WHEN 'disable remove battleground' THEN 3
  WHEN 'disable remove map' THEN 3
  WHEN 'disable remove outdoorpvp' THEN 3
  WHEN 'disable remove quest' THEN 3
  WHEN 'disable remove spell' THEN 3
  WHEN 'disable remove vmap' THEN 3
  WHEN 'dismount' THEN 0
  WHEN 'distance' THEN 3
  WHEN 'event' THEN 2
  WHEN 'event activelist' THEN 2
  WHEN 'event start' THEN 2
  WHEN 'event stop' THEN 2
  WHEN 'flusharenapoints' THEN 3
  WHEN 'freeze' THEN 2
  WHEN 'gear repair' THEN 2
  WHEN 'gear stats' THEN 0
  WHEN 'gm' THEN 1
  WHEN 'gm chat' THEN 2
  WHEN 'gm fly' THEN 2
  WHEN 'gm ingame' THEN 0
  WHEN 'gm list' THEN 3
  WHEN 'gm visible' THEN 2
  WHEN 'gmannounce' THEN 2
  WHEN 'gmnameannounce' THEN 2
  WHEN 'gmnotify' THEN 2
  WHEN 'go' THEN 1
  WHEN 'go creature' THEN 1
  WHEN 'go creature id' THEN 1
  WHEN 'go creature name' THEN 1
  WHEN 'go gameobject' THEN 1
  WHEN 'go gameobject id' THEN 1
  WHEN 'go graveyard' THEN 1
  WHEN 'go grid' THEN 1
  WHEN 'go quest' THEN 1
  WHEN 'go taxinode' THEN 1
  WHEN 'go ticket' THEN 2
  WHEN 'go trigger' THEN 1
  WHEN 'go xyz' THEN 1
  WHEN 'go zonexy' THEN 1
  WHEN 'gobject' THEN 2
  WHEN 'gobject activate' THEN 2
  WHEN 'gobject add' THEN 3
  WHEN 'gobject add temp' THEN 2
  WHEN 'gobject delete' THEN 3
  WHEN 'gobject info' THEN 1
  WHEN 'gobject move' THEN 3
  WHEN 'gobject near' THEN 1
  WHEN 'gobject respawn' THEN 1
  WHEN 'gobject set' THEN 3
  WHEN 'gobject set phase' THEN 3
  WHEN 'gobject set state' THEN 3
  WHEN 'gobject target' THEN 1
  WHEN 'gobject turn' THEN 3
  WHEN 'gps' THEN 1
  WHEN 'group' THEN 2
  WHEN 'group disband' THEN 2
  WHEN 'group join' THEN 2
  WHEN 'group leader' THEN 2
  WHEN 'group list' THEN 2
  WHEN 'group remove' THEN 2
  WHEN 'group revive' THEN 2
  WHEN 'groupsummon' THEN 2
  WHEN 'guid' THEN 2
  WHEN 'guild' THEN 2
  WHEN 'guild create' THEN 2
  WHEN 'guild delete' THEN 2
  WHEN 'guild info' THEN 2
  WHEN 'guild invite' THEN 2
  WHEN 'guild rank' THEN 2
  WHEN 'guild rename' THEN 3
  WHEN 'guild uninvite' THEN 2
  WHEN 'help' THEN 0
  WHEN 'hidearea' THEN 3
  WHEN 'honor' THEN 2
  WHEN 'honor add' THEN 2
  WHEN 'honor add kill' THEN 2
  WHEN 'honor update' THEN 2
  WHEN 'instance' THEN 1
  WHEN 'instance getbossstate' THEN 1
  WHEN 'instance listbinds' THEN 1
  WHEN 'instance savedata' THEN 3
  WHEN 'instance setbossstate' THEN 2
  WHEN 'instance unbind' THEN 2
  WHEN 'inventory' THEN 1
  WHEN 'inventory count' THEN 1
  WHEN 'item move' THEN 2
  WHEN 'item refund' THEN 3
  WHEN 'item restore' THEN 2
  WHEN 'item restore list' THEN 2
  WHEN 'kick' THEN 2
  WHEN 'learn' THEN 2
  WHEN 'learn all' THEN 2
  WHEN 'learn all crafts' THEN 2
  WHEN 'learn all default' THEN 2
  WHEN 'learn all gm' THEN 2
  WHEN 'learn all lang' THEN 2
  WHEN 'learn all my' THEN 2
  WHEN 'learn all my class' THEN 2
  WHEN 'learn all my pettalents' THEN 2
  WHEN 'learn all my spells' THEN 2
  WHEN 'learn all my talents' THEN 2
  WHEN 'learn all recipes' THEN 2
  WHEN 'levelup' THEN 2
  WHEN 'lfg' THEN 1
  WHEN 'lfg clean' THEN 3
  WHEN 'lfg group' THEN 1
  WHEN 'lfg options' THEN 2
  WHEN 'lfg player' THEN 1
  WHEN 'lfg queue' THEN 1
  WHEN 'linkgrave' THEN 3
  WHEN 'list' THEN 1
  WHEN 'list auras' THEN 1
  WHEN 'list creature' THEN 1
  WHEN 'list item' THEN 1
  WHEN 'list object' THEN 1
  WHEN 'lookup' THEN 1
  WHEN 'lookup area' THEN 1
  WHEN 'lookup creature' THEN 1
  WHEN 'lookup event' THEN 1
  WHEN 'lookup faction' THEN 1
  WHEN 'lookup gobject' THEN 1
  WHEN 'lookup item' THEN 1
  WHEN 'lookup item set' THEN 1
  WHEN 'lookup map' THEN 1
  WHEN 'lookup object' THEN 1
  WHEN 'lookup player' THEN 2
  WHEN 'lookup player account' THEN 2
  WHEN 'lookup player email' THEN 2
  WHEN 'lookup player ip' THEN 2
  WHEN 'lookup quest' THEN 1
  WHEN 'lookup skill' THEN 1
  WHEN 'lookup spell' THEN 1
  WHEN 'lookup spell id' THEN 1
  WHEN 'lookup taxinode' THEN 1
  WHEN 'lookup teleport' THEN 1
  WHEN 'lookup title' THEN 1
  WHEN 'mailbox' THEN 1
  WHEN 'maxskill' THEN 2
  WHEN 'mmap' THEN 3
  WHEN 'mmap loadedtiles' THEN 3
  WHEN 'mmap loc' THEN 3
  WHEN 'mmap path' THEN 3
  WHEN 'mmap stats' THEN 3
  WHEN 'mmap testarea' THEN 3
  WHEN 'modify' THEN 2
  WHEN 'modify arenapoints' THEN 2
  WHEN 'modify bit' THEN 2
  WHEN 'modify drunk' THEN 2
  WHEN 'modify energy' THEN 2
  WHEN 'modify faction' THEN 3
  WHEN 'modify gender' THEN 2
  WHEN 'modify honor' THEN 2
  WHEN 'modify hp' THEN 2
  WHEN 'modify mana' THEN 2
  WHEN 'modify money' THEN 2
  WHEN 'modify mount' THEN 2
  WHEN 'modify phase' THEN 2
  WHEN 'modify rage' THEN 2
  WHEN 'modify reputation' THEN 2
  WHEN 'modify runicpower' THEN 2
  WHEN 'modify scale' THEN 2
  WHEN 'modify speed' THEN 2
  WHEN 'modify speed all' THEN 2
  WHEN 'modify speed backwalk' THEN 2
  WHEN 'modify speed fly' THEN 2
  WHEN 'modify speed swim' THEN 2
  WHEN 'modify speed walk' THEN 2
  WHEN 'modify spell' THEN 4
  WHEN 'modify standstate' THEN 2
  WHEN 'modify talentpoints' THEN 2
  WHEN 'morph' THEN 1
  WHEN 'morph mount' THEN 1
  WHEN 'morph reset' THEN 1
  WHEN 'morph target' THEN 1
  WHEN 'movegens' THEN 3
  WHEN 'mute' THEN 2
  WHEN 'mutehistory' THEN 2
  WHEN 'nameannounce' THEN 2
  WHEN 'neargrave' THEN 2
  WHEN 'notify' THEN 2
  WHEN 'npc' THEN 2
  WHEN 'npc add' THEN 3
  WHEN 'npc add formation' THEN 3
  WHEN 'npc add item' THEN 3
  WHEN 'npc add move' THEN 3
  WHEN 'npc add temp' THEN 3
  WHEN 'npc delete' THEN 3
  WHEN 'npc delete item' THEN 3
  WHEN 'npc follow' THEN 2
  WHEN 'npc follow stop' THEN 2
  WHEN 'npc guid' THEN 1
  WHEN 'npc info' THEN 1
  WHEN 'npc move' THEN 3
  WHEN 'npc near' THEN 2
  WHEN 'npc playemote' THEN 2
  WHEN 'npc say' THEN 2
  WHEN 'npc set' THEN 3
  WHEN 'npc set allowmove' THEN 3
  WHEN 'npc set data' THEN 3
  WHEN 'npc set entry' THEN 3
  WHEN 'npc set faction original' THEN 3
  WHEN 'npc set faction permanent' THEN 3
  WHEN 'npc set faction temp' THEN 3
  WHEN 'npc set flag' THEN 3
  WHEN 'npc set level' THEN 3
  WHEN 'npc set link' THEN 3
  WHEN 'npc set model' THEN 3
  WHEN 'npc set movetype' THEN 3
  WHEN 'npc set phase' THEN 3
  WHEN 'npc set spawntime' THEN 3
  WHEN 'npc set wanderdistance' THEN 3
  WHEN 'npc tame' THEN 2
  WHEN 'npc textemote' THEN 2
  WHEN 'npc whisper' THEN 2
  WHEN 'npc yell' THEN 2
  WHEN 'opendoor' THEN 1
  WHEN 'packetlog' THEN 2
  WHEN 'pdump' THEN 3
  WHEN 'pdump load' THEN 3
  WHEN 'pdump write' THEN 3
  WHEN 'pet' THEN 2
  WHEN 'pet create' THEN 2
  WHEN 'pet learn' THEN 2
  WHEN 'pet unlearn' THEN 2
  WHEN 'pinfo' THEN 2
  WHEN 'playall' THEN 2
  WHEN 'player learn' THEN 2
  WHEN 'player unlearn' THEN 2
  WHEN 'pooltools' THEN 3
  WHEN 'pooltools add' THEN 3
  WHEN 'pooltools clear' THEN 3
  WHEN 'pooltools def' THEN 3
  WHEN 'pooltools end' THEN 3
  WHEN 'pooltools remove' THEN 3
  WHEN 'pooltools start' THEN 3
  WHEN 'possess' THEN 2
  WHEN 'quest' THEN 2
  WHEN 'quest add' THEN 2
  WHEN 'quest complete' THEN 2
  WHEN 'quest remove' THEN 2
  WHEN 'quest reward' THEN 2
  WHEN 'quest status' THEN 2
  WHEN 'recall' THEN 2
  WHEN 'reload' THEN 3
  WHEN 'reload achievement_criteria_data' THEN 3
  WHEN 'reload achievement_reward' THEN 3
  WHEN 'reload achievement_reward_locale' THEN 3
  WHEN 'reload acore_string' THEN 3
  WHEN 'reload all' THEN 3
  WHEN 'reload all achievement' THEN 3
  WHEN 'reload all area' THEN 3
  WHEN 'reload all gossips' THEN 3
  WHEN 'reload all item' THEN 3
  WHEN 'reload all locales' THEN 3
  WHEN 'reload all loot' THEN 3
  WHEN 'reload all npc' THEN 3
  WHEN 'reload all quest' THEN 3
  WHEN 'reload all scripts' THEN 3
  WHEN 'reload all spell' THEN 3
  WHEN 'reload areatrigger_involvedrelation' THEN 3
  WHEN 'reload areatrigger_tavern' THEN 3
  WHEN 'reload areatrigger_teleport' THEN 3
  WHEN 'reload auctions' THEN 3
  WHEN 'reload autobroadcast' THEN 3
  WHEN 'reload battleground_template' THEN 3
  WHEN 'reload broadcast_text' THEN 3
  WHEN 'reload command' THEN 3
  WHEN 'reload conditions' THEN 3
  WHEN 'reload config' THEN 3
  WHEN 'reload creature_linked_respawn' THEN 3
  WHEN 'reload creature_loot_template' THEN 3
  WHEN 'reload creature_movement_override' THEN 3
  WHEN 'reload creature_onkill_reputation' THEN 3
  WHEN 'reload creature_questender' THEN 3
  WHEN 'reload creature_queststarter' THEN 3
  WHEN 'reload creature_template' THEN 3
  WHEN 'reload creature_template_locale' THEN 3
  WHEN 'reload creature_text' THEN 3
  WHEN 'reload creature_text_locale' THEN 3
  WHEN 'reload disables' THEN 3
  WHEN 'reload disenchant_loot_template' THEN 3
  WHEN 'reload dungeon_access_requirements' THEN 3
  WHEN 'reload dungeon_access_template' THEN 3
  WHEN 'reload event_scripts' THEN 3
  WHEN 'reload fishing_loot_template' THEN 3
  WHEN 'reload game_event_npc_vendor' THEN 3
  WHEN 'reload game_graveyard' THEN 3
  WHEN 'reload game_tele' THEN 3
  WHEN 'reload gameobject_loot_template' THEN 3
  WHEN 'reload gameobject_questender' THEN 3
  WHEN 'reload gameobject_queststarter' THEN 3
  WHEN 'reload gameobject_template_locale' THEN 3
  WHEN 'reload gm_tickets' THEN 3
  WHEN 'reload gossip_menu' THEN 3
  WHEN 'reload gossip_menu_option' THEN 3
  WHEN 'reload gossip_menu_option_locale' THEN 3
  WHEN 'reload graveyard_zone' THEN 3
  WHEN 'reload item_enchantment_template' THEN 3
  WHEN 'reload item_loot_template' THEN 3
  WHEN 'reload item_set_name_locale' THEN 3
  WHEN 'reload item_set_names' THEN 3
  WHEN 'reload item_template_locale' THEN 3
  WHEN 'reload lfg_dungeon_rewards' THEN 3
  WHEN 'reload mail_level_reward' THEN 3
  WHEN 'reload mail_loot_template' THEN 3
  WHEN 'reload mail_server_template' THEN 3
  WHEN 'reload milling_loot_template' THEN 3
  WHEN 'reload module_string' THEN 3
  WHEN 'reload motd' THEN 3
  WHEN 'reload npc_spellclick_spells' THEN 3
  WHEN 'reload npc_text_locale' THEN 3
  WHEN 'reload npc_vendor' THEN 3
  WHEN 'reload page_text' THEN 3
  WHEN 'reload page_text_locale' THEN 3
  WHEN 'reload pickpocketing_loot_template' THEN 3
  WHEN 'reload player_loot_template' THEN 3
  WHEN 'reload points_of_interest' THEN 3
  WHEN 'reload points_of_interest_locale' THEN 3
  WHEN 'reload prospecting_loot_template' THEN 3
  WHEN 'reload quest_greeting' THEN 3
  WHEN 'reload quest_poi' THEN 3
  WHEN 'reload quest_template' THEN 3
  WHEN 'reload quest_template_locale' THEN 3
  WHEN 'reload reference_loot_template' THEN 3
  WHEN 'reload reserved_name' THEN 3
  WHEN 'reload skill_discovery_template' THEN 3
  WHEN 'reload skill_extra_item_template' THEN 3
  WHEN 'reload skill_fishing_base_level' THEN 3
  WHEN 'reload skinning_loot_template' THEN 3
  WHEN 'reload smart_scripts' THEN 3
  WHEN 'reload spell_area' THEN 3
  WHEN 'reload spell_bonus_data' THEN 3
  WHEN 'reload spell_group' THEN 3
  WHEN 'reload spell_group_stack_rules' THEN 3
  WHEN 'reload spell_linked_spell' THEN 3
  WHEN 'reload spell_loot_template' THEN 3
  WHEN 'reload spell_pet_auras' THEN 3
  WHEN 'reload spell_proc' THEN 3
  WHEN 'reload spell_proc_event' THEN 3
  WHEN 'reload spell_required' THEN 3
  WHEN 'reload spell_scripts' THEN 3
  WHEN 'reload spell_target_position' THEN 3
  WHEN 'reload spell_threats' THEN 3
  WHEN 'reload trainer' THEN 3
  WHEN 'reload vehicle_accessory' THEN 3
  WHEN 'reload vehicle_template_accessory' THEN 3
  WHEN 'reload waypoint_data' THEN 3
  WHEN 'reload waypoint_scripts' THEN 3
  WHEN 'reset' THEN 3
  WHEN 'reset achievements' THEN 4
  WHEN 'reset all' THEN 4
  WHEN 'reset honor' THEN 3
  WHEN 'reset items' THEN 3
  WHEN 'reset items all' THEN 3
  WHEN 'reset items allbags' THEN 3
  WHEN 'reset items bags' THEN 3
  WHEN 'reset items bank' THEN 3
  WHEN 'reset items currency' THEN 3
  WHEN 'reset items equipped' THEN 3
  WHEN 'reset items keyring' THEN 3
  WHEN 'reset items vendor_buyback' THEN 3
  WHEN 'reset level' THEN 3
  WHEN 'reset spells' THEN 3
  WHEN 'reset stats' THEN 3
  WHEN 'reset talents' THEN 3
  WHEN 'respawn' THEN 2
  WHEN 'respawn all' THEN 2
  WHEN 'revive' THEN 2
  WHEN 'save' THEN 0
  WHEN 'saveall' THEN 2
  WHEN 'send' THEN 2
  WHEN 'send items' THEN 2
  WHEN 'send mail' THEN 2
  WHEN 'send message' THEN 3
  WHEN 'send money' THEN 2
  WHEN 'server' THEN 3
  WHEN 'server corpses' THEN 2
  WHEN 'server debug' THEN 3
  WHEN 'server exit' THEN 4
  WHEN 'server idlerestart' THEN 4
  WHEN 'server idlerestart cancel' THEN 3
  WHEN 'server idleshutdown' THEN 4
  WHEN 'server idleshutdown cancel' THEN 3
  WHEN 'server info' THEN 0
  WHEN 'server motd' THEN 0
  WHEN 'server restart' THEN 3
  WHEN 'server restart cancel' THEN 3
  WHEN 'server set closed' THEN 4
  WHEN 'server set loglevel' THEN 4
  WHEN 'server set motd' THEN 3
  WHEN 'server shutdown' THEN 3
  WHEN 'server shutdown cancel' THEN 3
  WHEN 'setskill' THEN 2
  WHEN 'settings' THEN 1
  WHEN 'settings announcer' THEN 1
  WHEN 'showarea' THEN 2
  WHEN 'skirmish' THEN 3
  WHEN 'spect' THEN 0
  WHEN 'spect leave' THEN 0
  WHEN 'spect reset' THEN 0
  WHEN 'spect spectate' THEN 0
  WHEN 'spect version' THEN 0
  WHEN 'spect watch' THEN 0
  WHEN 'string' THEN 2
  WHEN 'summon' THEN 2
  WHEN 'teleport' THEN 1
  WHEN 'teleport add' THEN 3
  WHEN 'teleport del' THEN 3
  WHEN 'teleport group' THEN 2
  WHEN 'teleport name' THEN 2
  WHEN 'teleport name npc guid' THEN 2
  WHEN 'teleport name npc id' THEN 2
  WHEN 'teleport name npc name' THEN 2
  WHEN 'ticket' THEN 2
  WHEN 'ticket assign' THEN 2
  WHEN 'ticket close' THEN 2
  WHEN 'ticket closedlist' THEN 2
  WHEN 'ticket comment' THEN 2
  WHEN 'ticket complete' THEN 2
  WHEN 'ticket delete' THEN 3
  WHEN 'ticket escalate' THEN 2
  WHEN 'ticket escalatedlist' THEN 2
  WHEN 'ticket list' THEN 2
  WHEN 'ticket onlinelist' THEN 2
  WHEN 'ticket reset' THEN 4
  WHEN 'ticket response' THEN 2
  WHEN 'ticket response append' THEN 2
  WHEN 'ticket response appendln' THEN 2
  WHEN 'ticket response delete' THEN 2
  WHEN 'ticket response show' THEN 2
  WHEN 'ticket togglesystem' THEN 3
  WHEN 'ticket unassign' THEN 2
  WHEN 'ticket viewid' THEN 2
  WHEN 'ticket viewname' THEN 2
  WHEN 'titles' THEN 2
  WHEN 'titles add' THEN 2
  WHEN 'titles current' THEN 2
  WHEN 'titles remove' THEN 2
  WHEN 'titles set' THEN 2
  WHEN 'titles set mask' THEN 2
  WHEN 'unaura' THEN 2
  WHEN 'unban' THEN 3
  WHEN 'unban account' THEN 3
  WHEN 'unban character' THEN 3
  WHEN 'unban ip' THEN 3
  WHEN 'unban playeraccount' THEN 3
  WHEN 'unbindsight' THEN 3
  WHEN 'unfreeze' THEN 2
  WHEN 'unlearn' THEN 2
  WHEN 'unmute' THEN 2
  WHEN 'unpossess' THEN 2
  WHEN 'unstuck' THEN 2
  WHEN 'wchange' THEN 3
  WHEN 'whispers' THEN 1
  WHEN 'worldstate scourgeinvasion battleswon' THEN 3
  WHEN 'worldstate scourgeinvasion show' THEN 3
  WHEN 'worldstate scourgeinvasion startzone' THEN 3
  WHEN 'worldstate scourgeinvasion state' THEN 3
  WHEN 'worldstate sunsreach gate' THEN 3
  WHEN 'worldstate sunsreach phase' THEN 3
  WHEN 'worldstate sunsreach subphase' THEN 3
  WHEN 'wp' THEN 3
  WHEN 'wp add' THEN 3
  WHEN 'wp event' THEN 3
  WHEN 'wp load' THEN 3
  WHEN 'wp modify' THEN 3
  WHEN 'wp reload' THEN 3
  WHEN 'wp show' THEN 3
  WHEN 'wp unload' THEN 3
  WHEN 'wpgps' THEN 3
  ELSE `security` END
WHERE `name` IN (
  'account',
  'account 2fa',
  'account 2fa remove',
  'account 2fa setup',
  'account addon',
  'account create',
  'account delete',
  'account lock country',
  'account lock ip',
  'account onlinelist',
  'account password',
  'account set',
  'account set 2fa',
  'account set addon',
  'account set email',
  'account set gmlevel',
  'account set password',
  'achievement',
  'achievement add',
  'achievement checkall',
  'additem',
  'additem set',
  'announce',
  'appear',
  'arena captain',
  'arena create',
  'arena disband',
  'arena info',
  'arena lookup',
  'arena rename',
  'arena season deleteteams',
  'arena season reward',
  'arena season set state',
  'arena season start',
  'aura',
  'aura stack',
  'bags',
  'bags clear',
  'ban',
  'ban account',
  'ban character',
  'ban ip',
  'ban playeraccount',
  'baninfo',
  'baninfo account',
  'baninfo character',
  'baninfo ip',
  'banlist',
  'banlist account',
  'banlist character',
  'banlist ip',
  'bf',
  'bf enable',
  'bf start',
  'bf stop',
  'bf switch',
  'bf timer',
  'bindsight',
  'bm',
  'cache',
  'cache delete',
  'cache info',
  'cache refresh',
  'cast',
  'cast back',
  'cast dest',
  'cast dist',
  'cast self',
  'cast target',
  'character',
  'character changeaccount',
  'character changefaction',
  'character changerace',
  'character check bag',
  'character check bank',
  'character check profession',
  'character customize',
  'character deleted delete',
  'character deleted list',
  'character deleted purge',
  'character deleted restore',
  'character erase',
  'character level',
  'character rename',
  'character reputation',
  'character titles',
  'cheat',
  'cheat casttime',
  'cheat cooldown',
  'cheat explore',
  'cheat god',
  'cheat power',
  'cheat taxi',
  'cheat waterwalk',
  'combatstop',
  'cometome',
  'commands',
  'commentator',
  'cooldown',
  'damage',
  'debug',
  'debug Mod32Value',
  'debug anim',
  'debug areatriggers',
  'debug arena',
  'debug bg',
  'debug boundary',
  'debug cooldown',
  'debug dummy',
  'debug entervehicle',
  'debug getitemstate',
  'debug getitemvalue',
  'debug getvalue',
  'debug hostile',
  'debug itemexpire',
  'debug lfg',
  'debug lootrecipient',
  'debug los',
  'debug moveflags',
  'debug objectcount',
  'debug play',
  'debug play cinematic',
  'debug play movie',
  'debug play music',
  'debug play sound',
  'debug play visual',
  'debug send',
  'debug send buyerror',
  'debug send channelnotify',
  'debug send chatmessage',
  'debug send equiperror',
  'debug send largepacket',
  'debug send opcode',
  'debug send qinvalidmsg',
  'debug send qpartymsg',
  'debug send sellerror',
  'debug send setphaseshift',
  'debug send spellfail',
  'debug setaurastate',
  'debug setbit',
  'debug setitemvalue',
  'debug setvalue',
  'debug setvid',
  'debug spawnvehicle',
  'debug threat',
  'debug update',
  'debug uws',
  'debug zonestats',
  'deserter bg add',
  'deserter bg remove',
  'deserter bg remove all',
  'deserter instance add',
  'deserter instance remove',
  'deserter instance remove all',
  'dev',
  'die',
  'disable add battleground',
  'disable add map',
  'disable add outdoorpvp',
  'disable add quest',
  'disable add spell',
  'disable add vmap',
  'disable remove battleground',
  'disable remove map',
  'disable remove outdoorpvp',
  'disable remove quest',
  'disable remove spell',
  'disable remove vmap',
  'dismount',
  'distance',
  'event',
  'event activelist',
  'event start',
  'event stop',
  'flusharenapoints',
  'freeze',
  'gear repair',
  'gear stats',
  'gm',
  'gm chat',
  'gm fly',
  'gm ingame',
  'gm list',
  'gm visible',
  'gmannounce',
  'gmnameannounce',
  'gmnotify',
  'go',
  'go creature',
  'go creature id',
  'go creature name',
  'go gameobject',
  'go gameobject id',
  'go graveyard',
  'go grid',
  'go quest',
  'go taxinode',
  'go ticket',
  'go trigger',
  'go xyz',
  'go zonexy',
  'gobject',
  'gobject activate',
  'gobject add',
  'gobject add temp',
  'gobject delete',
  'gobject info',
  'gobject move',
  'gobject near',
  'gobject respawn',
  'gobject set',
  'gobject set phase',
  'gobject set state',
  'gobject target',
  'gobject turn',
  'gps',
  'group',
  'group disband',
  'group join',
  'group leader',
  'group list',
  'group remove',
  'group revive',
  'groupsummon',
  'guid',
  'guild',
  'guild create',
  'guild delete',
  'guild info',
  'guild invite',
  'guild rank',
  'guild rename',
  'guild uninvite',
  'help',
  'hidearea',
  'honor',
  'honor add',
  'honor add kill',
  'honor update',
  'instance',
  'instance getbossstate',
  'instance listbinds',
  'instance savedata',
  'instance setbossstate',
  'instance unbind',
  'inventory',
  'inventory count',
  'item move',
  'item refund',
  'item restore',
  'item restore list',
  'kick',
  'learn',
  'learn all',
  'learn all crafts',
  'learn all default',
  'learn all gm',
  'learn all lang',
  'learn all my',
  'learn all my class',
  'learn all my pettalents',
  'learn all my spells',
  'learn all my talents',
  'learn all recipes',
  'levelup',
  'lfg',
  'lfg clean',
  'lfg group',
  'lfg options',
  'lfg player',
  'lfg queue',
  'linkgrave',
  'list',
  'list auras',
  'list creature',
  'list item',
  'list object',
  'lookup',
  'lookup area',
  'lookup creature',
  'lookup event',
  'lookup faction',
  'lookup gobject',
  'lookup item',
  'lookup item set',
  'lookup map',
  'lookup object',
  'lookup player',
  'lookup player account',
  'lookup player email',
  'lookup player ip',
  'lookup quest',
  'lookup skill',
  'lookup spell',
  'lookup spell id',
  'lookup taxinode',
  'lookup teleport',
  'lookup title',
  'mailbox',
  'maxskill',
  'mmap',
  'mmap loadedtiles',
  'mmap loc',
  'mmap path',
  'mmap stats',
  'mmap testarea',
  'modify',
  'modify arenapoints',
  'modify bit',
  'modify drunk',
  'modify energy',
  'modify faction',
  'modify gender',
  'modify honor',
  'modify hp',
  'modify mana',
  'modify money',
  'modify mount',
  'modify phase',
  'modify rage',
  'modify reputation',
  'modify runicpower',
  'modify scale',
  'modify speed',
  'modify speed all',
  'modify speed backwalk',
  'modify speed fly',
  'modify speed swim',
  'modify speed walk',
  'modify spell',
  'modify standstate',
  'modify talentpoints',
  'morph',
  'morph mount',
  'morph reset',
  'morph target',
  'movegens',
  'mute',
  'mutehistory',
  'nameannounce',
  'neargrave',
  'notify',
  'npc',
  'npc add',
  'npc add formation',
  'npc add item',
  'npc add move',
  'npc add temp',
  'npc delete',
  'npc delete item',
  'npc follow',
  'npc follow stop',
  'npc guid',
  'npc info',
  'npc move',
  'npc near',
  'npc playemote',
  'npc say',
  'npc set',
  'npc set allowmove',
  'npc set data',
  'npc set entry',
  'npc set faction original',
  'npc set faction permanent',
  'npc set faction temp',
  'npc set flag',
  'npc set level',
  'npc set link',
  'npc set model',
  'npc set movetype',
  'npc set phase',
  'npc set spawntime',
  'npc set wanderdistance',
  'npc tame',
  'npc textemote',
  'npc whisper',
  'npc yell',
  'opendoor',
  'packetlog',
  'pdump',
  'pdump load',
  'pdump write',
  'pet',
  'pet create',
  'pet learn',
  'pet unlearn',
  'pinfo',
  'playall',
  'player learn',
  'player unlearn',
  'pooltools',
  'pooltools add',
  'pooltools clear',
  'pooltools def',
  'pooltools end',
  'pooltools remove',
  'pooltools start',
  'possess',
  'quest',
  'quest add',
  'quest complete',
  'quest remove',
  'quest reward',
  'quest status',
  'recall',
  'reload',
  'reload achievement_criteria_data',
  'reload achievement_reward',
  'reload achievement_reward_locale',
  'reload acore_string',
  'reload all',
  'reload all achievement',
  'reload all area',
  'reload all gossips',
  'reload all item',
  'reload all locales',
  'reload all loot',
  'reload all npc',
  'reload all quest',
  'reload all scripts',
  'reload all spell',
  'reload areatrigger_involvedrelation',
  'reload areatrigger_tavern',
  'reload areatrigger_teleport',
  'reload auctions',
  'reload autobroadcast',
  'reload battleground_template',
  'reload broadcast_text',
  'reload command',
  'reload conditions',
  'reload config',
  'reload creature_linked_respawn',
  'reload creature_loot_template',
  'reload creature_movement_override',
  'reload creature_onkill_reputation',
  'reload creature_questender',
  'reload creature_queststarter',
  'reload creature_template',
  'reload creature_template_locale',
  'reload creature_text',
  'reload creature_text_locale',
  'reload disables',
  'reload disenchant_loot_template',
  'reload dungeon_access_requirements',
  'reload dungeon_access_template',
  'reload event_scripts',
  'reload fishing_loot_template',
  'reload game_event_npc_vendor',
  'reload game_graveyard',
  'reload game_tele',
  'reload gameobject_loot_template',
  'reload gameobject_questender',
  'reload gameobject_queststarter',
  'reload gameobject_template_locale',
  'reload gm_tickets',
  'reload gossip_menu',
  'reload gossip_menu_option',
  'reload gossip_menu_option_locale',
  'reload graveyard_zone',
  'reload item_enchantment_template',
  'reload item_loot_template',
  'reload item_set_name_locale',
  'reload item_set_names',
  'reload item_template_locale',
  'reload lfg_dungeon_rewards',
  'reload mail_level_reward',
  'reload mail_loot_template',
  'reload mail_server_template',
  'reload milling_loot_template',
  'reload module_string',
  'reload motd',
  'reload npc_spellclick_spells',
  'reload npc_text_locale',
  'reload npc_vendor',
  'reload page_text',
  'reload page_text_locale',
  'reload pickpocketing_loot_template',
  'reload player_loot_template',
  'reload points_of_interest',
  'reload points_of_interest_locale',
  'reload prospecting_loot_template',
  'reload quest_greeting',
  'reload quest_poi',
  'reload quest_template',
  'reload quest_template_locale',
  'reload reference_loot_template',
  'reload reserved_name',
  'reload skill_discovery_template',
  'reload skill_extra_item_template',
  'reload skill_fishing_base_level',
  'reload skinning_loot_template',
  'reload smart_scripts',
  'reload spell_area',
  'reload spell_bonus_data',
  'reload spell_group',
  'reload spell_group_stack_rules',
  'reload spell_linked_spell',
  'reload spell_loot_template',
  'reload spell_pet_auras',
  'reload spell_proc',
  'reload spell_proc_event',
  'reload spell_required',
  'reload spell_scripts',
  'reload spell_target_position',
  'reload spell_threats',
  'reload trainer',
  'reload vehicle_accessory',
  'reload vehicle_template_accessory',
  'reload waypoint_data',
  'reload waypoint_scripts',
  'reset',
  'reset achievements',
  'reset all',
  'reset honor',
  'reset items',
  'reset items all',
  'reset items allbags',
  'reset items bags',
  'reset items bank',
  'reset items currency',
  'reset items equipped',
  'reset items keyring',
  'reset items vendor_buyback',
  'reset level',
  'reset spells',
  'reset stats',
  'reset talents',
  'respawn',
  'respawn all',
  'revive',
  'save',
  'saveall',
  'send',
  'send items',
  'send mail',
  'send message',
  'send money',
  'server',
  'server corpses',
  'server debug',
  'server exit',
  'server idlerestart',
  'server idlerestart cancel',
  'server idleshutdown',
  'server idleshutdown cancel',
  'server info',
  'server motd',
  'server restart',
  'server restart cancel',
  'server set closed',
  'server set loglevel',
  'server set motd',
  'server shutdown',
  'server shutdown cancel',
  'setskill',
  'settings',
  'settings announcer',
  'showarea',
  'skirmish',
  'spect',
  'spect leave',
  'spect reset',
  'spect spectate',
  'spect version',
  'spect watch',
  'string',
  'summon',
  'teleport',
  'teleport add',
  'teleport del',
  'teleport group',
  'teleport name',
  'teleport name npc guid',
  'teleport name npc id',
  'teleport name npc name',
  'ticket',
  'ticket assign',
  'ticket close',
  'ticket closedlist',
  'ticket comment',
  'ticket complete',
  'ticket delete',
  'ticket escalate',
  'ticket escalatedlist',
  'ticket list',
  'ticket onlinelist',
  'ticket reset',
  'ticket response',
  'ticket response append',
  'ticket response appendln',
  'ticket response delete',
  'ticket response show',
  'ticket togglesystem',
  'ticket unassign',
  'ticket viewid',
  'ticket viewname',
  'titles',
  'titles add',
  'titles current',
  'titles remove',
  'titles set',
  'titles set mask',
  'unaura',
  'unban',
  'unban account',
  'unban character',
  'unban ip',
  'unban playeraccount',
  'unbindsight',
  'unfreeze',
  'unlearn',
  'unmute',
  'unpossess',
  'unstuck',
  'wchange',
  'whispers',
  'worldstate scourgeinvasion battleswon',
  'worldstate scourgeinvasion show',
  'worldstate scourgeinvasion startzone',
  'worldstate scourgeinvasion state',
  'worldstate sunsreach gate',
  'worldstate sunsreach phase',
  'worldstate sunsreach subphase',
  'wp',
  'wp add',
  'wp event',
  'wp load',
  'wp modify',
  'wp reload',
  'wp show',
  'wp unload',
  'wpgps'
);

-- End of update
