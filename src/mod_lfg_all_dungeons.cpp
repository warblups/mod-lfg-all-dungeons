#include "ScriptMgr.h"
#include "Player.h"
#include "LFGMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "DatabaseEnv.h"

// ============================================================
// mod-lfg-all-dungeons
// Description : Permet l'accès à tous les donjons via le LFG
//               sans restriction de niveau min/max.
// ============================================================

using namespace Acore::ChatCommands;

bool g_ModEnabled       = true;
bool g_BypassLevelCheck = true;
bool g_AnnounceOnJoin   = true;

// ----------------------------------------------------------
// Patch BDD
// ----------------------------------------------------------
static void ApplyPatch()
{
    WorldDatabase.Execute(
        "UPDATE dungeon_access_template SET min_level = 1, max_level = 80"
    );
    LOG_INFO("module", "mod-lfg-all-dungeons : patch dungeon_access_template appliqué.");
}

// ----------------------------------------------------------
// WorldScript : chargement config + patch au démarrage
// ----------------------------------------------------------
class LFGAllDungeonsWorldScript : public WorldScript
{
public:
    LFGAllDungeonsWorldScript() : WorldScript("LFGAllDungeonsWorldScript") {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        g_ModEnabled       = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable",      true);
        g_BypassLevelCheck = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel", true);
        g_AnnounceOnJoin   = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce",    true);

        LOG_INFO("module", "mod-lfg-all-dungeons : Enable={} BypassLevel={} Announce={}",
            g_ModEnabled, g_BypassLevelCheck, g_AnnounceOnJoin);
    }

    void OnStartup() override
    {
        if (!g_ModEnabled || !g_BypassLevelCheck)
            return;

        ApplyPatch();
    }
};

// ----------------------------------------------------------
// PlayerScript : message à la connexion
// ----------------------------------------------------------
class LFGAllDungeonsPlayerScript : public PlayerScript
{
public:
    LFGAllDungeonsPlayerScript() : PlayerScript("LFGAllDungeonsPlayerScript") {}

    void OnPlayerLogin(Player* player) override
    {
        if (!g_ModEnabled || !g_AnnounceOnJoin || !player)
            return;

        ChatHandler(player->GetSession()).PSendSysMessage(
            "|cff00ff00[LFG Unlock]|r Tous les donjons sont accessibles via le LFG "
            "sans restriction de niveau.");
    }
};

// ----------------------------------------------------------
// CommandScript : .lfgall status / reload / patch
// ----------------------------------------------------------
class LFGAllDungeonsCommandScript : public CommandScript
{
public:
    LFGAllDungeonsCommandScript() : CommandScript("LFGAllDungeonsCommandScript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable lfgAllSub =
        {
            { "reload", HandleReload, SEC_ADMINISTRATOR, Console::No  },
            { "patch",  HandlePatch,  SEC_ADMINISTRATOR, Console::No  },
            { "status", HandleStatus, SEC_PLAYER,        Console::No  },
        };

        static ChatCommandTable root =
        {
            { "lfgall", lfgAllSub },
        };

        return root;
    }

    static bool HandleReload(ChatHandler* handler)
    {
        g_ModEnabled       = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable",      true);
        g_BypassLevelCheck = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel", true);
        g_AnnounceOnJoin   = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce",    true);

        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Configuration rechargée.");
        return true;
    }

    static bool HandlePatch(ChatHandler* handler)
    {
        if (!g_ModEnabled)
        {
            handler->PSendSysMessage("|cffff0000[LFGAll]|r Module désactivé dans la config.");
            return true;
        }

        ApplyPatch();
        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Patch appliqué sur dungeon_access_template.");
        return true;
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        handler->PSendSysMessage("|cff00ff00[LFGAll] Statut :|r");
        handler->PSendSysMessage("  Activé        : %s", g_ModEnabled       ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Bypass niveau : %s", g_BypassLevelCheck ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Annonce login : %s", g_AnnounceOnJoin   ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        return true;
    }
};

// ----------------------------------------------------------
// Enregistrement
// ----------------------------------------------------------
void Addmod_lfg_all_dungeonsScripts()
{
    new LFGAllDungeonsWorldScript();
    new LFGAllDungeonsPlayerScript();
    new LFGAllDungeonsCommandScript();
}
