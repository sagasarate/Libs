delimiter $$

CREATE TABLE `t_account_info` (
  `AccountID` bigint(20) NOT NULL,
  `AccountName` varchar(50) CHARACTER SET gbk NOT NULL,
  `Password` varchar(50) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `CreateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`AccountID`),
  UNIQUE KEY `AccountName_UNIQUE` (`AccountName`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1$$


CREATE TABLE `t_player_char_info` (
  `CharID` bigint(20) NOT NULL,
  `AccountID` bigint(20) NOT NULL,
  `CharName` varchar(50) CHARACTER SET gbk NOT NULL,
  `CreateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `IsDel` tinyint(4) NOT NULL DEFAULT '0',
  `Race` tinyint(4) NOT NULL,
  `Gender` tinyint(4) NOT NULL,
  `SkinColor` tinyint(4) NOT NULL,
  `HairColor` tinyint(4) NOT NULL,
  `FaceType` tinyint(4) NOT NULL,
  `HairType` tinyint(4) NOT NULL,
  `BeardType` tinyint(4) NOT NULL,
  `IsCharBald` tinyint(4) NOT NULL,
  `MapID` bigint(20) NOT NULL,
  `PosX` float NOT NULL,
  `PosY` float NOT NULL,
  `Height` float NOT NULL,
  `Direction` float NOT NULL,
  PRIMARY KEY (`CharID`),
  UNIQUE KEY `CharName_UNIQUE` (`CharName`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1$$

