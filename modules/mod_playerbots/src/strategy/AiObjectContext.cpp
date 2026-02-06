

#include "AiObjectContext.h"

#include "Helper.h"
#include "ActionContext.h"
#include "AzjolNerubActionContext.h"
#include "CullingOfStratholmeActionContext.h"
#include "DrakTharonKeepActionContext.h"
#include "ForgeOfSoulsActionContext.h"
#include "GundrakActionContext.h"
#include "HallsOfLightningActionContext.h"
#include "HallsOfStoneActionContext.h"
#include "NexusActionContext.h"
#include "OculusActionContext.h"
#include "OldKingdomActionContext.h"
#include "PitOfSaronActionContext.h"
#include "RaidAq20ActionContext.h"
#include "RaidBwlActionContext.h"
#include "RaidEoEActionContext.h"
#include "RaidGruulsLairActionContext.h"
#include "RaidIccActionContext.h"
#include "RaidKarazhanActionContext.h"
#include "RaidMagtheridonActionContext.h"
#include "RaidMcActionContext.h"
#include "RaidOnyxiaActionContext.h"
#include "RaidOsActionContext.h"
#include "RaidUlduarActionContext.h"
#include "RaidVoAActionContext.h"
#include "TrialOfTheChampionActionContext.h"
#include "UtgardeKeepActionContext.h"
#include "UtgardePinnacleActionContext.h"
#include "VioletHoldActionContext.h"

#include "Playerbots.h"

#include "SharedValueContext.h"
#include "StrategyContext.h"
#include "TriggerContext.h"
#include "AzjolNerubTriggerContext.h"
#include "CullingOfStratholmeTriggerContext.h"
#include "DrakTharonKeepTriggerContext.h"
#include "ForgeOfSoulsTriggerContext.h"
#include "GundrakTriggerContext.h"
#include "HallsOfLightningTriggerContext.h"
#include "HallsOfStoneTriggerContext.h"
#include "NexusTriggerContext.h"
#include "OculusTriggerContext.h"
#include "OldKingdomTriggerContext.h"
#include "PitOfSaronTriggerContext.h"
#include "RaidAq20TriggerContext.h"
#include "RaidBwlTriggerContext.h"
#include "RaidEoETriggerContext.h"
#include "RaidGruulsLairTriggerContext.h"
#include "RaidIccTriggerContext.h"
#include "RaidKarazhanTriggerContext.h"
#include "RaidMagtheridonTriggerContext.h"
#include "RaidMcTriggerContext.h"
#include "RaidOnyxiaTriggerContext.h"
#include "RaidOsTriggerContext.h"
#include "RaidUlduarTriggerContext.h"
#include "RaidVoATriggerContext.h"
#include "TrialOfTheChampionTriggerContext.h"
#include "UtgardeKeepTriggerContext.h"
#include "UtgardePinnacleTriggerContext.h"
#include "VioletHoldTriggerContext.h"
#include "ValueContext.h"
#include "WorldPacketActionContext.h"
#include "WorldPacketTriggerContext.h"


AiObjectContext::AiObjectContext(PlayerbotAI* botAI) : PlayerbotAIAware(botAI)
{
    strategyContexts.Add(new StrategyContext());
    strategyContexts.Add(new MovementStrategyContext());
    strategyContexts.Add(new AssistStrategyContext());
    strategyContexts.Add(new QuestStrategyContext());
    strategyContexts.Add(new FishStrategyContext());

    actionContexts.Add(new ActionContext());
    actionContexts.Add(new WorldPacketActionContext());
    actionContexts.Add(new WotlkDungeonANActionContext());
    actionContexts.Add(new WotlkDungeonCoSActionContext());
    actionContexts.Add(new WotlkDungeonDTKActionContext());
    actionContexts.Add(new WotlkDungeonFoSActionContext());
    actionContexts.Add(new WotlkDungeonGDActionContext());
    actionContexts.Add(new WotlkDungeonHoLActionContext());
    actionContexts.Add(new WotlkDungeonHoSActionContext());
    actionContexts.Add(new WotlkDungeonNexActionContext());
    actionContexts.Add(new WotlkDungeonOccActionContext());
    actionContexts.Add(new WotlkDungeonOKActionContext());
    actionContexts.Add(new WotlkDungeonPoSActionContext());
    actionContexts.Add(new WotlkDungeonToCActionContext());
    actionContexts.Add(new WotlkDungeonUKActionContext());
    actionContexts.Add(new WotlkDungeonUPActionContext());
    actionContexts.Add(new WotlkDungeonVHActionContext());
    actionContexts.Add(new RaidAq20ActionContext());
    actionContexts.Add(new RaidBwlActionContext());
    actionContexts.Add(new RaidEoEActionContext());
    actionContexts.Add(new RaidGruulsLairActionContext());
    actionContexts.Add(new RaidIccActionContext());
    actionContexts.Add(new RaidKarazhanActionContext());
    actionContexts.Add(new RaidMagtheridonActionContext());
    actionContexts.Add(new RaidMcActionContext());
    actionContexts.Add(new RaidOnyxiaActionContext());
    actionContexts.Add(new RaidOsActionContext());
    actionContexts.Add(new RaidUlduarActionContext());
    actionContexts.Add(new RaidVoAActionContext());

    triggerContexts.Add(new TriggerContext());
    triggerContexts.Add(new WorldPacketTriggerContext());
    triggerContexts.Add(new WotlkDungeonANTriggerContext());
    triggerContexts.Add(new WotlkDungeonCoSTriggerContext());
    triggerContexts.Add(new WotlkDungeonDTKTriggerContext());
    triggerContexts.Add(new WotlkDungeonFoSTriggerContext());
    triggerContexts.Add(new WotlkDungeonGDTriggerContext());
    triggerContexts.Add(new WotlkDungeonHoLTriggerContext());
    triggerContexts.Add(new WotlkDungeonHoSTriggerContext());
    triggerContexts.Add(new WotlkDungeonNexTriggerContext());
    triggerContexts.Add(new WotlkDungeonOccTriggerContext());
    triggerContexts.Add(new WotlkDungeonOKTriggerContext());
    triggerContexts.Add(new WotlkDungeonPoSTriggerContext());
    triggerContexts.Add(new WotlkDungeonToCTriggerContext());
    triggerContexts.Add(new WotlkDungeonUKTriggerContext());
    triggerContexts.Add(new WotlkDungeonUPTriggerContext());
    triggerContexts.Add(new WotlkDungeonVHTriggerContext());
    triggerContexts.Add(new RaidAq20TriggerContext());
    triggerContexts.Add(new RaidBwlTriggerContext());
    triggerContexts.Add(new RaidEoETriggerContext());
    triggerContexts.Add(new RaidGruulsLairTriggerContext());
    triggerContexts.Add(new RaidIccTriggerContext());
    triggerContexts.Add(new RaidKarazhanTriggerContext());
    triggerContexts.Add(new RaidMagtheridonTriggerContext());
    triggerContexts.Add(new RaidMcTriggerContext());
    triggerContexts.Add(new RaidOnyxiaTriggerContext());
    triggerContexts.Add(new RaidOsTriggerContext());
    triggerContexts.Add(new RaidUlduarTriggerContext());
    triggerContexts.Add(new RaidVoATriggerContext());

    valueContexts.Add(new ValueContext());
    valueContexts.Add(sSharedValueContext);
}

