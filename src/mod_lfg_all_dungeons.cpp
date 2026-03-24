#include "ScriptMgr.h"
#include "Player.h"
#include "LFGMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "DatabaseEnv.h"

using namespace Acore::ChatCommands;

// ============================================================================
//  MOD CONFIG
// ============================================================================
namespace LFGAll
{
    bool Enabled     = true;
    bool BypassLevel = true;
    bool Announce    = true;
}

// ============================================================================
//  SQL PATCH
// ============================================================================
static void ApplyPatch()
{
    WorldDatabase.Execute(
        "UPDATE dungeon_access_template SET min_level = 1, max_level = 80 "
        "WHERE min_level != 1 OR max_level != 80"
    );

    LOG_INFO("module", "mod-lfg-all-dungeons : patch appliqué sur dungeon_access_template.");
}

// ============================================================================
//  WORLD SCRIPT : CHARGEMENT CONFIG + PATCH STARTUP
// ============================================================================
class LFGAllDungeonsWorldScript : public WorldScript
{
public:
    LFGAllDungeonsWorldScript() : WorldScript("LFGAllDungeonsWorldScript") {}

    void OnBeforeConfigLoad(bool) override
    {
        LFGAll::Enabled     = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable", true);
        LFGAll::BypassLevel = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel", true);
        LFGAll::Announce    = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce", true);

        LOG_INFO("module",
                 "mod-lfg-all-dungeons : Enable={} BypassLevel={} Announce={}",
                 LFGAll::Enabled, LFGAll::BypassLevel, LFGAll::Announce);
    }

    void OnStartup() override
    {
        if (LFGAll::Enabled && LFGAll::BypassLevel)
            ApplyPatch();
    }
};

// ============================================================================
//  PLAYER SCRIPT : MESSAGE LOGIN
// ============================================================================
class LFGAllDungeonsPlayerScript : public PlayerScript
{
public:
    LFGAllDungeonsPlayerScript() : PlayerScript("LFGAllDungeonsPlayerScript") {}

    void OnPlayerLogin(Player* player) override
    {
        if (LFGAll::Enabled && LFGAll::Announce && player)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "|cff00ff00[LFG Unlock]|r Tous les donjons sont accessibles via le LFG sans restriction de niveau."
            );
        }
    }
};

// ============================================================================
//  LFG SCRIPT OVERRIDE : BYPASS DU CHECK DE NIVEAU C++
// ============================================================================
class LFGAllOverride : public LFGScript
{
public:
    LFGAllOverride() : LFGScript("LFGAllOverride") {}

    // Hook permettant d'IGNORER les restrictions de niveau du LFG
    bool IsDungeonValid(uint32 dungeonId, uint8 level) override
    {
        // Si le module est activé → TOUS les donjons sont valides
        if (LFGAll::Enabled && LFGAll::BypassLevel)
            return true;

        // Sinon → false = laisser AzerothCore utiliser son système par défaut
        return false;
    }
};

// ============================================================================
//  COMMAND SCRIPT : .lfgall reload / patch / status
// ============================================================================
class LFGAllDungeonsCommandScript : public CommandScript
{
public:
    LFGAllDungeonsCommandScript() : CommandScript("LFGAllDungeonsCommandScript") {}

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable sub =
        {
            { "reload", HandleReload, SEC_ADMINISTRATOR, Console::No },
            { "patch",  HandlePatch,  SEC_ADMINISTRATOR, Console::No },
            { "status", HandleStatus, SEC_PLAYER,        Console::No }
        };

        static ChatCommandTable root =
        {
            { "lfgall", sub },
        };

        return root;
    }

    static bool HandleReload(ChatHandler* handler)
    {
        LFGAll::Enabled     = sConfigMgr->GetOption<bool>("LFGAllDungeons.Enable", true);
        LFGAll::BypassLevel = sConfigMgr->GetOption<bool>("LFGAllDungeons.BypassLevel", true);
        LFGAll::Announce    = sConfigMgr->GetOption<bool>("LFGAllDungeons.Announce", true);

        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Configuration rechargée.");
        return true;
    }

    static bool HandlePatch(ChatHandler* handler)
    {
        if (!LFGAll::Enabled)
        {
            handler->PSendSysMessage("|cffff0000[LFGAll]|r Module désactivé.");
            return true;
        }

        ApplyPatch();
        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Patch appliqué.");
        return true;
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        handler->PSendSysMessage("|cff00ff00[LFGAll] Statut :|r");
        handler->PSendSysMessage("  Activé        : %s", LFGAll::Enabled     ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Bypass niveau : %s", LFGAll::BypassLevel ? "|cff00ff00Oui|r" : "|cffff0000Non|r");
        handler->PSendSysMessage("  Annonce login : %s", LFGAll::Announce    ? "|cff00ff00Oui|r" : "|cffff0000Non|r");

        return true;
    }
};

// ============================================================================
//  ENREGISTREMENT
// ============================================================================
void Addmod_lfg_all_dungeonsScripts()
{
    new LFGAllDungeonsWorldScript();
    new LFGAllDungeonsPlayerScript();
    new LFGAllDungeonsCommandScript();
    new LFGAllOverride();   // ✅ Hook LFG ajouté
}
