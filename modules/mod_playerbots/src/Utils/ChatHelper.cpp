/*
 * Minimal ChatHelper implementation for playerbots.
 */

#include "ChatHelper.h"

#include <cstdlib>
#include <sstream>

#include "ItemPrototype.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "QuestDef.h"
#include "SpellInfo.h"
#include "World.h"
#include "Object.h"

std::string const ChatHelper::FormatQuest(Quest const* quest)
{
    if (!quest)
        return "Invalid quest";

    std::string title = quest->GetLogTitle();
    if (QuestTemplateLocale const* locale = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
    {
        if (locale->LogTitle.size() > sWorld->GetDefaultDbcLocale())
        {
            std::string const& localized = locale->LogTitle[sWorld->GetDefaultDbcLocale()];
            if (!localized.empty())
                title = localized;
        }
    }

    std::ostringstream out;
    out << title << " (" << quest->GetQuestId() << ")";
    return out.str();
}

std::string const ChatHelper::FormatItem(ItemTemplate const* proto, uint32 count, uint32 total)
{
    if (!proto)
        return "Unknown item";

    std::string name = proto->Name1;
    if (ItemLocale const* locale = sObjectMgr->GetItemLocale(proto->ItemId))
    {
        if (locale->Name.size() > sWorld->GetDefaultDbcLocale())
        {
            std::string const& localized = locale->Name[sWorld->GetDefaultDbcLocale()];
            if (!localized.empty())
                name = localized;
        }
    }

    std::ostringstream out;
    out << name;
    if (count > 1)
        out << " x" << count;
    if (total > 0)
        out << " (" << total << ")";
    return out.str();
}

std::string const ChatHelper::FormatWorldEntry(int32 entry)
{
    CreatureTemplate const* cInfo = nullptr;
    GameObjectTemplate const* gInfo = nullptr;

    if (entry > 0)
        cInfo = sObjectMgr->GetCreatureTemplate(entry);
    else if (entry < 0)
        gInfo = sObjectMgr->GetGameObjectTemplate(static_cast<uint32>(-entry));

    std::ostringstream out;
    if (entry > 0 && cInfo)
        out << cInfo->Name;
    else if (entry < 0 && gInfo)
        out << gInfo->name;
    else
        out << "unknown";

    out << " (" << std::abs(entry) << ")";
    return out.str();
}

std::string const ChatHelper::FormatWorldobject(WorldObject const* object)
{
    if (!object)
        return "unknown";

    if (object->GetTypeId() == TYPEID_PLAYER)
        return object->ToPlayer()->GetName();

    if (object->GetTypeId() == TYPEID_UNIT)
        return FormatWorldEntry(static_cast<int32>(object->ToCreature()->GetEntry()));

    if (object->GetTypeId() == TYPEID_GAMEOBJECT)
        return FormatWorldEntry(-static_cast<int32>(object->ToGameObject()->GetEntry()));

    std::ostringstream out;
    out << "object (" << object->GetEntry() << ")";
    return out.str();
}

std::string const ChatHelper::FormatSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return "Unknown spell";

    std::string spellName = spellInfo->SpellName[sWorld->GetDefaultDbcLocale()]
        ? spellInfo->SpellName[sWorld->GetDefaultDbcLocale()]
        : spellInfo->SpellName[LOCALE_enUS];

    std::ostringstream out;
    out << spellName << " (" << spellInfo->Id << ")";
    return out.str();
}

std::string ChatHelper::formatValue(std::string const& type, std::string const& code, std::string const& name, std::string const& color)
{
    std::ostringstream out;
    out << "|c" << color << "|Hvalue:" << type << ":" << code << "|h[" << name << "]|h|r";
    return out.str();
}

std::string ChatHelper::parseValue(std::string const& type, std::string const& text)
{
    std::string const pattern = "Hvalue:" + type + ":";
    size_t pos = text.find(pattern);
    if (pos == std::string::npos)
        return std::string();

    pos += pattern.size();
    size_t endPos = text.find('|', pos);
    if (endPos == std::string::npos || endPos <= pos)
        return std::string();

    return text.substr(pos, endPos - pos);
}
