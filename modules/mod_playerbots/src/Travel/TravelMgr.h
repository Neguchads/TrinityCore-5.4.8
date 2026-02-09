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

#ifndef _PLAYERBOT_TRAVELMGR_H
#define _PLAYERBOT_TRAVELMGR_H

#include <boost/functional/hash.hpp>

#include <algorithm>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "AiObject.h"
#include "WorldPosition.h"

class Player;
class PlayerbotAI;
class Quest;
class CreatureTemplate;
class GameObjectTemplate;
struct QuestStatusData;

class mapTransfer
{
public:
    mapTransfer(WorldPosition pointFrom1, WorldPosition pointTo1, float portalLength1 = 0.1f)
        : pointFrom(pointFrom1), pointTo(pointTo1), portalLength(portalLength1)
    {
    }

    bool isFrom(WorldPosition point) { return point.getMapId() == pointFrom.getMapId(); }
    bool isTo(WorldPosition point) { return point.getMapId() == pointTo.getMapId(); }

    WorldPosition* getPointFrom() { return &pointFrom; }
    WorldPosition* getPointTo() { return &pointTo; }

    bool isUseful(WorldPosition point) { return isFrom(point) || isTo(point); }
    float distance(WorldPosition point)
    {
        return isUseful(point) ? (isFrom(point) ? point.distance(pointFrom) : point.distance(pointTo)) : 200000.0f;
    }

    bool isUseful(WorldPosition start, WorldPosition end) { return isFrom(start) && isTo(end); }

    float distance(WorldPosition start, WorldPosition end)
    {
        return isUseful(start, end) ? (start.distance(pointFrom) + portalLength + pointTo.distance(end)) : 200000.0f;
    }

    float fDist(WorldPosition start, WorldPosition end);

private:
    WorldPosition pointFrom;
    WorldPosition pointTo;
    float portalLength = 0.1f;
};

// A destination for a bot to travel to and do something.
class TravelDestination
{
public:
    TravelDestination() {}
    TravelDestination(float radiusMin1, float radiusMax1)
    {
        radiusMin = radiusMin1;
        radiusMax = radiusMax1;
    }
    TravelDestination(std::vector<WorldPosition*> points1, float radiusMin1, float radiusMax1)
    {
        points = points1;
        radiusMin = radiusMin1;
        radiusMax = radiusMax1;
    }
    virtual ~TravelDestination() = default;

    void addPoint(WorldPosition* pos) { points.push_back(pos); }
    void setExpireDelay(uint32 delay) { expireDelay = delay; }
    void setCooldownDelay(uint32 delay) { cooldownDelay = delay; }

    void setMaxVisitors(uint32 maxVisitors1 = 0, uint32 maxVisitorsPerPoint1 = 0)
    {
        maxVisitors = maxVisitors1;
        maxVisitorsPerPoint = maxVisitorsPerPoint1;
    }

    std::vector<WorldPosition*> getPoints(bool ignoreFull = false);
    uint32 getExpireDelay() { return expireDelay; }
    uint32 getCooldownDelay() { return cooldownDelay; }

    void addVisitor() { ++visitors; }
    void remVisitor() { --visitors; }
    uint32 getVisitors() { return visitors; }

    virtual Quest const* GetQuestTemplate() { return nullptr; }
    virtual bool isActive([[maybe_unused]] Player* bot) { return false; }

    bool isFull(bool ignoreFull = false);

    virtual std::string const getName() { return "TravelDestination"; }
    virtual int32 getEntry() { return 0; }
    virtual std::string const getTitle() { return "generic travel destination"; }

    WorldPosition* nearestPoint(WorldPosition* pos);

    float distanceTo(WorldPosition* pos) { return nearestPoint(pos)->distance(pos); }
    bool onMap(WorldPosition* pos) { return nearestPoint(pos)->getMapId() == pos->getMapId(); }
    virtual bool isIn(WorldPosition* pos, float radius = 0.f)
    {
        return onMap(pos) && distanceTo(pos) <= (radius ? radius : radiusMin);
    }
    virtual bool isOut(WorldPosition* pos, float radius = 0.f)
    {
        return !onMap(pos) || distanceTo(pos) > (radius ? radius : radiusMax);
    }

