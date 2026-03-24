[![Logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png)](https://azerothcore.org/)

# mod-lfg-all-dungeons

Latest build status with azerothcore:

[![Build Status](https://github.com/YOUR_USERNAME/mod-lfg-all-dungeons/actions/workflows/core-build.yml/badge.svg)](https://github.com/YOUR_USERNAME/mod-lfg-all-dungeons/actions/workflows/core-build.yml)

Module AzerothCore permettant l'accès à **tous les donjons via le système LFG (Looking For Group)** sans restriction de niveau minimum ou maximum.

> 🇬🇧 [English version below](#english)

---

## Fonctionnalités

- Suppression des restrictions de niveau min/max dans le LFG pour tous les types de donjons (Normal, Héroïque, Raid, Aléatoire)
- Message d'information au joueur quand il rejoint la file
- Commandes GM pour recharger la config ou forcer le patch
- Configuration complète via `.conf`

---

## Installation

### 1. Copier le module

```bash
cp -r mod-lfg-all-dungeons /chemin/vers/azerothcore/modules/
```

### 2. Recompiler AzerothCore

```bash
cd /chemin/vers/azerothcore/build
cmake .. -DMODULES_FOLDER=../modules
make -j$(nproc)
```

### 3. Appliquer le SQL

```bash
mysql -u root -p acore_world < sql/world/mod_lfg_all_dungeons_patch.sql
```

> Le SQL cible la table **`dungeon_access_template`** (121 entrées) qui contrôle les colonnes `min_level` / `max_level` pour chaque donjon et chaque difficulté.

### 4. Copier la configuration

```bash
cp conf/mod_lfg_all_dungeons.conf.dist \
   /chemin/vers/worldserver/configs/mod_lfg_all_dungeons.conf
```

### 5. Redémarrer le worldserver

---

## Configuration (`mod_lfg_all_dungeons.conf`)

| Option | Défaut | Description |
|---|---|---|
| `LFGAllDungeons.Enable` | `1` | Active/désactive le module |
| `LFGAllDungeons.BypassLevel` | `1` | Ignore les restrictions de niveau dans `dungeon_access_template` |
| `LFGAllDungeons.Announce` | `1` | Message au joueur quand il rejoint la file |

---

## Commandes GM in-game

| Commande | Niveau requis | Description |
|---|---|---|
| `.lfgall status` | Joueur | Affiche l'état actuel du module |
| `.lfgall reload` | Administrateur | Recharge la configuration |
| `.lfgall patch` | Administrateur | Réapplique le patch sur `dungeon_access_template` |

---

## Compatibilité

- AzerothCore 3.x (WotLK 3.3.5a)
- Compilateur : C++17 ou supérieur

---

## Restaurer les restrictions originales

Pour revenir aux valeurs Blizzard, restaurez la table `dungeon_access_template` depuis votre backup AzerothCore ou remettez manuellement les `min_level` / `max_level` d'origine.

---

## Licence

MIT — libre d'utilisation et de modification.

---
---

<a name="english"></a>

[![Logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png)](https://azerothcore.org/)

# mod-lfg-all-dungeons

Latest build status with azerothcore:

[![Build Status](https://github.com/YOUR_USERNAME/mod-lfg-all-dungeons/actions/workflows/core-build.yml/badge.svg)](https://github.com/YOUR_USERNAME/mod-lfg-all-dungeons/actions/workflows/core-build.yml)

AzerothCore module that allows access to **all dungeons through the LFG (Looking For Group) system** regardless of the player's level.

---

## Features

- Removes min/max level restrictions in LFG for all dungeon types (Normal, Heroic, Raid, Random)
- In-game notification to the player when joining the queue
- GM commands to reload the config or force re-apply the patch
- Fully configurable via `.conf` file

---

## Installation

### 1. Copy the module

```bash
cp -r mod-lfg-all-dungeons /path/to/azerothcore/modules/
```

### 2. Recompile AzerothCore

```bash
cd /path/to/azerothcore/build
cmake .. -DMODULES_FOLDER=../modules
make -j$(nproc)
```

### 3. Apply the SQL patch

```bash
mysql -u root -p acore_world < sql/world/mod_lfg_all_dungeons_patch.sql
```

> The SQL targets the **`dungeon_access_template`** table (121 rows), which controls the `min_level` / `max_level` columns for each dungeon and difficulty.

### 4. Copy the configuration file

```bash
cp conf/mod_lfg_all_dungeons.conf.dist \
   /path/to/worldserver/configs/mod_lfg_all_dungeons.conf
```

### 5. Restart the worldserver

---

## Configuration (`mod_lfg_all_dungeons.conf`)

| Option | Default | Description |
|---|---|---|
| `LFGAllDungeons.Enable` | `1` | Enable or disable the module |
| `LFGAllDungeons.BypassLevel` | `1` | Bypass level restrictions in `dungeon_access_template` |
| `LFGAllDungeons.Announce` | `1` | Notify the player when joining the LFG queue |

---

## In-game GM Commands

| Command | Required level | Description |
|---|---|---|
| `.lfgall status` | Player | Display the current module status |
| `.lfgall reload` | Administrator | Reload the configuration |
| `.lfgall patch` | Administrator | Re-apply the SQL patch on `dungeon_access_template` |

---

## Compatibility

- AzerothCore 3.x (WotLK 3.3.5a)
- Compiler: C++17 or higher

---

## Restoring original restrictions

To revert to Blizzard's original values, restore the `dungeon_access_template` table from your AzerothCore backup, or manually reset the `min_level` / `max_level` values.

---

## License

MIT — free to use and modify.
