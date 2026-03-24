#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "DatabaseEnv.h"

using namespace Acore::ChatCommands;

namespace LFGAll
{
    bool Enabled     = true;
    bool BypassLevel = true;
    bool Announce    = true;
}

// ============================================================================
//  SQL PATCH : Débloque tous les donjons pour tous les niveaux
// ============================================================================
static void ApplyPatch()
{
    // Table utilisée par l’UI du LFG
    WorldDatabase.Execute(
        "UPDATE lfg_dungeon_template SET minlevel = 1, maxlevel = 80 WHERE minlevel != 1 OR maxlevel != 80;"
    );

    // Table utilisée par le serveur retail-like AC
    WorldDatabase.Execute(
        "UPDATE dungeon_access_template SET min_level = 1, max_level = 80 WHERE min_level != 1 OR max_level != 80;"
    );

    LOG_INFO("module", "mod-lfg-all-dungeons : patch appliqué sur les tables LFG.");
}

// ============================================================================
//  WORLD SCRIPT
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
    }

    void OnStartup() override
    {
        if (LFGAll::Enabled && LFGAll::BypassLevel)
            ApplyPatch();
    }
};

// ============================================================================
//  PLAYER LOGIN MESSAGE
// ============================================================================
class LFGAllDungeonsPlayerScript : public PlayerScript
{
public:
    LFGAllDungeonsPlayerScript() : PlayerScript("LFGAllDungeonsPlayerScript") {}

    void OnPlayerLogin(Player* player) override
    {
        if (LFGAll::Enabled && LFGAll::Announce)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(
                "|cff00ff00[LFG Unlock]|r Tous les donjons sont maintenant accessibles dans le LFG."
            );
        }
    }
};

// ============================================================================
//  COMMANDES ADMIN : .lfgall reload / patch / status
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
        ApplyPatch();
        handler->PSendSysMessage("|cff00ff00[LFGAll]|r Patch appliqué.");
        return true;
    }

    static bool HandleStatus(ChatHandler* handler)
    {
        handler->PSendSysMessage("|cff00ff00[LFGAll] Statut :|r");
        handler->PSendSysMessage("  Activé        : %s", LFGAll::Enabled     ? "Oui" : "Non");
        handler->PSendSysMessage("  Bypass Niveau : %s", LFGAll::BypassLevel ? "Oui" : "Non");
        handler->PSendSysMessage("  Annonce Login : %s", LFGAll::Announce    ? "Oui" : "Non");

        return true;
    }
};

// ============================================================================
//  REGISTRATION
// ============================================================================
void Addmod_lfg_all_dungeonsScripts()
{
    new LFGAllDungeonsWorldScript();
    new LFGAllDungeonsPlayerScript();
    new LFGAllDungeonsCommandScript();
}