    float getRadiusMin() { return radiusMin; }

    std::vector<WorldPosition*> touchingPoints(WorldPosition* pos);
    std::vector<WorldPosition*> sortedPoints(WorldPosition* pos);
    std::vector<WorldPosition*> nextPoint(WorldPosition* pos, bool ignoreFull = true);

protected:
    std::vector<WorldPosition*> points;
    float radiusMin = 0.0f;
    float radiusMax = 0.0f;

    uint32 visitors = 0;
    uint32 maxVisitors = 0;
    uint32 maxVisitorsPerPoint = 0;
    uint32 expireDelay = 5 * 1000;
    uint32 cooldownDelay = 60 * 1000;
};

class NullTravelDestination : public TravelDestination
{
public:
    NullTravelDestination(uint32 cooldownDelay1 = 5 * 60 * 1000) : TravelDestination()
    {
        cooldownDelay = cooldownDelay1;
    };

    Quest const* GetQuestTemplate() override { return nullptr; }
    bool isActive([[maybe_unused]] Player* bot) override { return false; }

    std::string const getName() override { return "NullTravelDestination"; }
    std::string const getTitle() override { return "no destination"; }

    bool isIn([[maybe_unused]] WorldPosition* pos, [[maybe_unused]] float radius = 0.f) override { return true; }
    bool isOut([[maybe_unused]] WorldPosition* pos, [[maybe_unused]] float radius = 0.f) override { return false; }
};

class QuestTravelDestination : public TravelDestination
{
public:
    QuestTravelDestination(uint32 questId1, float radiusMin1, float radiusMax1);

    Quest const* GetQuestTemplate() override { return questTemplate; }
    bool isActive(Player* bot) override;

    std::string const getName() override { return "QuestTravelDestination"; }
    int32 getEntry() override { return 0; }
    std::string const getTitle() override;

protected:
    uint32 questId = 0;
    Quest const* questTemplate = nullptr;
};

class QuestRelationTravelDestination : public QuestTravelDestination
{
public:
    QuestRelationTravelDestination(uint32 quest_id1, uint32 entry1, uint32 relation1, float radiusMin1,
                                   float radiusMax1)
        : QuestTravelDestination(quest_id1, radiusMin1, radiusMax1)
    {
        entry = entry1;
        relation = relation1;
    }

    bool isActive(Player* bot) override;
    std::string const getName() override { return "QuestRelationTravelDestination"; }
    int32 getEntry() override { return entry; }
    std::string const getTitle() override;
    virtual uint32 getRelation() { return relation; }

private:
    uint32 relation = 0;
    int32 entry = 0;
};

class QuestObjectiveTravelDestination : public QuestTravelDestination
{
public:
    QuestObjectiveTravelDestination(uint32 quest_id1, uint32 entry1, uint32 objective1, float radiusMin1,
                                    float radiusMax1, uint32 itemId1 = 0)
        : QuestTravelDestination(quest_id1, radiusMin1, radiusMax1)
    {
        objective = objective1;
        entry = entry1;
        itemId = itemId1;
    }

    bool isCreature();
    uint32 ReqCreature();
    uint32 ReqGOId();
    uint32 ReqCount();

    bool isActive(Player* bot) override;
    std::string const getName() override { return "QuestObjectiveTravelDestination"; }
    int32 getEntry() override { return entry; }
    std::string const getTitle() override;

private:
    uint32 objective = 0;
    uint32 entry = 0;
    uint32 itemId = 0;
};

class RpgTravelDestination : public TravelDestination
{
public:
    RpgTravelDestination(uint32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1)
    {
        entry = entry1;
    }

    bool isActive(Player* bot) override;
    virtual CreatureTemplate const* GetCreatureTemplate();
    std::string const getName() override { return "RpgTravelDestination"; }
    int32 getEntry() override { return 0; }
    std::string const getTitle() override;

protected:
    uint32 entry = 0;
};

class ExploreTravelDestination : public TravelDestination
{
public:
    ExploreTravelDestination(uint32 areaId1, float radiusMin1, float radiusMax1)
        : TravelDestination(radiusMin1, radiusMax1)
    {
        areaId = areaId1;
    }

