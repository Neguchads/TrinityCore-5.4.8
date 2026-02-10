SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account`  (
  `id` int unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL DEFAULT '',
  `battlenet_account` varchar(32) NOT NULL DEFAULT '',
  `sha_pass_hash` varchar(40) NOT NULL DEFAULT '',
  `sessionkey` varchar(80) NOT NULL DEFAULT '',
  `v` varchar(64) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `email` varchar(255) NOT NULL DEFAULT '',
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int unsigned NOT NULL DEFAULT '0',
  `locked` tinyint unsigned NOT NULL DEFAULT '0',
  `lock_country` varchar(2) NOT NULL DEFAULT '00',
  `last_login` timestamp NULL DEFAULT NULL,
  `online` tinyint unsigned NOT NULL DEFAULT '0',
  `expansion` tinyint unsigned NOT NULL DEFAULT '4',
  `mutetime` bigint NOT NULL DEFAULT '0',
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint unsigned NOT NULL DEFAULT '0',
  `os` varchar(4) NOT NULL DEFAULT '',
  `recruiter` int unsigned NOT NULL DEFAULT '0',
  `project_member_id` int unsigned NOT NULL DEFAULT '0',
  `rank` int DEFAULT NULL,
  `vp` int DEFAULT NULL,
  `dp` int NOT NULL DEFAULT '0',
  `isactive` varchar(50) DEFAULT NULL,
  `activation` varchar(256) DEFAULT NULL,
  `rewarded` int NOT NULL DEFAULT '0',
  `flags` int NOT NULL DEFAULT '0',
  `gmlevel` tinyint unsigned NOT NULL DEFAULT '0',
  `online_mute_timer` bigint unsigned NOT NULL DEFAULT '0',
  `active_mute_id` int unsigned NOT NULL DEFAULT '0',
  `cash` int NOT NULL DEFAULT '0',
  `project_hwid` varchar(40) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `idx_username`(`username` ASC) USING BTREE,
  INDEX `idx_id`(`id` ASC) USING BTREE,
  INDEX `idx_sha`(`sha_pass_hash` ASC) USING BTREE,
  INDEX `idx_session`(`sessionkey` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account System' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account
-- ----------------------------

-- ----------------------------
-- Table structure for account_access
-- ----------------------------
DROP TABLE IF EXISTS `account_access`;
CREATE TABLE `account_access`  (
  `id` int UNSIGNED NOT NULL,
  `gmlevel` tinyint UNSIGNED NOT NULL,
  `RealmID` int NOT NULL DEFAULT -1,
  `name` varchar(255) NULL DEFAULT NULL,
  PRIMARY KEY (`id`, `RealmID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_access
-- ----------------------------

