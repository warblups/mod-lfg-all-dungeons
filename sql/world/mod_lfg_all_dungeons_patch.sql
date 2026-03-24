-- ============================================================
-- mod-lfg-all-dungeons : SQL d'initialisation
-- À appliquer sur la base world d'AzerothCore
-- ============================================================

-- Sauvegarde optionnelle de l'état original
-- (décommenter si vous souhaitez pouvoir revenir en arrière)
-- CREATE TABLE IF NOT EXISTS `mod_lfg_backup` LIKE `lfg_dungeon_template`;
-- INSERT INTO `mod_lfg_backup` SELECT * FROM `lfg_dungeon_template`;

-- Supprime les restrictions de niveau min/max pour TOUS les donjons
-- (Normal, Héroïque, Raid — difficulty 0 = normal, 1 = héroïque, etc.)
UPDATE `dungeon_access_template`
SET `min_level` = 1,
    `max_level` = 80;

-- Vérification rapide
SELECT `id`, `map_id`, `difficulty`, `min_level`, `max_level`, `min_avg_item_level`, `comment`
FROM `dungeon_access_template`
ORDER BY `map_id`, `difficulty`;