    bool isActive(Player* bot) override;
    std::string const getName() override { return "ExploreTravelDestination"; }
    int32 getEntry() override { return 0; }
    std::string const getTitle() override { return title; };
    virtual void setTitle(std::string newTitle) { title = std::move(newTitle); }
    virtual uint32 getAreaId() { return areaId; }

protected:
    uint32 areaId = 0;
    std::string title;
};

class GrindTravelDestination : public TravelDestination
{
public:
    GrindTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1)
    {
        entry = entry1;
    }

    bool isActive(Player* bot) override;
    virtual CreatureTemplate const* GetCreatureTemplate();
    std::string const getName() override { return "GrindTravelDestination"; }
    int32 getEntry() override { return entry; }
    std::string const getTitle() override;

protected:
    int32 entry = 0;
};

class BossTravelDestination : public TravelDestination
{
public:
    BossTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1)
    {
        entry = entry1;
        cooldownDelay = 1000;
    }

    bool isActive(Player* bot) override;
    CreatureTemplate const* getCreatureTemplate();
    std::string const getName() override { return "BossTravelDestination"; }
    int32 getEntry() override { return entry; }
    std::string const getTitle() override;

protected:
    int32 entry = 0;
};

class TravelTarget : AiObject
{
public:
    TravelTarget(PlayerbotAI* botAI) : AiObject(botAI), m_status(TRAVEL_STATUS_NONE), startTime(getMSTime()){};
    TravelTarget(PlayerbotAI* botAI, TravelDestination* tDestination1, WorldPosition* wPosition1)
        : AiObject(botAI), m_status(TRAVEL_STATUS_NONE), startTime(getMSTime())
    {
        setTarget(tDestination1, wPosition1);
    }

    ~TravelTarget();

    void setTarget(TravelDestination* tDestination1, WorldPosition* wPosition1, bool groupCopy1 = false);
    void setStatus(TravelStatus status);
    void setExpireIn(uint32 expireMs) { statusTime = getExpiredTime() + expireMs; }

    void incRetry(bool isMove)
    {
        if (isMove)
            ++moveRetryCount;
        else
            ++extendRetryCount;
    }

    void setRetry(bool isMove, uint32 newCount = 0)
    {
        if (isMove)
            moveRetryCount = newCount;
        else
            extendRetryCount = newCount;
    }

    void setForced(bool forced1) { forced = forced1; }
    void setRadius(float radius1) { radius = radius1; }

    void copyTarget(TravelTarget* target);
    void addVisitors();
    void releaseVisitors();

    float distance(Player* bot);

    WorldPosition* getPosition();
    TravelDestination* getDestination();

    uint32 getEntry()
    {
        if (!tDestination)
            return 0;

        return tDestination->getEntry();
    }

    PlayerbotAI* getAi() { return botAI; }

    uint32 getExpiredTime() { return getMSTime() - startTime; }
    uint32 getTimeLeft() { return statusTime - getExpiredTime(); }

    uint32 getMaxTravelTime();
    uint32 getRetryCount(bool isMove) { return isMove ? moveRetryCount : extendRetryCount; }

    bool isTraveling();
    bool isActive();
    bool isWorking();
    bool isPreparing();
    bool isMaxRetry(bool isMove) { return isMove ? (moveRetryCount > 5) : (extendRetryCount > 5); }

    TravelStatus getStatus() { return m_status; }
    TravelState getTravelState();

    bool isGroupCopy() { return groupCopy; }
    bool isForced() { return forced; }

protected:
    TravelStatus m_status;

    uint32 startTime = 0;
    uint32 statusTime = 0;

    bool forced = false;
    float radius = 0.f;
    bool groupCopy = false;
    bool visitor = true;

    uint32 extendRetryCount = 0;
    uint32 moveRetryCount = 0;

    TravelDestination* tDestination = nullptr;
    WorldPosition* wPosition = nullptr;
};

class TravelMgr
{
public:
    static TravelMgr& instance()
    {
        static TravelMgr instance;
        return instance;
    }

    void Clear();
    void LoadQuestTravelTable();

    template <class D, class W, class URBG>
    void weighted_shuffle(D first, D last, W first_weight, W last_weight, URBG&& g)
    {
        while (first != last && first_weight != last_weight)
        {
            std::discrete_distribution<int> dd(first_weight, last_weight);
            auto i = dd(g);

            if (i)
            {
                std::swap(*first, *std::next(first, i));
                std::swap(*first_weight, *std::next(first_weight, i));
            }
            ++first;
            ++first_weight;
        }
    }

