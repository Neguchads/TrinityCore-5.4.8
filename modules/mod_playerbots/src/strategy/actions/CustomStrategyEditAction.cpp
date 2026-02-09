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

#include "CustomStrategyEditAction.h"

#include "CustomStrategy.h"
#include "DatabaseEnv.h"
#include "Event.h"
#include "Playerbots.h"

bool CustomStrategyEditAction::Execute(Event event)
{
    std::string text = event.getParam();
    size_t pos = text.find(' ');
    if (pos == std::string::npos)
        return PrintHelp();

    std::string const name = text.substr(0, pos);
    text = text.substr(pos + 1);

    pos = text.find(' ');
    if (pos == std::string::npos)
        pos = text.size();

    std::string const idx = text.substr(0, pos);
    text = pos >= text.size() ? "" : text.substr(pos + 1);

    return idx == "?" ? Print(name) : Edit(name, atoi(idx.c_str()), text);
}

bool CustomStrategyEditAction::PrintHelp()
{
    botAI->TellMaster("=== Custom strategies ===");

    uint32 owner = botAI->GetBot()->GetGUID().GetCounter();

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT DISTINCT name FROM playerbots_custom_strategy WHERE owner = %u", owner);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            std::string const name = fields[0].GetString();
            botAI->TellMaster(name);
        } while (result->NextRow());
    }

    botAI->TellMaster("Usage: cs <name> <idx> <command>");
    return false;
}

bool CustomStrategyEditAction::Print(std::string const name)
{
    std::ostringstream out;
    out << "=== " << name << " ===";
    botAI->TellMaster(out.str());

    uint32 owner = botAI->GetBot()->GetGUID().GetCounter();

    std::string nameEsc = name;
    CharacterDatabase.EscapeString(nameEsc);

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT idx, action_line FROM playerbots_custom_strategy WHERE owner = %u AND name = '%s' ORDER BY idx",
        owner, nameEsc.c_str());
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 idx = fields[0].GetUInt32();
            std::string const action = fields[1].GetString();

            PrintActionLine(idx, action);
        } while (result->NextRow());
    }

    return true;
}

bool CustomStrategyEditAction::Edit(std::string const name, uint32 idx, std::string const command)
{
    uint32 owner = botAI->GetBot()->GetGUID().GetCounter();

    std::string nameEsc = name;
    CharacterDatabase.EscapeString(nameEsc);

    std::string commandEsc = command;
    CharacterDatabase.EscapeString(commandEsc);

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT 1 FROM playerbots_custom_strategy WHERE owner = %u AND name = '%s' AND idx = %u",
        owner, nameEsc.c_str(), idx);

    if (result)
    {
        if (command.empty())
        {
            CharacterDatabase.PExecute(
                "DELETE FROM playerbots_custom_strategy WHERE owner = %u AND name = '%s' AND idx = %u",
                owner, nameEsc.c_str(), idx);
        }
        else
        {
            CharacterDatabase.PExecute(
                "UPDATE playerbots_custom_strategy SET action_line = '%s' WHERE owner = %u AND name = '%s' AND idx = %u",
                commandEsc.c_str(), owner, nameEsc.c_str(), idx);
        }
    }
    else
    {
        CharacterDatabase.PExecute(
            "INSERT INTO playerbots_custom_strategy (name, owner, idx, action_line) VALUES ('%s', %u, %u, '%s')",
            nameEsc.c_str(), owner, idx, commandEsc.c_str());
    }

    PrintActionLine(idx, command);

    std::ostringstream ss;
    ss << "custom::" << name;

    if (Strategy* strategy = botAI->GetAiObjectContext()->GetStrategy(ss.str()))
    {
        if (CustomStrategy* cs = dynamic_cast<CustomStrategy*>(strategy))
        {
            cs->Reset();
            botAI->ReInitCurrentEngine();
        }
    }

    return true;
}

bool CustomStrategyEditAction::PrintActionLine(uint32 idx, std::string const command)
{
    std::ostringstream out;
    out << "#" << idx << " " << command;
    botAI->TellMaster(out.str());
    return true;
}
