# mod-lfg-all-dungeons

Module AzerothCore permettant l'accès à **tous les donjons via le système LFG (Looking For Group)** sans restriction de niveau minimum ou maximum.

---

## Fonctionnalités

- Suppression des restrictions de niveau min/max dans le LFG pour tous les types de donjons (Normal, Héroïque, Raid, Aléatoire)
- Suppression optionnelle des restrictions d'extension (BC, WotLK)
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
| `LFGAllDungeons.BypassLevel` | `1` | Ignore les restrictions de niveau |
| `LFGAllDungeons.BypassExpansion` | `1` | Ignore les restrictions d'extension |
| `LFGAllDungeons.Announce` | `1` | Message au joueur quand il rejoint la file |

---

## Commandes GM in-game

| Commande | Niveau requis | Description |
|---|---|---|
| `.lfgall status` | Joueur | Affiche l'état actuel du module |
| `.lfgall reload` | Administrateur | Recharge la configuration |
| `.lfgall patch` | Administrateur | Réapplique le patch SQL en BDD |

---

## Compatibilité

- AzerothCore 3.x (WotLK 3.3.5a)
- Compilateur : C++17 ou supérieur

---

## Restaurer les restrictions originales

Si vous souhaitez revenir aux paramètres Blizzard, restaurez la table `lfg_dungeon_template` depuis votre backup AzerothCore ou remettez manuellement les valeurs originales.

---

## Licence

MIT — libre d'utilisation et de modification.
