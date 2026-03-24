-- ============================================================
-- mod-lfg-all-dungeons : SQL d'initialisation
-- À appliquer sur la base world d'AzerothCore
-- ============================================================

-- Sauvegarde optionnelle de l'état original
-- (décommenter si vous souhaitez pouvoir revenir en arrière)
-- CREATE TABLE IF NOT EXISTS `mod_lfg_backup` LIKE `lfg_dungeon_template`;
-- INSERT INTO `mod_lfg_backup` SELECT * FROM `lfg_dungeon_template`;

-- Supprime les restrictions de niveau pour tous les types de donjons :
--   type 1 = DUNGEON (donjon normal)
--   type 2 = RAID
--   type 5 = HEROIC
--   type 6 = RANDOM DUNGEON
UPDATE `lfg_dungeon_template`
SET `minlevel` = 1,
    `maxlevel` = 80
WHERE `type` IN (1, 2, 5, 6);

-- Supprime les restrictions d'extension
UPDATE `lfg_dungeon_template`
SET `expansion` = 0
WHERE `expansion` > 0;

-- Vérification rapide
SELECT `id`, `name`, `type`, `minlevel`, `maxlevel`, `expansion`
FROM `lfg_dungeon_template`
ORDER BY `type`, `id`;
