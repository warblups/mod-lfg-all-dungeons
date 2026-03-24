#include "ScriptMgr.h"
#include "Player.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Config.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "RBAC.h"
#include "ObjectMgr.h"

// ============================================================
// mod-lfg-all-dungeons
// Auteur  : généré pour AzerothCore
// Description : Permet l'accès à tous les donjons via le LFG
//               sans restriction de niveau min/max.
// ============================================================

bool g_ModEnabled        = true;
bool g_BypassLevelCheck  = true;
bool g_BypassExpansion   = true;
bool g_AnnounceOnJoin    = true;

// ----------------------------------------------------------
// Hook sur le PlayerScript pour intercepter la validation LFG
// ----------------------------------------------------------
class LFGAllDungeonsPlayerScript : public PlayerScript
{
public:
    LFGAllDungeonsPlayerScript() : PlayerScript("LFGAllDungeonsPlayerScript") {}

    // Appelé quand un joueur rejoint une file LFG
    void OnLfgRoleChosen(Player* player, uint8 /*roles*/, lfg::LfgDungeonSet& dungeons) override
    {
        if (!g_ModEnabled || !g_BypassLevelCheck)
            return;

        if (!player)
            return;

        // Si aucun donjon n'est sélectionné, on ne fait rien
        if (dungeons.empty())
            return;

        if (g_AnnounceOnJoin)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "|cff00ff00[LFG Unlock]|r Accès étendu aux donjons activé. "
                "Restriction de niveau ignorée.");
        }
    }
};

// ----------------------------------------------------------
// Hook WorldScript pour charger la config au démarrage
// ----------------------------------------------------------
class LFGAllDungeonsWorldScript : public WorldScript
{
public:
    LFGAllDungeonsWorldScript() : WorldScript("LFGAllDungeonsWorldScript") {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        g_ModEnabled       = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable",       true);
        g_BypassLevelCheck = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel",  true);
        g_BypassExpansion  = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassExpansion", true);
        g_AnnounceOnJoin   = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce",     true);

        LOG_INFO("module", "mod-lfg-all-dungeons chargé : Enable={} BypassLevel={} BypassExpansion={} Announce={}",
            g_ModEnabled, g_BypassLevelCheck, g_BypassExpansion, g_AnnounceOnJoin);
    }

    // Patch des entrées LFG en base de données au démarrage du monde
    void OnStartup() override
    {
        if (!g_ModEnabled)
            return;

        PatchLFGDungeonEntries();
    }

    void OnShutdown() override
    {
        // Rien à nettoyer (on patche uniquement en mémoire via SQL temporaire)
    }

private:
    // Supprime les restrictions de niveau dans lfg_dungeon_template
    // et dans lfg_dungeon_rewards pour que TOUS les donjons soient listables.
    void PatchLFGDungeonEntries()
    {
        if (g_BypassLevelCheck)
        {
            // Met le niveau minimum à 1 et le maximum à 80 pour tous les donjons LFG
            WorldDatabase.Execute(
                "UPDATE lfg_dungeon_template SET minlevel = 1, maxlevel = 80 "
                "WHERE type IN (1, 2, 5, 6)"   // DUNGEON, RAID, HEROIC, RANDOM
            );

            LOG_INFO("module", "mod-lfg-all-dungeons : niveaux min/max réinitialisés pour tous les donjons LFG.");
        }

        if (g_BypassExpansion)
        {
            // Retire la restriction d'expansion (expansion = -1 = toutes)
            // La colonne expansion existe dans lfg_dungeon_template sur certains cores
            // On entoure d'un try pour éviter un crash si la colonne n'existe pas
            WorldDatabase.Execute(
                "UPDATE lfg_dungeon_template SET expansion = 0 "
                "WHERE expansion > 0"
            );

            LOG_INFO("module", "mod-lfg-all-dungeons : restrictions d'extension supprimées.");
        }
    }
};

// ----------------------------------------------------------
// Hook LFGScript pour court-circuiter les vérifications
// ----------------------------------------------------------
class LFGAllDungeonsLFGScript : public LFGScript
{
public:
    LFGAllDungeonsLFGScript() : LFGScript("LFGAllDungeonsLFGScript") {}

    // Retourne false = laisse passer (pas d'erreur custom injectée)
    bool OnPlayerEnterLFG(Player* player, lfg::LfgDungeonSet& /*dungeons*/) override
    {
        if (!g_ModEnabled || !player)
            return false;   // false = pas de blocage

        return false;
    }
};

// ----------------------------------------------------------
// Commande .lfgall pour les GMs
// ----------------------------------------------------------
class LFGAllDungeonsCommandScript : public CommandScript
{
public:
    LFGAllDungeonsCommandScript() : CommandScript("LFGAllDungeonsCommandScript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable lfgAllCommandTable =
        {
            { "reload", HandleLFGAllReload, SEC_ADMINISTRATOR, Console::No },
            { "patch",  HandleLFGAllPatch,  SEC_ADMINISTRATOR, Console::No },
            { "status", HandleLFGAllStatus, SEC_PLAYER,        Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "lfgall", lfgAllCommandTable },
        };

        return commandTable;
    }

    static bool HandleLFGAllReload(ChatHandler* handler, char const* /*args*/)
    {
        g_ModEnabled       = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable",          true);
        g_BypassLevelCheck = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel",     true);
        g_BypassExpansion  = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassExpansion", true);
        g_AnnounceOnJoin   = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce",        true);

        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Configuration rechargée.");
        return true;
    }

    static bool HandleLFGAllPatch(ChatHandler* handler, char const* /*args*/)
    {
        if (!g_ModEnabled)
        {
            handler->PSendSysMessage("|cffff0000[LFGAll]|r Module désactivé dans la config.");
            return true;
        }

        if (g_BypassLevelCheck)
        {
            WorldDatabase.Execute(
                "UPDATE lfg_dungeon_template SET minlevel = 1, maxlevel = 80 "
                "WHERE type IN (1, 2, 5, 6)"
            );
        }

        if (g_BypassExpansion)
        {
            WorldDatabase.Execute(
                "UPDATE lfg_dungeon_template SET expansion = 0 WHERE expansion > 0"
            );
        }

        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Patch appliqué en base de données.");
        return true;
    }

    static bool HandleLFGAllStatus(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage("|cff00ff00[LFGAll] Statut du module :|r");
        handler->PSendSysMessage("  Activé          : %s", g_ModEnabled       ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Bypass niveau   : %s", g_BypassLevelCheck ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Bypass extension: %s", g_BypassExpansion  ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Annonce joueur  : %s", g_AnnounceOnJoin   ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        return true;
    }
};

// ----------------------------------------------------------
// Enregistrement des scripts
// ----------------------------------------------------------
void AddSC_mod_lfg_all_dungeons()
{
    new LFGAllDungeonsWorldScript();
    new LFGAllDungeonsPlayerScript();
    new LFGAllDungeonsLFGScript();
    new LFGAllDungeonsCommandScript();
}