void AiObjectContext::Update()
{
    strategyContexts.Update();
    triggerContexts.Update();
    actionContexts.Update();
    valueContexts.Update();
}

void AiObjectContext::Reset()
{
    strategyContexts.Reset();
    triggerContexts.Reset();
    actionContexts.Reset();
    valueContexts.Reset();
}

std::vector<std::string> AiObjectContext::Save()
{
    std::vector<std::string> result;

    std::set<std::string> names = valueContexts.GetCreated();
    for (std::set<std::string>::iterator i = names.begin(); i != names.end(); ++i)
    {
        UntypedValue* value = GetUntypedValue(*i);
        if (!value)
            continue;

        std::string const data = value->Save();
        if (data == "?")
            continue;

        std::string const name = *i;
        std::ostringstream out;
        out << name;

        out << ">" << data;
        result.push_back(out.str());
    }

    return result;
}

void AiObjectContext::Load(std::vector<std::string> data)
{
    for (std::vector<std::string>::iterator i = data.begin(); i != data.end(); ++i)
    {
        std::string const row = *i;
        std::vector<std::string> parts = split(row, '>');
        if (parts.size() != 2)
            continue;

        std::string const name = parts[0];
        std::string const text = parts[1];

        UntypedValue* value = GetUntypedValue(name);
        if (!value)
            continue;

        value->Load(text);
    }
}

Strategy* AiObjectContext::GetStrategy(std::string const name)
{
    Strategy* strategy = strategyContexts.GetContextObject(name, botAI);
    return strategy;
}

std::set<std::string> AiObjectContext::GetSiblingStrategy(std::string const name)
{
    return strategyContexts.GetSiblings(name);
}

Trigger* AiObjectContext::GetTrigger(std::string const name)
{
    Trigger* trigger = triggerContexts.GetContextObject(name, botAI);
    return trigger;
}

Action* AiObjectContext::GetAction(std::string const name)
{
    Action* action = actionContexts.GetContextObject(name, botAI);
    return action;
}

UntypedValue* AiObjectContext::GetUntypedValue(std::string const name)
{
    return valueContexts.GetContextObject(name, botAI);
}

std::set<std::string> AiObjectContext::GetValues() { return valueContexts.GetCreated(); }

std::set<std::string> AiObjectContext::GetSupportedStrategies() { return strategyContexts.supports(); }

std::set<std::string> AiObjectContext::GetSupportedActions() { return actionContexts.supports(); }

std::string const AiObjectContext::FormatValues()
{
    std::ostringstream out;
    std::set<std::string> names = valueContexts.GetCreated();
    for (std::set<std::string>::iterator i = names.begin(); i != names.end(); ++i, out << "|")
    {
        UntypedValue* value = GetUntypedValue(*i);
        if (!value)
            continue;

        std::string const text = value->Format();
        if (text == "?")
            continue;

        out << "{" << *i << "=" << text << "}";
    }

    return out.str();
}

void AiObjectContext::AddShared(NamedObjectContext<UntypedValue>* sharedValues) { valueContexts.Add(sharedValues); }
