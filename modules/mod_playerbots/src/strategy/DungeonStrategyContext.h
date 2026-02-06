/*
* This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _PLAYERBOT_DUNGEONSTRATEGYCONTEXT_H
#define _PLAYERBOT_DUNGEONSTRATEGYCONTEXT_H

#include "NamedObjectContext.h"
#include "Strategy.h"

#include "UtgardeKeepStrategy.h"
#include "NexusStrategy.h"
#include "AzjolNerubStrategy.h"
#include "OldKingdomStrategy.h"
#include "DrakTharonKeepStrategy.h"
#include "VioletHoldStrategy.h"
#include "GundrakStrategy.h"
#include "HallsOfStoneStrategy.h"
#include "HallsOfLightningStrategy.h"
#include "OculusStrategy.h"
#include "UtgardePinnacleStrategy.h"
#include "CullingOfStratholmeStrategy.h"
#include "ForgeOfSoulsStrategy.h"
#include "PitOfSaronStrategy.h"
#include "TrialOfTheChampionStrategy.h"
#include "DungeonStrategy.h"

class DungeonStrategyContext : public NamedObjectContext<Strategy>
{
public:
    DungeonStrategyContext() : NamedObjectContext<Strategy>(false, true)
    {
        creators["wotlk-uk"] = &DungeonStrategyContext::wotlk_uk;   // Utgarde Keep
        creators["wotlk-nex"] = &DungeonStrategyContext::wotlk_nex; // The Nexus
        creators["wotlk-an"] = &DungeonStrategyContext::wotlk_an;   // Azjol-Nerub
        creators["wotlk-ok"] = &DungeonStrategyContext::wotlk_ok;   // Old Kingdom
        creators["wotlk-dtk"] = &DungeonStrategyContext::wotlk_dtk; // Drak'Tharon Keep
        creators["wotlk-vh"] = &DungeonStrategyContext::wotlk_vh;   // The Violet Hold
        creators["wotlk-gd"] = &DungeonStrategyContext::wotlk_gd;   // Gundrak
        creators["wotlk-hos"] = &DungeonStrategyContext::wotlk_hos; // Halls of Stone
        creators["wotlk-hol"] = &DungeonStrategyContext::wotlk_hol; // Halls of Lightning
        creators["wotlk-occ"] = &DungeonStrategyContext::wotlk_occ; // The Oculus
        creators["wotlk-up"] = &DungeonStrategyContext::wotlk_up;   // Utgarde Pinnacle
        creators["wotlk-cos"] = &DungeonStrategyContext::wotlk_cos; // Culling of Stratholme
        creators["wotlk-toc"] = &DungeonStrategyContext::wotlk_toc; // Trial of the Champion
        creators["wotlk-pos"] = &DungeonStrategyContext::wotlk_pos; // Pit of Saron
        creators["wotlk-fos"] = &DungeonStrategyContext::wotlk_fos; // Forge of Souls
        creators["wotlk-hor"] = &DungeonStrategyContext::wotlk_hor; // Halls of Reflection (generic fallback)
    }

private:
    static Strategy* wotlk_uk(PlayerbotAI* botAI) { return new WotlkDungeonUKStrategy(botAI); }
    static Strategy* wotlk_nex(PlayerbotAI* botAI) { return new WotlkDungeonNexStrategy(botAI); }
    static Strategy* wotlk_an(PlayerbotAI* botAI) { return new WotlkDungeonANStrategy(botAI); }
    static Strategy* wotlk_ok(PlayerbotAI* botAI) { return new WotlkDungeonOKStrategy(botAI); }
    static Strategy* wotlk_dtk(PlayerbotAI* botAI) { return new WotlkDungeonDTKStrategy(botAI); }
    static Strategy* wotlk_vh(PlayerbotAI* botAI) { return new WotlkDungeonVHStrategy(botAI); }
    static Strategy* wotlk_gd(PlayerbotAI* botAI) { return new WotlkDungeonGDStrategy(botAI); }
    static Strategy* wotlk_hos(PlayerbotAI* botAI) { return new WotlkDungeonHoSStrategy(botAI); }
    static Strategy* wotlk_hol(PlayerbotAI* botAI) { return new WotlkDungeonHoLStrategy(botAI); }
    static Strategy* wotlk_occ(PlayerbotAI* botAI) { return new WotlkDungeonOccStrategy(botAI); }
    static Strategy* wotlk_up(PlayerbotAI* botAI) { return new WotlkDungeonUPStrategy(botAI); }
    static Strategy* wotlk_cos(PlayerbotAI* botAI) { return new WotlkDungeonCoSStrategy(botAI); }
    static Strategy* wotlk_toc(PlayerbotAI* botAI) { return new WotlkDungeonToCStrategy(botAI); }
    static Strategy* wotlk_pos(PlayerbotAI* botAI) { return new WotlkDungeonPoSStrategy(botAI); }
    static Strategy* wotlk_fos(PlayerbotAI* botAI) { return new WotlkDungeonFoSStrategy(botAI); }
    static Strategy* wotlk_hor(PlayerbotAI* botAI) { return new DungeonStrategy(botAI); }
};

#endif