-- ----------------------------
-- Table structure for account_banned
-- ----------------------------
DROP TABLE IF EXISTS `account_banned`;
CREATE TABLE `account_banned`  (
  `id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Account id',
  `realm` int NOT NULL,
  `bandate` int UNSIGNED NOT NULL DEFAULT 0,
  `unbandate` int UNSIGNED NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Ban List' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_banned
-- ----------------------------

-- ----------------------------
-- Table structure for account_boost
-- ----------------------------
DROP TABLE IF EXISTS `account_boost`;
CREATE TABLE `account_boost`  (
  `id` int NOT NULL DEFAULT 0,
  `realmid` int UNSIGNED NOT NULL DEFAULT 1,
  `counter` int UNSIGNED NOT NULL DEFAULT 0
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_boost
-- ----------------------------

-- ----------------------------
-- Table structure for account_muted
-- ----------------------------
DROP TABLE IF EXISTS `account_muted`;
CREATE TABLE `account_muted`  (
  `id` int NOT NULL,
  `realmid` int NOT NULL DEFAULT 0,
  `acc_id` int NOT NULL,
  `char_id` int NOT NULL,
  `mute_acc` varchar(32) NOT NULL,
  `mute_name` varchar(50) NOT NULL,
  `mute_date` bigint NOT NULL,
  `muted_by` varchar(50) NOT NULL,
  `mute_time` bigint NOT NULL,
  `mute_reason` varchar(255) NOT NULL,
  `public_channels_only` tinyint NOT NULL,
  PRIMARY KEY (`realmid`, `id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_muted
-- ----------------------------

-- ----------------------------
-- Table structure for arena_games
-- ----------------------------
DROP TABLE IF EXISTS `arena_games`;
CREATE TABLE `arena_games`  (
  `gameid` bigint NOT NULL DEFAULT 0,
  `teamid` bigint NOT NULL DEFAULT 0,
  `guid` bigint NOT NULL DEFAULT 0,
  `changeType` int NOT NULL,
  `ratingChange` int NOT NULL,
  `teamRating` int NOT NULL,
  `matchMakerRating` smallint UNSIGNED NULL DEFAULT NULL,
  `damageDone` int NOT NULL,
  `deaths` int NOT NULL,
  `healingDone` int NOT NULL,
  `damageTaken` int NOT NULL,
  `healingTaken` int NOT NULL,
  `killingBlows` int NOT NULL,
  `damageAbsorbed` int UNSIGNED NOT NULL,
  `timeControlled` int UNSIGNED NOT NULL,
  `aurasDispelled` int UNSIGNED NOT NULL,
  `aurasStolen` int UNSIGNED NOT NULL,
  `highLatencyTimes` int UNSIGNED NOT NULL,
  `spellsPrecast` int UNSIGNED NOT NULL,
  `mapId` int NOT NULL,
  `start` int NOT NULL,
  `end` int NOT NULL,
  `class` tinyint UNSIGNED NULL DEFAULT NULL,
  `season` smallint UNSIGNED NULL DEFAULT NULL,
  `type` tinyint UNSIGNED NULL DEFAULT NULL,
  `realmid` tinyint UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`gameid`, `teamid`, `guid`) USING BTREE,
  INDEX `idx__teamid`(`teamid` ASC) USING BTREE,
  INDEX `idx__season__class__type`(`season` ASC, `class` ASC, `type` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'WoWArmory Game Chart' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_games
-- ----------------------------

-- ----------------------------
-- Table structure for autobroadcast
-- ----------------------------
DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast`  (
  `realmid` int NOT NULL DEFAULT -1,
  `id` tinyint UNSIGNED NOT NULL AUTO_INCREMENT,
  `weight` tinyint UNSIGNED NULL DEFAULT 1,
  `text` longtext NOT NULL,
  PRIMARY KEY (`id`, `realmid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of autobroadcast
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_account_bans
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_account_bans`;
CREATE TABLE `battlenet_account_bans`  (
  `id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Account id',
  `bandate` int UNSIGNED NOT NULL DEFAULT 0,
  `unbandate` int UNSIGNED NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  PRIMARY KEY (`id`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Ban List' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlenet_account_bans
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_accounts
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_accounts`;
CREATE TABLE `battlenet_accounts`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `email` varchar(320) NOT NULL,
  `sha_pass_hash` varchar(64) NOT NULL DEFAULT '',
  `v` varchar(256) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `sessionKey` varchar(128) NOT NULL DEFAULT '',
  `locked` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `last_login` timestamp NULL DEFAULT NULL,
  `online` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int UNSIGNED NOT NULL DEFAULT 0,
  `project_member_id` int UNSIGNED NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account System' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlenet_accounts
-- ----------------------------

-- ----------------------------
-- Table structure for battlepay_log
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_log`;
CREATE TABLE `battlepay_log`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `accountId` int UNSIGNED NOT NULL,
  `characterGuid` int NOT NULL DEFAULT 0,
  `realm` int UNSIGNED NOT NULL,
  `item` int NOT NULL DEFAULT 0,
  `price` int UNSIGNED NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlepay_log
-- ----------------------------

-- ----------------------------
-- Table structure for bonus_rates
-- ----------------------------
DROP TABLE IF EXISTS `bonus_rates`;
CREATE TABLE `bonus_rates`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Autoincrementable ID',
  `realmid` int NOT NULL DEFAULT -1 COMMENT 'RealmID for which the rates would be active. -1 for all realms',
  `active` tinyint UNSIGNED NOT NULL DEFAULT 1 COMMENT 'If set to 0 - this bonus would not be loaded',
  `schedule` varchar(50) NOT NULL DEFAULT '* * * * *' COMMENT 'Cron-style schedule defining the time for the bonus rates period. Multiple periods can be specified with a semicolon separated list',
  `multiplier` float NOT NULL DEFAULT 2 COMMENT 'Rate multiplier (i.e. 2 would change the rates to be twice their usual value during the bonus rates period)',
  `rates` text NOT NULL COMMENT 'Space separated list of rate names as used in config (i.e. \"Rate.XP.Kill Rate.Honor\")',
  `start_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all online players when the bonus rate period starts',
  `end_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all online players when the bonus rate period ends',
  `active_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all players logging in whenever the bonus rate period is active',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of bonus_rates
-- ----------------------------

-- ----------------------------
-- Table structure for ip_banned
-- ----------------------------
DROP TABLE IF EXISTS `ip_banned`;
CREATE TABLE `ip_banned`  (
  `ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `bandate` int UNSIGNED NOT NULL,
  `unbandate` int UNSIGNED NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(255) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Banned IPs' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of ip_banned
-- ----------------------------

-- ----------------------------
-- Table structure for logs
-- ----------------------------
DROP TABLE IF EXISTS `logs`;
CREATE TABLE `logs`  (
  `time` int UNSIGNED NOT NULL,
  `realm` int UNSIGNED NOT NULL,
  `type` tinyint UNSIGNED NOT NULL,
  `level` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `string` text NULL
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of logs
-- ----------------------------

-- ----------------------------
-- Table structure for mute_active
-- ----------------------------
DROP TABLE IF EXISTS `mute_active`;
CREATE TABLE `mute_active`  (
  `realmid` tinyint NOT NULL,
  `account` int NOT NULL,
  `mute_id` int NOT NULL,
  `mute_timer` int NOT NULL,
  PRIMARY KEY (`realmid`, `account`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of mute_active
-- ----------------------------

-- ----------------------------
-- Table structure for promotion_auras
-- ----------------------------
DROP TABLE IF EXISTS `promotion_auras`;
CREATE TABLE `promotion_auras`  (
  `entry` int UNSIGNED NOT NULL,
  `start_date` int UNSIGNED NOT NULL,
  `lenght` int UNSIGNED NOT NULL COMMENT 'Lenght in minutes',
  `active` tinyint UNSIGNED NOT NULL,
  `comment` text NOT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of promotion_auras
-- ----------------------------

-- ----------------------------
-- Table structure for realm_diff_stats
-- ----------------------------
DROP TABLE IF EXISTS `realm_diff_stats`;
CREATE TABLE `realm_diff_stats`  (
  `realm_id` tinyint UNSIGNED NOT NULL,
  `diff` mediumint UNSIGNED NULL DEFAULT NULL,
  `min` mediumint UNSIGNED NULL DEFAULT NULL,
  `max` mediumint UNSIGNED NULL DEFAULT NULL,
  `unixtime` int NOT NULL,
  PRIMARY KEY (`realm_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realm_diff_stats
-- ----------------------------
INSERT INTO `realm_diff_stats` VALUES (1, 25, 14, 129, 1636139221);

-- ----------------------------
-- Table structure for realmcharacters
-- ----------------------------
DROP TABLE IF EXISTS `realmcharacters`;
CREATE TABLE `realmcharacters`  (
  `realmid` int UNSIGNED NOT NULL DEFAULT 0,
  `acctid` int UNSIGNED NOT NULL,
  `numchars` tinyint UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`realmid`, `acctid`) USING BTREE,
  INDEX `acctid`(`acctid` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'How many characters accounts have' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realmcharacters
-- ----------------------------
INSERT INTO `realmcharacters` VALUES (1, 1, 4);
INSERT INTO `realmcharacters` VALUES (1, 2, 1);
INSERT INTO `realmcharacters` VALUES (2, 1, 0);
INSERT INTO `realmcharacters` VALUES (2, 2, 0);

-- ----------------------------
-- Table structure for realmlist
-- ----------------------------
DROP TABLE IF EXISTS `realmlist`;
CREATE TABLE `realmlist` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `port` smallint NOT NULL DEFAULT '8085',
  `localAddress` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `localSubnetMask` varchar(255) NOT NULL DEFAULT '255.255.255.0',
  `icon` tinyint unsigned NOT NULL DEFAULT '0',
  `flag` tinyint NOT NULL,
  `timezone` tinyint unsigned NOT NULL DEFAULT '0',
  `allowedSecurityLevel` tinyint unsigned NOT NULL DEFAULT '0',
  `population` float unsigned NOT NULL DEFAULT '0',
  `gamebuild` int unsigned NOT NULL DEFAULT '12340',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT = 1 DEFAULT CHARSET=utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT=DYNAMIC COMMENT='Realm System';

-- ----------------------------
-- Records of realmlist
-- ----------------------------
INSERT INTO `realmlist` (`id`, `name`, `address`, `port`, `localAddress`, `localSubnetMask`, `icon`, `flag`, `timezone`, `allowedSecurityLevel`, `population`, `gamebuild`) VALUES
(1, 'Trinity', '127.0.0.1', 8085, '127.0.0.1', '255.255.255.0', 0, 0, 14, 0, 0, 18414);

-- ----------------------------
-- Table structure for build_info
-- ----------------------------
DROP TABLE IF EXISTS `build_info`;
CREATE TABLE `build_info`  (
  `build` int NOT NULL,
  `majorVersion` int NULL DEFAULT NULL,
  `minorVersion` int NULL DEFAULT NULL,
  `bugfixVersion` int NULL DEFAULT NULL,
  `hotfixVersion` varchar(3) NULL DEFAULT NULL,
  `winAuthSeed` varchar(32) NULL DEFAULT NULL,
  `win64AuthSeed` varchar(32) NULL DEFAULT NULL,
  `mac64AuthSeed` varchar(32) NULL DEFAULT NULL,
  `winChecksumSeed` varchar(40) NULL DEFAULT NULL,
  `macChecksumSeed` varchar(40) NULL DEFAULT NULL,
  PRIMARY KEY (`build`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for build_info
-- ----------------------------
INSERT INTO `build_info` VALUES
(5875, 1, 12, 1, NULL, NULL, NULL, NULL, '95EDB27C7823B363CBDDAB56A392E7CB73FCCA20', '8D173CC381961EEBABF336F5E6675B101BB513E5'),
(6005, 1, 12, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(6141, 1, 12, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(8606, 2, 4, 3, NULL, NULL, NULL, NULL, '319AFAA3F2559682F9FF658BE01456255F456FB1', 'D8B0ECFE534BC1131E19BAD1D4C0E813EEE4994F'),
(9947, 3, 1, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(10505, 3, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(11159, 3, 3, 0, 'a', NULL, NULL, NULL, NULL, NULL),
(11403, 3, 3, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(11723, 3, 3, 3, 'a', NULL, NULL, NULL, NULL, NULL),
(12340, 3, 3, 5, 'a', NULL, NULL, NULL, 'CDCBBD5188315E6B4D19449D492DBCFAF156A347', 'B706D13FF2F4018839729461E3F8A0E2B5FDC034'),
(13623, 4, 0, 6, 'a', NULL, NULL, NULL, NULL, NULL),
(13930, 3, 3, 5, 'a', NULL, NULL, NULL, NULL, NULL),
(14545, 4, 2, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(15595, 4, 3, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(18414, 5, 4, 8, NULL, NULL, NULL, NULL, NULL, NULL),
(19116, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19243, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19342, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19702, 6, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(19802, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(19831, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(19865, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(20182, 6, 2, 0, 'a', NULL, NULL, NULL, NULL, NULL),
(20201, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20216, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20253, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20338, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20444, 6, 2, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(20490, 6, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(20574, 6, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(20726, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(20779, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(20886, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(21355, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(21463, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(21742, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(22248, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22293, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22345, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22410, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22423, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22498, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22522, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22566, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22594, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22624, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22747, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22810, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22900, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22908, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22950, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22995, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22996, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23171, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23222, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23360, 7, 1, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(23420, 7, 1, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(23911, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23937, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(24015, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(24330, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24367, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24415, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24430, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24461, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24742, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(25549, 7, 3, 2, NULL, 'FE594FC35E7F9AFF86D99D8A364AB297', '1252624ED8CBD6FAC7D33F5D67A535F3', '66FC5E09B8706126795F140308C8C1D8', NULL, NULL),
(25996, 7, 3, 5, NULL, '23C59C5963CBEF5B728D13A50878DFCB', 'C7FF932D6A2174A3D538CA7212136D2B', '210B970149D6F56CAC9BADF2AAC91E8E', NULL, NULL),
(26124, 7, 3, 5, NULL, 'F8C05AE372DECA1D6C81DA7A8D1C5C39', '46DF06D0147BA67BA49AF553435E093F', 'C9CA997AB8EDE1C65465CB2920869C4E', NULL, NULL),
(26365, 7, 3, 5, NULL, '2AAC82C80E829E2CA902D70CFA1A833A', '59A53F307288454B419B13E694DF503C', 'DBE7F860276D6B400AAA86B35D51A417', NULL, NULL),
(26654, 7, 3, 5, NULL, 'FAC2D693E702B9EC9F750F17245696D8', 'A752640E8B99FE5B57C1320BC492895A', '9234C1BD5E9687ADBD19F764F2E0E811', NULL, NULL),
(26822, 7, 3, 5, NULL, '283E8D77ECF7060BE6347BE4EB99C7C7', '2B05F6D746C0C6CC7EF79450B309E595', '91003668C245D14ECD8DF094E065E06B', NULL, NULL),
(26899, 7, 3, 5, NULL, 'F462CD2FE4EA3EADF875308FDBB18C99', '3551EF0028B51E92170559BD25644B03', '8368EFC2021329110A16339D298200D4', NULL, NULL),
(26972, 7, 3, 5, NULL, '797ECC19662DCBD5090A4481173F1D26', '6E212DEF6A0124A3D9AD07F5E322F7AE', '341CFEFE3D72ACA9A4407DC535DED66A', NULL, NULL),
(28153, 8, 0, 1, NULL, NULL, 'DD626517CC6D31932B479934CCDC0ABF', NULL, NULL, NULL),
(30706, 8, 1, 5, NULL, NULL, 'BB6D9866FE4A19A568015198783003FC', NULL, NULL, NULL),
(30993, 8, 2, 0, NULL, NULL, '2BAD61655ABC2FC3D04893B536403A91', NULL, NULL, NULL),
(31229, 8, 2, 0, NULL, NULL, '8A46F23670309F2AAE85C9A47276382B', NULL, NULL, NULL),
(31429, 8, 2, 0, NULL, NULL, '7795A507AF9DC3525EFF724FEE17E70C', NULL, NULL, NULL),
(31478, 8, 2, 0, NULL, NULL, '7973A8D54BDB8B798D9297B096E771EF', NULL, NULL, NULL),
(32305, 8, 2, 5, NULL, NULL, '21F5A6FC7AD89FBF411FDA8B8738186A', NULL, NULL, NULL),
(32494, 8, 2, 5, NULL, NULL, '58984ACE04919401835C61309A848F8A', NULL, NULL, NULL),
(32580, 8, 2, 5, NULL, NULL, '87C2FAA0D7931BF016299025C0DDCA14', NULL, NULL, NULL),
(32638, 8, 2, 5, NULL, NULL, '5D07ECE7D4A867DDDE615DAD22B76D4E', NULL, NULL, NULL),
(32722, 8, 2, 5, NULL, NULL, '1A09BE1D38A122586B4931BECCEAD4AA', NULL, NULL, NULL);

-- ----------------------------
-- Table structure for uptime
-- ----------------------------
DROP TABLE IF EXISTS `uptime`;
CREATE TABLE `uptime`  (
  `realmid` int UNSIGNED NOT NULL,
  `starttime` int UNSIGNED NOT NULL DEFAULT 0,
  `uptime` int UNSIGNED NOT NULL DEFAULT 0,
  `maxplayers` smallint UNSIGNED NOT NULL DEFAULT 0,
  `revision` varchar(255) NOT NULL DEFAULT 'SkyFire',
  PRIMARY KEY (`realmid`, `starttime`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Uptime system' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for wow_token
-- ----------------------------
DROP TABLE IF EXISTS `wow_token`;
CREATE TABLE `wow_token`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `accountId` int UNSIGNED NOT NULL,
  `characterGuid` int NOT NULL DEFAULT 0,
  `realm` int UNSIGNED NOT NULL,
  `coins` int UNSIGNED NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of wow_token
-- ----------------------------

-- ----------------------------
-- Table structure for updates
-- ----------------------------
DROP TABLE IF EXISTS `updates`;
CREATE TABLE IF NOT EXISTS `updates` (
  `name` varchar(200) NOT NULL COMMENT 'filename with extension of the update.',
  `hash` char(40) DEFAULT '' COMMENT 'sha1 hash of the sql file.',
  `state` enum('RELEASED','CUSTOM','MODULE','ARCHIVED','PENDING') NOT NULL DEFAULT 'RELEASED' COMMENT 'defines if an update is released or archived.',
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'timestamp when the query was applied.',
  `speed` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'time the query takes to apply in ms.',
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='List of all applied updates in this database.';

-- ----------------------------
-- Records of updates
-- ----------------------------

-- ----------------------------
-- Table structure for updates_include
-- ----------------------------
DROP TABLE IF EXISTS `updates_include`;
CREATE TABLE IF NOT EXISTS `updates_include` (
  `path` varchar(200) NOT NULL COMMENT 'directory to include. $ means relative to the source directory.',
  `state` enum('RELEASED','ARCHIVED','CUSTOM','PENDING') NOT NULL DEFAULT 'RELEASED' COMMENT 'defines if the directory contains released or archived updates.',
  PRIMARY KEY (`path`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='List of directories where we want to include sql updates.';

-- ----------------------------
-- Records of updates_include
-- ----------------------------
INSERT INTO `updates_include` (`path`, `state`) VALUES
('$/sql/updates/auth', 'RELEASED');

SET FOREIGN_KEY_CHECKS = 1;
