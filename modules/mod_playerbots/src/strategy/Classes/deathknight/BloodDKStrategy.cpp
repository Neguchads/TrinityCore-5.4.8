

#include "BloodDKStrategy.h"

#include "Playerbots.h"

class BloodDKStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    BloodDKStrategyActionNodeFactory()
    {
        // creators["melee"] = &melee;
        // creators["blood strike"] = &blood_strike;
        creators["rune strike"] = &rune_strike;
        creators["heart strike"] = &heart_strike;
        creators["death strike"] = &death_strike;
        creators["outbreak"] = &outbreak;
        creators["blood boil"] = &blood_boil;
        creators["death and decay"] = &death_and_decay;
        creators["soul reaper"] = &soul_reaper;
        creators["bone shield"] = &bone_shield;
        creators["dancing rune weapon"] = &dancing_rune_weapon;
        // creators["death grip"] = &death_grip;
        // creators["plague strike"] = &plague_strike;
        // creators["pestilence"] = &pestilence;
        creators["icy touch"] = &icy_touch;
        // creators["obliterate"] = &obliterate;
        // creators["blood boil"] = &blood_boil;
        // creators["mark of_blood"] = &mark_of_blood;
        // creators["blood presence"] = &blood_presence;
        // creators["rune tap"] = &rune_tap;
        // creators["vampiric blood"] = &vampiric_blood;
        // creators["death pact"] = &death_pact;
        // creators["death rune_mastery"] = &death_rune_mastery;
        // creators["hysteria"] = &hysteria;
        // creators["dancing weapon"] = &dancing_weapon;
        creators["dark command"] = &dark_command;
        creators["taunt spell"] = &dark_command;
    }

private:
    static ActionNode* rune_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("rune strike",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* icy_touch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("icy touch",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* heart_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("heart strike",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }

    static ActionNode* death_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("death strike",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* outbreak([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("outbreak",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* blood_boil([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("blood boil",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* death_and_decay([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("death and decay",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* soul_reaper([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("soul reaper",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* bone_shield([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("bone shield",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* dancing_rune_weapon([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("dancing rune weapon",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), nullptr),
                              /*A*/ nullptr,
                              /*C*/ nullptr);
    }
    static ActionNode* dark_command([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("dark command",
                              /*P*/ NextAction::array(0, new NextAction("blood presence"), NULL),
                              /*A*/ NextAction::array(0, new NextAction("death grip"), NULL),
                              /*C*/ NULL);
    }
};

BloodDKStrategy::BloodDKStrategy(PlayerbotAI* botAI) : GenericDKStrategy(botAI)
{
    actionNodeFactories.Add(new BloodDKStrategyActionNodeFactory());
}

NextAction** BloodDKStrategy::getDefaultActions()
{
    return NextAction::array(
        0, new NextAction("bone shield", ACTION_DEFAULT + 0.95f),
        new NextAction("outbreak", ACTION_DEFAULT + 0.9f),
        new NextAction("dancing rune weapon", ACTION_DEFAULT + 0.8f),
        new NextAction("death strike", ACTION_DEFAULT + 0.7f),
        new NextAction("heart strike", ACTION_DEFAULT + 0.6f),
        new NextAction("blood boil", ACTION_DEFAULT + 0.5f),
        new NextAction("death and decay", ACTION_DEFAULT + 0.4f),
        new NextAction("rune strike", ACTION_DEFAULT + 0.3f),
        new NextAction("soul reaper", ACTION_DEFAULT + 0.25f),
        new NextAction("horn of winter", ACTION_DEFAULT + 0.1f),
        new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void BloodDKStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDKStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "rune strike", NextAction::array(0, new NextAction("rune strike", ACTION_NORMAL + 3), nullptr)));
    triggers.push_back(
        new TriggerNode("blood tap", NextAction::array(0, new NextAction("blood tap", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(
        new TriggerNode("lose aggro", NextAction::array(0, new NextAction("dark command", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(
        new TriggerNode("low health", NextAction::array(0, new NextAction("army of the dead", ACTION_HIGH + 4),
                                                        new NextAction("death strike", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(
        new TriggerNode("critical health", NextAction::array(0, new NextAction("vampiric blood", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(
        new TriggerNode("icy touch", NextAction::array(0, new NextAction("icy touch", ACTION_HIGH + 2), nullptr)));
    triggers.push_back(new TriggerNode(
        "plague strike", NextAction::array(0, new NextAction("plague strike", ACTION_HIGH + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "crimson scourge", NextAction::array(0, new NextAction("death and decay", ACTION_HIGH + 4), nullptr)));
    triggers.push_back(new TriggerNode(
        "scent of blood", NextAction::array(0, new NextAction("death strike", ACTION_HIGH + 3), nullptr)));
    triggers.push_back(new TriggerNode(
        "target critical health", NextAction::array(0, new NextAction("soul reaper", ACTION_HIGH + 5), nullptr)));
}
