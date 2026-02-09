#/*
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

#include "UnholyDKStrategy.h"

#include "Playerbots.h"

class UnholyDKStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    UnholyDKStrategyActionNodeFactory()
    {
        // Unholy
        // creators["bone shield"] = &bone_shield;
        // creators["plague strike"] = &plague_strike;
        // creators["death grip"] = &death_grip;
        // creators["death coil"] = &death_coil;
        creators["death strike"] = &death_strike;
        creators["outbreak"] = &outbreak;
        creators["festering strike"] = &festering_strike;
        // creators["unholy blight"] = &unholy_blight;
        creators["scourge strike"] = &scourge_strike;
        creators["soul reaper"] = &soul_reaper;
        creators["dark transformation"] = &dark_transformation;
        // creators["death and decay"] = &death_and_decay;
        // creators["unholy pressence"] = &unholy_pressence;
        // creators["raise dead"] = &raise_dead;
        // creators["army of the dead"] = &army of the dead;
        // creators["summon gargoyle"] = &army of the dead;
        // creators["anti magic shell"] = &anti_magic_shell;
        // creators["anti magic zone"] = &anti_magic_zone;
        creators["ghoul frenzy"] = &ghoul_frenzy;
        creators["corpse explosion"] = &corpse_explosion;
        creators["icy touch"] = &icy_touch;
    }

private:
    static ActionNode* death_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("death strike",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* outbreak([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("outbreak",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* festering_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("festering strike",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* ghoul_frenzy([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("ghoul frenzy",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* corpse_explosion([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("corpse explosion",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* scourge_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("scourge strike",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* soul_reaper([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("soul reaper",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* dark_transformation([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("dark transformation",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* icy_touch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("icy touch",
                              /*P*/ NextAction::array(0, new NextAction("unholy presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
};

UnholyDKStrategy::UnholyDKStrategy(PlayerbotAI* botAI) : GenericDKStrategy(botAI)
{
    actionNodeFactories.Add(new UnholyDKStrategyActionNodeFactory());
}

NextAction** UnholyDKStrategy::getDefaultActions()
{
    return NextAction::array(
        0, new NextAction("outbreak", ACTION_DEFAULT + 0.95f),
        new NextAction("summon gargoyle", ACTION_DEFAULT + 0.9f),
        new NextAction("dark transformation", ACTION_DEFAULT + 0.85f),
        new NextAction("death and decay", ACTION_DEFAULT + 0.8f),
        new NextAction("festering strike", ACTION_DEFAULT + 0.75f),
        new NextAction("scourge strike", ACTION_DEFAULT + 0.7f),
        new NextAction("death coil", ACTION_DEFAULT + 0.6f),
        new NextAction("soul reaper", ACTION_DEFAULT + 0.55f),
        new NextAction("empower rune weapon", ACTION_DEFAULT + 0.3f),
        new NextAction("horn of winter", ACTION_DEFAULT + 0.1f),
        new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void UnholyDKStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDKStrategy::InitTriggers(triggers);
    triggers.push_back(new TriggerNode(
        "sudden doom", NextAction::array(0, new NextAction("death coil", ACTION_HIGH + 4), nullptr)));

    // triggers.push_back(
    //     new TriggerNode("icy touch", NextAction::array(0, new NextAction("icy touch", ACTION_HIGH + 2), nullptr)));
    // triggers.push_back(new TriggerNode(
    //     "plague strike", NextAction::array(0, new NextAction("plague strike", ACTION_HIGH + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "high frost rune", NextAction::array(0, new NextAction("festering strike", ACTION_NORMAL + 3), nullptr)));

    triggers.push_back(new TriggerNode(
        "high blood rune", NextAction::array(0, new NextAction("festering strike", ACTION_NORMAL + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "high unholy rune", NextAction::array(0, new NextAction("scourge strike", ACTION_NORMAL + 1), nullptr)));

    triggers.push_back(
        new TriggerNode("no rune", NextAction::array(0, new NextAction("empower rune weapon", ACTION_HIGH + 1), nullptr)));

    // triggers.push_back(new TriggerNode("often", NextAction::array(0, new NextAction(, ACTION_NORMAL + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "army of the dead", NextAction::array(0, new NextAction("army of the dead", ACTION_HIGH + 6), nullptr)));
    triggers.push_back(
        new TriggerNode("bone shield", NextAction::array(0, new NextAction("bone shield", ACTION_HIGH + 3), nullptr)));

    triggers.push_back(new TriggerNode(
        "dark transformation", NextAction::array(0, new NextAction("dark transformation", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode(
        "target critical health", NextAction::array(0, new NextAction("soul reaper", ACTION_HIGH + 6), nullptr)));
}

void UnholyDKAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "loot available", NextAction::array(0, new NextAction("corpse explosion", ACTION_NORMAL + 1), nullptr)));
    triggers.push_back(new TriggerNode(
        "medium aoe", NextAction::array(0, new NextAction("death and decay", ACTION_NORMAL + 3),
                                        new NextAction("corpse explosion", ACTION_NORMAL + 3), nullptr)));
}