    std::vector<WorldPosition*> getNextPoint(WorldPosition* center, std::vector<WorldPosition*> points,
                                             uint32 amount = 1);
    std::vector<WorldPosition> getNextPoint(WorldPosition center, std::vector<WorldPosition> points, uint32 amount = 1);
    QuestStatusData* getQuestStatus(Player* bot, uint32 questId);
    bool getObjectiveStatus(Player* bot, Quest const* pQuest, uint32 objective);
    uint32 getDialogStatus(Player* pPlayer, int32 questgiver, Quest const* pQuest);

    std::vector<TravelDestination*> getQuestTravelDestinations(Player* bot, int32 questId = -1, bool ignoreFull = false,
                                                               bool ignoreInactive = false, float maxDistance = 5000,
                                                               bool ignoreObjectives = false);
    std::vector<TravelDestination*> getRpgTravelDestinations(Player* bot, bool ignoreFull = false,
                                                             bool ignoreInactive = false, float maxDistance = 5000);
    std::vector<TravelDestination*> getExploreTravelDestinations(Player* bot, bool ignoreFull = false,
                                                                 bool ignoreInactive = false);
    std::vector<TravelDestination*> getGrindTravelDestinations(Player* bot, bool ignoreFull = false,
                                                               bool ignoreInactive = false, float maxDistance = 5000);
    std::vector<TravelDestination*> getBossTravelDestinations(Player* bot, bool ignoreFull = false,
                                                              bool ignoreInactive = false, float maxDistance = 25000);

    void setNullTravelTarget(Player* player);

    void addMapTransfer(WorldPosition start, WorldPosition end, float portalDistance = 0.1f, bool makeShortcuts = true);
    void loadMapTransfers();
    float mapTransDistance(WorldPosition start, WorldPosition end);
    float fastMapTransDistance(WorldPosition start, WorldPosition end);

    NullTravelDestination* nullTravelDestination = new NullTravelDestination();
    WorldPosition* nullWorldPosition = new WorldPosition();

    void addBadVmap(uint32 mapId, uint8 x, uint8 y) { badVmap.push_back(std::make_tuple(mapId, x, y)); }
    void addBadMmap(uint32 mapId, uint8 x, uint8 y) { badMmap.push_back(std::make_tuple(mapId, x, y)); }

    bool isBadVmap(uint32 mapId, uint8 x, uint8 y)
    {
        return std::find(badVmap.begin(), badVmap.end(), std::make_tuple(mapId, x, y)) != badVmap.end();
    }

    bool isBadMmap(uint32 mapId, uint8 x, uint8 y)
    {
        return std::find(badMmap.begin(), badMmap.end(), std::make_tuple(mapId, x, y)) != badMmap.end();
    }

    void printGrid(uint32 mapId, int x, int y, std::string const type);
    void printObj(WorldObject* obj, std::string const type);

    void logQuestError(uint32 errorNr, Quest* quest, uint32 objective = 0, uint32 unitId = 0, uint32 itemId = 0);

    std::vector<uint32> avoidLoaded;

    std::vector<QuestTravelDestination*> questGivers;
    std::vector<RpgTravelDestination*> rpgNpcs;
    std::vector<GrindTravelDestination*> grindMobs;
    std::vector<BossTravelDestination*> bossMobs;

    std::unordered_map<uint32, ExploreTravelDestination*> exploreLocs;
    std::unordered_map<uint32, QuestContainer*> quests;

    std::vector<std::tuple<uint32, uint8, uint8>> badVmap, badMmap;

    std::unordered_map<std::pair<uint32, uint32>, std::vector<mapTransfer>, boost::hash<std::pair<uint32, uint32>>>
        mapTransfersMap;

private:
    TravelMgr() = default;
    ~TravelMgr() = default;

    TravelMgr(const TravelMgr&) = delete;
    TravelMgr& operator=(const TravelMgr&) = delete;

    TravelMgr(TravelMgr&&) = delete;
    TravelMgr& operator=(TravelMgr&&) = delete;
};

#endif
