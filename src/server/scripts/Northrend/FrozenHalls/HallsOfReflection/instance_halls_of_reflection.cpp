/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Group.h"
#include "InstanceMapScript.h"
#include "InstanceScript.h"
#include "MapMgr.h"
#include "Transport.h"
#include "WorldStateDefines.h"
#include "halls_of_reflection.h"

class UtherBatteredHiltEvent : public BasicEvent
{
public:
    UtherBatteredHiltEvent(Creature& owner, uint8 eventId) : _owner(owner), _eventId(eventId) { }

    bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/) override
    {
        switch (_eventId)
        {
            case 1:
                _owner.UpdatePosition(5300.53f, 1987.80f, 707.70f, 3.89f, true);
                _owner.StopMovingOnCurrentPos();
                _owner.GetMotionMaster()->Clear();
                _owner.SetVisible(true);
                _owner.NearTeleportTo(5300.53f, 1987.80f, 707.70f, 3.89f);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, 2), _owner.m_Events.CalculateTime(1000));
                break;
            case 2:
                _owner.AI()->Talk(SAY_BATTERED_HILT_HALT);
                break;
            case 3:
                _owner.CastSpell((Unit*)nullptr, SPELL_SUMMON_EVIL_QUEL, true);
                _owner.AI()->Talk(SAY_BATTERED_HILT_REALIZE);
                if (InstanceScript* instance = _owner.GetInstanceScript())
                    instance->SetData(DATA_BATTERED_HILT, 4);
                if (Creature* quel = _owner.FindNearestCreature(NPC_QUEL_DELAR, 50))
                {
                    quel->AI()->Talk(EMOTE_QUEL_SPAWN);
                }
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, 4), _owner.m_Events.CalculateTime(3500));
                break;
            case 4:
                _owner.SetWalk(false);
                _owner.GetMotionMaster()->MovePoint(0, 5337.53f, 1981.21f, 709.32f);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, 5), _owner.m_Events.CalculateTime(6000));
                break;
            case 5:
                _owner.SetFacingTo(2.82f);
                _owner.SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 6:
                if (InstanceScript* instance = _owner.GetInstanceScript())
                    instance->SetData(DATA_BATTERED_HILT, 6);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, 7), _owner.m_Events.CalculateTime(2000));
                break;
            case 7:
                if (InstanceScript* instance = _owner.GetInstanceScript())
                {
                    instance->SetData(DATA_BATTERED_HILT, 7);
                }
                if (Creature* quel = _owner.FindNearestCreature(NPC_QUEL_DELAR, 50))
                {
                    quel->AI()->Talk(EMOTE_QUEL_PREPARE);
                }
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, 8), _owner.m_Events.CalculateTime(4000));
                break;
            case 8:
                _owner.SetReactState(REACT_AGGRESSIVE);
                _owner.SetImmuneToAll(false);
                if (InstanceScript* instance = _owner.GetInstanceScript())
                    instance->SetData(DATA_BATTERED_HILT, 8);
                break;
            case 9:
                _owner.AI()->Talk(SAY_BATTERED_HILT_OUTRO1);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, _eventId + 1), _owner.m_Events.CalculateTime(11000));
                break;
            case 10:
                _owner.AI()->Talk(SAY_BATTERED_HILT_OUTRO2);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, _eventId + 1), _owner.m_Events.CalculateTime(7500));
                break;
            case 11:
                _owner.AI()->Talk(SAY_BATTERED_HILT_OUTRO3);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, _eventId + 1), _owner.m_Events.CalculateTime(8000));
                break;
            case 12:
                _owner.AI()->Talk(SAY_BATTERED_HILT_OUTRO4);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, _eventId + 1), _owner.m_Events.CalculateTime(5000));
                break;
            case 13:
                _owner.CastSpell((Unit*)nullptr, 73036, true);
                _owner.m_Events.AddEvent(new UtherBatteredHiltEvent(_owner, _eventId + 1), _owner.m_Events.CalculateTime(3000));
                break;
            case 14:
                {
                    Position homePos = _owner.GetHomePosition();
                    _owner.SetReactState(REACT_PASSIVE);
                    _owner.SetImmuneToAll(true);
                    _owner.SetVisible(false);
                    _owner.UpdatePosition(homePos.GetPositionX(), homePos.GetPositionY(), homePos.GetPositionZ(), homePos.GetOrientation(), true);
                    _owner.StopMovingOnCurrentPos();
                    _owner.GetMotionMaster()->Clear();
                    if (InstanceScript* instance = _owner.GetInstanceScript())
                        instance->SetData(DATA_BATTERED_HILT, 9);
                }
                break;
        }
        return true;
    }

private:
    Creature& _owner;
    uint8 _eventId;
};

class instance_halls_of_reflection : public InstanceMapScript
{
public:
    instance_halls_of_reflection() : InstanceMapScript("instance_halls_of_reflection", MAP_HALLS_OF_REFLECTION) { }

    InstanceScript* GetInstanceScript(InstanceMap* pMap) const override
    {
        return new instance_halls_of_reflection_InstanceMapScript(pMap);
    }

    struct instance_halls_of_reflection_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_reflection_InstanceMapScript(Map* pMap) : InstanceScript(pMap)
        {
            SetHeaders(DataHeader);
        };

        uint32 EncounterMask;
        ObjectGuid NPC_FalricGUID;
        ObjectGuid NPC_MarwynGUID;
        ObjectGuid NPC_LichKingIntroGUID;
        ObjectGuid NPC_LeaderIntroGUID;
        ObjectGuid NPC_GuardGUID;
        ObjectGuid NPC_UtherGUID;
        ObjectGuid NPC_LichKingGUID;
        ObjectGuid NPC_LeaderGUID;
        ObjectGuid NPC_IceWallTargetGUID[4];
        ObjectGuid NPC_AltarBunnyGUID;
        ObjectGuid NPC_QuelDelarGUID;
        ObjectGuid NPC_ShipCaptainGUID;
        ObjectGuid GO_FrostmourneGUID;
        ObjectGuid GO_FrostmourneAltarGUID;
        ObjectGuid GO_FrontDoorGUID;
        ObjectGuid GO_ArthasDoorGUID;
        ObjectGuid GO_CaveInGUID;
        ObjectGuid GO_DoorBeforeThroneGUID;
        ObjectGuid GO_DoorAfterThroneGUID;
        ObjectGuid GO_IceWallGUID;

        ObjectGuid NPC_TrashGUID[NUM_OF_TRASH];
        bool TrashActive[NUM_OF_TRASH];
        uint8 TrashCounter;
        uint32 chosenComposition[8][5];
        uint8 WaveNumber;
        uint32 NextWaveTimer;
        uint16 CheckPlayersTimer;
        uint16 ResumeFirstEventTimer;
        uint8 ResumeFirstEventStep;
        bool bFinished5Waves;
        uint8 reqKillCount;
        bool IsDuringLKFight;
        uint32 BatteredHiltStatus;

        ObjectGuid NPC_FrostswornGeneralGUID;
        ObjectGuid NPC_SpiritualReflectionGUID[5];

        uint32 outroTimer;
        uint8 outroStep;
        MotionTransport* T1;

        void Initialize() override
        {
            EncounterMask = 0;
            memset(&TrashActive, 0, sizeof(TrashActive));
            TrashCounter = 0;
            memset(&chosenComposition, 0, sizeof(chosenComposition));
            WaveNumber = 0;
            NextWaveTimer = 0;
            CheckPlayersTimer = 5000;
            ResumeFirstEventTimer = 0;
            ResumeFirstEventStep = 0;
            bFinished5Waves = false;
            reqKillCount = 0;
            IsDuringLKFight = false;
            BatteredHiltStatus = 0;

            outroTimer = 0;
            outroStep = 0;
            T1 = nullptr;
        }

        bool IsEncounterInProgress() const override
        {
            return (instance->HavePlayers() && WaveNumber)  || IsDuringLKFight; // during LK fight npcs are active and will unset this variable
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_SYLVANAS_PART1:
                    creature->SetVisible(false);
                    creature->SetSpeed(MOVE_RUN, 1.1);
                    NPC_LeaderIntroGUID = creature->GetGUID();
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_JAINA_PART1);
                    creature->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    creature->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_DARK_RANGER_LORALEN:
                    creature->SetVisible(false);
                    NPC_GuardGUID = creature->GetGUID();
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_ARCHMAGE_KORELN);
                    break;
                case NPC_UTHER:
                    creature->SetVisible(false);
                    NPC_UtherGUID = creature->GetGUID();
                    creature->SetReactState(REACT_PASSIVE);
                    break;
                case NPC_LICH_KING_EVENT:
                    creature->SetVisible(false);
                    NPC_LichKingIntroGUID = creature->GetGUID();
                    creature->SetReactState(REACT_PASSIVE);
                    creature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_FALRIC:
                    creature->SetVisible(false);
                    NPC_FalricGUID = creature->GetGUID();
                    creature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_MARWYN:
                    creature->SetVisible(false);
                    NPC_MarwynGUID = creature->GetGUID();
                    creature->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_WAVE_MERCENARY:
                case NPC_WAVE_FOOTMAN:
                case NPC_WAVE_RIFLEMAN:
                case NPC_WAVE_PRIEST:
                case NPC_WAVE_MAGE:
                    if (TrashCounter < NUM_OF_TRASH)
                        NPC_TrashGUID[TrashCounter++] = creature->GetGUID();
                    if (!(EncounterMask & (1 << DATA_MARWYN)) && !creature->IsAlive())
                        creature->Respawn();
                    creature->SetVisible(false);
                    break;
                case NPC_FROSTSWORN_GENERAL:
                    if (!(EncounterMask & (1 << DATA_MARWYN)))
                    {
                        creature->SetVisible(false);
                        creature->SetReactState(REACT_PASSIVE);
                    }
                    NPC_FrostswornGeneralGUID = creature->GetGUID();
                    break;
                case NPC_SPIRITUAL_REFLECTION:
                    for (uint8 i = 0; i < 5; ++i)
                        if (!NPC_SpiritualReflectionGUID[i])
                        {
                            NPC_SpiritualReflectionGUID[i] = creature->GetGUID();
                            break;
                        }
                    creature->SetVisible(false);
                    break;
                case NPC_LICH_KING_BOSS:
                    if (!(EncounterMask & (1 << DATA_FROSTSWORN_GENERAL)))
                        creature->SetVisible(false);
                    if (!(EncounterMask & (1 << DATA_LK_INTRO)))
                    {
                        creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK2HTIGHT); //the fight cannot be in the form of an emote, it is causing bugs
                        creature->CastSpell(creature, SPELL_SOUL_REAPER, true);
                    }
                    else if (!(EncounterMask & (1 << DATA_LICH_KING)))
                        creature->AddAura(GetTeamIdInInstance() == TEAM_ALLIANCE ? SPELL_JAINA_ICE_PRISON : SPELL_SYLVANAS_DARK_BINDING, creature);
                    else
                        creature->SetVisible(false);

                    NPC_LichKingGUID = creature->GetGUID();
                    creature->SetHealth((creature->GetMaxHealth() * 3) / 4);
                    creature->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_SYLVANAS_PART2:
                    if (!creature->IsAlive())
                        creature->Respawn();
                    NPC_LeaderGUID = creature->GetGUID();
                    creature->SetWalk(false);
                    creature->SetSheath(SHEATH_STATE_MELEE);
                    if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                        creature->UpdateEntry(NPC_JAINA_PART2);
                    creature->SetWalk(false);
                    creature->SetHealth(creature->GetMaxHealth() / 20);
                    if (!(EncounterMask & (1 << DATA_FROSTSWORN_GENERAL)))
                        creature->SetVisible(false);
                    if (!(EncounterMask & (1 << DATA_LK_INTRO)))
                    {
                        creature->SetSheath(SHEATH_STATE_MELEE);
                        creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, GetTeamIdInInstance() == TEAM_ALLIANCE ? EMOTE_ONESHOT_ATTACK2HTIGHT : EMOTE_ONESHOT_ATTACK1H); //the fight cannot be in the form of an emote, it is causing bugs.
                        creature->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                        creature->CastSpell(creature, GetTeamIdInInstance() == TEAM_ALLIANCE ? SPELL_JAINA_ICE_BARRIER : SPELL_SYLVANAS_CLOAK_OF_DARKNESS, true);
                    }
                    else if (!(EncounterMask & (1 << DATA_LICH_KING)))
                    {
                        creature->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        creature->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        creature->UpdatePosition(LeaderEscapePos, true);
                        creature->StopMovingOnCurrentPos();
                    }
                    else
                    {
                        creature->UpdatePosition(PathWaypoints[PATH_WP_COUNT - 1], true);
                        creature->StopMovingOnCurrentPos();
                    }
                    creature->SetSheath(SHEATH_STATE_MELEE);
                    creature->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    break;
                case NPC_ICE_WALL_TARGET:
                    if (creature->GetPositionX() > 5525.0f)
                        NPC_IceWallTargetGUID[0] = creature->GetGUID();
                    else if (creature->GetPositionX() > 5475.0f)
                        NPC_IceWallTargetGUID[1] = creature->GetGUID();
                    else if (creature->GetPositionX() > 5400.0f)
                        NPC_IceWallTargetGUID[2] = creature->GetGUID();
                    else
                        NPC_IceWallTargetGUID[3] = creature->GetGUID();
                    break;
                case NPC_ALTAR_BUNNY:
                    NPC_AltarBunnyGUID = creature->GetGUID();
                    break;
                case NPC_QUEL_DELAR:
                    NPC_QuelDelarGUID = creature->GetGUID();
                    creature->SetReactState(REACT_PASSIVE);
                    break;
                case NPC_HIGH_CAPTAIN_JUSTIN_BARLETT:
                case NPC_SKY_REAVER_KORM_BLACKSKAR:
                    NPC_ShipCaptainGUID = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
                case GO_FROSTMOURNE:
                    GO_FrostmourneGUID = go->GetGUID();
                    HandleGameObject(ObjectGuid::Empty, false, go);
                    if (EncounterMask & (1 << DATA_INTRO))
                        go->SetPhaseMask(2, true);
                    break;
                case GO_FROSTMOURNE_ALTAR:
                    GO_FrostmourneAltarGUID = go->GetGUID();
                    break;
                case GO_FRONT_DOOR:
                    GO_FrontDoorGUID = go->GetGUID();
                    HandleGameObject(ObjectGuid::Empty, true, go);
                    break;
                case GO_ARTHAS_DOOR:
                    GO_ArthasDoorGUID = go->GetGUID();
                    HandleGameObject(ObjectGuid::Empty, (EncounterMask & (1 << DATA_MARWYN)), go);
                    break;
                case GO_CAVE_IN:
                    GO_CaveInGUID = go->GetGUID();
                    break;
                case GO_DOOR_BEFORE_THRONE:
                    GO_DoorBeforeThroneGUID = go->GetGUID();
                    break;
                case GO_DOOR_AFTER_THRONE:
                    GO_DoorAfterThroneGUID = go->GetGUID();
                    break;
                case GO_ICE_WALL:
                    GO_IceWallGUID = go->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_INTRO:
                    EncounterMask |= (1 << DATA_INTRO);
                    AddWave1();
                    break;
                case ACTION_SHOW_TRASH:
                    RandomizeCompositionsAndShow();
                    break;
                case DATA_FALRIC:
                    if (WaveNumber)
                    {
                        if (data == NOT_STARTED)
                            DoWipe1();
                        else if (data == DONE)
                        {
                            NextWaveTimer = 60000;
                            EncounterMask |= (1 << DATA_FALRIC);
                        }
                    }
                    break;
                case DATA_MARWYN:
                    if (WaveNumber)
                    {
                        if (data == NOT_STARTED)
                            DoWipe1();
                        else if (data == DONE)
                        {
                            EncounterMask |= (1 << DATA_MARWYN);
                            HandleGameObject(GO_FrontDoorGUID, true);
                            HandleGameObject(GO_ArthasDoorGUID, true);
                            if (Creature* c = instance->GetCreature(NPC_FrostswornGeneralGUID))
                            {
                                c->SetVisible(true);
                                c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                                c->SetReactState(REACT_AGGRESSIVE);
                            }
                            if (Creature* c = instance->GetCreature(NPC_FrostswornGeneralGUID))
                            {
                                c->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                            }
                            WaveNumber = 0;
                            DoUpdateWorldState(WORLD_STATE_HALLS_OF_REFLECTION_WAVES_ENABLED, 0);

                            // give quest
                            Map::PlayerList const& pl = instance->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                                if (Player* p = itr->GetSource())
                                    p->CastSpell(p, p->GetTeamId() == TEAM_ALLIANCE ? SPELL_HOR_START_QUEST_ALLY : SPELL_HOR_START_QUEST_HORDE, true);
                        }
                    }
                    break;
                case DATA_FROSTSWORN_GENERAL:
                    EncounterMask |= (1 << DATA_FROSTSWORN_GENERAL);
                    if (data == DONE)
                    {
                        if (Creature* c = instance->GetCreature(NPC_LichKingGUID))
                            c->SetVisible(true);
                        if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                            c->SetVisible(true);
                    }
                    break;
                case ACTION_SPIRITUAL_REFLECTIONS_COPY:
                    {
                        uint8 i = 0;
                        Map::PlayerList const& pl = instance->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                            if (Player* p = itr->GetSource())
                                if (p->IsAlive() && !p->IsGameMaster())
                                    if (Creature* c = instance->GetCreature(NPC_SpiritualReflectionGUID[i++]))
                                    {
                                        if (!c->IsAlive())
                                            c->Respawn();
                                        c->SetCanFly(true);
                                        c->SetDisableGravity(true);
                                        c->SetVisible(true);

                                        Item* i;
                                        i = p->GetWeaponForAttack(BASE_ATTACK);
                                        c->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, i ? i->GetEntry() : 0);
                                        i = p->GetWeaponForAttack(OFF_ATTACK);
                                        c->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, i ? i->GetEntry() : 0);
                                        i = p->GetWeaponForAttack(RANGED_ATTACK);
                                        c->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, i ? i->GetEntry() : 0);
                                        p->CastSpell(c, SPELL_HOR_CLONE, true);
                                        p->CastSpell(c, SPELL_HOR_CLONE_NAME, true);
                                    }
                    }
                    break;
                case ACTION_SPIRITUAL_REFLECTIONS_ACTIVATE:
                    if (Creature* fg = instance->GetCreature(NPC_FrostswornGeneralGUID))
                        for (uint8 i = 0; i < 5; ++i)
                            if (Creature* c = instance->GetCreature(NPC_SpiritualReflectionGUID[i]))
                                if (c->IsVisible())
                                {
                                    c->SetInCombatWithZone();
                                    c->SetCanFly(false);
                                    c->SetDisableGravity(false);
                                    c->GetMotionMaster()->MoveJump(fg->GetPositionX(), fg->GetPositionY(), fg->GetPositionZ(), 20.0f, 10.0f);
                                }
                    break;
                case ACTION_SPIRITUAL_REFLECTIONS_HIDE:
                    for (uint8 i = 0; i < 5; ++i)
                        if (Creature* c = instance->GetCreature(NPC_SpiritualReflectionGUID[i]))
                            c->AI()->EnterEvadeMode();
                    break;
                case DATA_LK_INTRO:
                    EncounterMask |= (1 << DATA_LK_INTRO);
                    if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                        c->AI()->DoAction(ACTION_START_INTRO);
                    break;
                case ACTION_START_LK_FIGHT:
                    IsDuringLKFight = true;
                    DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_RETREATING_TIMED_EVENT);
                    DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_RETREATING_TIMED_EVENT);
                    break;
                case ACTION_STOP_LK_FIGHT:
                    if (!IsDuringLKFight)
                    {
                        break;
                    }
                    if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                    {
                        if (!c->IsAlive())
                        {
                            c->Respawn();
                            if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                                c->UpdateEntry(NPC_JAINA_PART2);
                        }
                        c->GetThreatMgr().ClearAllThreat();
                        c->CombatStop(true);
                        c->InterruptNonMeleeSpells(true);
                        c->GetMotionMaster()->Clear();
                        c->GetMotionMaster()->MoveIdle();
                        c->UpdatePosition(LeaderEscapePos, true);
                        c->StopMovingOnCurrentPos();
                        c->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        c->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        c->SetHealth(c->GetMaxHealth() / 20);
                        c->AI()->Reset();
                        c->setActive(false);
                        c->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                    }
                    if (Creature* c = instance->GetCreature(NPC_LichKingGUID))
                    {
                        c->GetThreatMgr().ClearAllThreat();
                        c->CombatStop(true);
                        c->InterruptNonMeleeSpells(true);
                        c->GetMotionMaster()->Clear();
                        c->GetMotionMaster()->MoveIdle();
                        c->UpdatePosition(c->GetHomePosition(), true);
                        c->StopMovingOnCurrentPos();
                        c->RemoveAllAuras();
                        c->AddAura(GetTeamIdInInstance() == TEAM_ALLIANCE ? SPELL_JAINA_ICE_PRISON : SPELL_SYLVANAS_DARK_BINDING, c);
                        c->AI()->Reset();
                        c->setActive(false);
                        c->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                        c->SetSpeed(MOVE_RUN, c->GetCreatureTemplate()->speed_run);
                    }
                    IsDuringLKFight = false;
                    outroTimer = 0;
                    outroStep = 0;
                    [[fallthrough]];
                case ACTION_DELETE_ICE_WALL:
                    HandleGameObject(GO_IceWallGUID, true);
                    GO_IceWallGUID.Clear();
                    break;
                case DATA_LICH_KING:
                    if (data == DONE)
                    {
                        EncounterMask |= (1 << DATA_LICH_KING);
                        if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                            c->setActive(false);
                        if (Creature* c = instance->GetCreature(NPC_LichKingGUID))
                            c->setActive(false);
                        IsDuringLKFight = false;
                        outroStep = 1;
                        outroTimer = 0;
                    }
                    break;
                case DATA_BATTERED_HILT:
                    {
                        if (EncounterMask & (1 << DATA_BATTERED_HILT))
                            return;

                        switch (data)
                        {
                            case 1: // talked to leader
                                EncounterMask |= (1 << DATA_BATTERED_HILT);
                                SaveToDB();
                                break;
                            case 2:
                                if (BatteredHiltStatus)
                                    break;
                                BatteredHiltStatus |= BHSF_STARTED;
                                if (Creature* c = instance->GetCreature(NPC_AltarBunnyGUID))
                                    c->CastSpell(c, 70720, true);
                                if (Creature* c = instance->GetCreature(NPC_UtherGUID))
                                    c->m_Events.AddEvent(new UtherBatteredHiltEvent(*c, 1), c->m_Events.CalculateTime(3000));
                                break;
                            case 3:
                                if ((BatteredHiltStatus & BHSF_STARTED) == 0 || (BatteredHiltStatus & BHSF_THROWN))
                                    break;
                                BatteredHiltStatus |= BHSF_THROWN;
                                if (Creature* c = instance->GetCreature(NPC_UtherGUID))
                                {
                                    c->m_Events.AddEvent(new UtherBatteredHiltEvent(*c, 3), c->m_Events.CalculateTime(5500));
                                }
                                break;
                            case 4:
                                if (Creature* c = instance->GetCreature(NPC_QuelDelarGUID))
                                {
                                    c->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                                    c->SetSpeed(MOVE_RUN, 2.5f);
                                }
                                break;
                            case 5:
                                if (Creature* c = instance->GetCreature(NPC_UtherGUID))
                                    c->m_Events.AddEvent(new UtherBatteredHiltEvent(*c, 6), c->m_Events.CalculateTime(3000));
                                break;
                            case 6:
                                if (Creature* c = instance->GetCreature(NPC_QuelDelarGUID))
                                {
                                    c->SetSpeed(MOVE_RUN, c->GetCreatureTemplate()->speed_run);
                                    c->GetMotionMaster()->MoveLand(0, c->GetPositionX(), c->GetPositionY(), 707.70f, 7.0f);
                                }
                                break;
                            case 7:
                                if (Creature* c = instance->GetCreature(NPC_QuelDelarGUID))
                                {
                                    c->SetReactState(REACT_AGGRESSIVE);
                                    c->SetImmuneToAll(false);
                                    c->RemoveAurasDueToSpell(70300);
                                }
                                break;
                            case 8:
                                if (Creature* c = instance->GetCreature(NPC_QuelDelarGUID))
                                    c->SetInCombatWithZone();
                                break;
                            case 9:
                                EncounterMask |= (1 << DATA_BATTERED_HILT);
                                BatteredHiltStatus |= BHSF_FINISHED;
                                SaveToDB();
                                break;
                        }
                    }
                    return;
            }

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_INTRO:
                case DATA_FALRIC:
                case DATA_MARWYN:
                case DATA_FROSTSWORN_GENERAL:
                case DATA_LK_INTRO:
                case DATA_LICH_KING:
                    return EncounterMask & (1 << type);
                case DATA_WAVE_NUMBER:
                    return (uint32)WaveNumber;
                case DATA_BATTERED_HILT:
                    return BatteredHiltStatus;
            }

            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
                case NPC_DARK_RANGER_LORALEN:
                    return NPC_GuardGUID;
                case NPC_LICH_KING_EVENT:
                    return NPC_LichKingIntroGUID;
                case NPC_UTHER:
                    return NPC_UtherGUID;
                case DATA_FALRIC:
                    return NPC_FalricGUID;
                case DATA_MARWYN:
                    return NPC_MarwynGUID;
                case NPC_LICH_KING_BOSS:
                    return NPC_LichKingGUID;
                case NPC_SYLVANAS_PART2:
                    return NPC_LeaderGUID;
                case NPC_ICE_WALL_TARGET:
                case NPC_ICE_WALL_TARGET+1:
                case NPC_ICE_WALL_TARGET+2:
                case NPC_ICE_WALL_TARGET+3:
                    return NPC_IceWallTargetGUID[type - NPC_ICE_WALL_TARGET];
                case GO_FROSTMOURNE:
                    return GO_FrostmourneGUID;
                case GO_ARTHAS_DOOR:
                    return GO_ArthasDoorGUID;
                case GO_FRONT_DOOR:
                    return GO_FrontDoorGUID;
            }
            return ObjectGuid::Empty;
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            data >> EncounterMask;
            BatteredHiltStatus = (EncounterMask & (1 << DATA_BATTERED_HILT)) ? BHSF_FINISHED : BHSF_NONE;
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << EncounterMask;
        }

        void OnUnitDeath(Unit* unit) override
        {
            if (WaveNumber && reqKillCount)
                if (unit->GetEntry() == NPC_WAVE_MERCENARY || unit->GetEntry() == NPC_WAVE_FOOTMAN || unit->GetEntry() == NPC_WAVE_RIFLEMAN || unit->GetEntry() == NPC_WAVE_PRIEST || unit->GetEntry() == NPC_WAVE_MAGE)
                    if ((--reqKillCount) == 0 && WaveNumber % 5 && NextWaveTimer > 5000)
                        NextWaveTimer = 5000;

            if (unit->GetEntry() == NPC_QUEL_DELAR)
                if (Creature* c = instance->GetCreature(NPC_UtherGUID))
                {
                    c->SetStandState(UNIT_STAND_STATE_STAND);
                    c->SetWalk(false);
                    c->GetMotionMaster()->MovePoint(0, 5313.92f, 1989.36f, 707.70f);
                    c->m_Events.AddEvent(new UtherBatteredHiltEvent(*c, 9), c->m_Events.CalculateTime(7000));
                }
        }

        void RandomizeCompositionsAndShow()
        {
            uint8 r1 = urand(0, 1), r2 = urand(2, 3);
            for (uint8 i = 0; i < 5; ++i)
            {
                chosenComposition[0][i] = allowedCompositions[r1][i];
                chosenComposition[1][i] = allowedCompositions[r1 == 0 ? 1 : 0][i];
                chosenComposition[2][i] = allowedCompositions[r2][i];
                chosenComposition[3][i] = allowedCompositions[r2 == 2 ? 3 : 2][i];
            }
            bool left[4] = {true, true, true, true};
            for (uint8 k = 4; k > 0; --k)
            {
                uint8 r = urand(0, k - 1);
                uint8 ur = 0;
                for (uint8 j = 0; j < 4; ++j)
                    if (left[j])
                    {
                        if (ur == r)
                        {
                            left[j] = false;
                            for (uint8 i = 0; i < 5; ++i)
                                chosenComposition[8 - k][i] = allowedCompositions[j + 4][i];
                            break;
                        }
                        ++ur;
                    }
            }
            if (bFinished5Waves)
            {
                for (; WaveNumber < 4; ++WaveNumber)
                {
                    uint8 num_to_activate;
                    if (WaveNumber <= 1)
                        num_to_activate = 3;
                    else
                        num_to_activate = 4;

                    for (uint8 i = 0; i < num_to_activate; ++i)
                    {
                        uint32 entry = chosenComposition[WaveNumber][i];
                        bool forward = !!urand(0, 1);
                        for (int8 j = (forward ? 0 : NUM_OF_TRASH - 1); (forward ? j<NUM_OF_TRASH : j >= 0); (forward ? ++j : --j))
                            if (!TrashActive[j])
                                if (Creature* c = instance->GetCreature(NPC_TrashGUID[j]))
                                    if (c->GetEntry() == entry)
                                    {
                                        TrashActive[j] = true;
                                        Unit::Kill(c, c);
                                        break;
                                    }
                    }
                }
                WaveNumber = 5;
            }

            for (uint8 i = 0; i < NUM_OF_TRASH; ++i)
                if (!TrashActive[i])
                    if (Creature* c = instance->GetCreature(NPC_TrashGUID[i]))
                    {
                        c->SetVisible(true);
                        c->CastSpell(c, SPELL_WELL_OF_SOULS_VISUAL, false);
                    }
        }

        void AddWave1()
        {
            if (WaveNumber >= 10)
                return;

            ++WaveNumber;
            if (WaveNumber >= 6)
                bFinished5Waves = true;

            DoUpdateWorldState(WORLD_STATE_HALLS_OF_REFLECTION_WAVES_ENABLED, 1);
            DoUpdateWorldState(WORLD_STATE_HALLS_OF_REFLECTION_WAVE_COUNT, WaveNumber);
            HandleGameObject(GO_FrontDoorGUID, false);

            // some of them could go back to spawn due to vanish, etc.
            // on activating next wave make those attack again
            for (uint8 i = 0; i < NUM_OF_TRASH; ++i)
                if (TrashActive[i])
                    if (Creature* c = instance->GetCreature(NPC_TrashGUID[i]))
                        if (c->IsAlive() && !c->IsInCombat())
                            c->AI()->DoAction(1);

            if (WaveNumber == 5 || WaveNumber == 10)
            {
                NextWaveTimer = 0;
                if (WaveNumber == 5)
                {
                    if (Creature* falric = instance->GetCreature(NPC_FalricGUID))
                    {
                        if (falric->IsAlive())
                            falric->AI()->DoAction(1);
                        else
                            NextWaveTimer = 1;
                    }
                }
                else
                {
                    if (Creature* marwyn = instance->GetCreature(NPC_MarwynGUID))
                        if (marwyn->IsAlive()) // should always be true, but just in case
                            marwyn->AI()->DoAction(1);
                }
            }
            else
            {
                NextWaveTimer = 150000;

                uint8 num_to_activate = 5;
                if (WaveNumber <= 2)
                    num_to_activate = 3;
                else if (WaveNumber <= 4)
                    num_to_activate = 4;
                reqKillCount += num_to_activate;
                for (uint8 i = 0; i < num_to_activate; ++i)
                {
                    uint32 entry = chosenComposition[WaveNumber - (WaveNumber > 5 ? 2 : 1)][i];
                    bool forward = !!urand(0, 1);
                    for (int8 j = (forward ? 0 : NUM_OF_TRASH - 1); (forward ? j<NUM_OF_TRASH : j >= 0); (forward ? ++j : --j))
                        if (!TrashActive[j])
                            if (Creature* c = instance->GetCreature(NPC_TrashGUID[j]))
                                if (c->GetEntry() == entry)
                                {
                                    TrashActive[j] = true;
                                    c->RemoveUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
                                    c->SetImmuneToAll(false);
                                    c->AI()->DoAction(1);
                                    break;
                                }
                }
            }
        }

        void DoWipe1()
        {
            if (!WaveNumber)
                return;

            DoUpdateWorldState(WORLD_STATE_HALLS_OF_REFLECTION_WAVES_ENABLED, 0);
            DoUpdateWorldState(WORLD_STATE_HALLS_OF_REFLECTION_WAVE_COUNT, 0);
            HandleGameObject(GO_FrontDoorGUID, true);

            TrashCounter = NUM_OF_TRASH;
            WaveNumber = 0;
            NextWaveTimer = 0;
            memset(&chosenComposition, 0, sizeof(chosenComposition));

            for (uint8 i = 0; i < NUM_OF_TRASH; ++i)
                if (TrashActive[i])
                    if (Creature* c = instance->GetCreature(NPC_TrashGUID[i]))
                    {
                        c->GetThreatMgr().ClearAllThreat();
                        c->CombatStop(true);
                        c->InterruptNonMeleeSpells(true);
                        c->SetUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
                        c->SetImmuneToAll(true);
                        c->Respawn(true);
                        c->UpdatePosition(c->GetHomePosition(), true);
                        c->StopMovingOnCurrentPos();
                    }
            memset(&TrashActive, 0, sizeof(TrashActive));
            if (Creature* falric = instance->GetCreature(NPC_FalricGUID))
                falric->AI()->EnterEvadeMode();
            if (Creature* marwyn = instance->GetCreature(NPC_MarwynGUID))
                marwyn->AI()->EnterEvadeMode();

            ResumeFirstEventTimer = 5000;
            ResumeFirstEventStep = 2;
            reqKillCount = 0;
        }

        void Update(uint32 diff) override
        {
            if (!instance->HavePlayers())
                return;

            if (CheckPlayersTimer <= diff)
            {
                CheckPlayersTimer = 5000;
                if ((EncounterMask & (1 << DATA_INTRO)) && !(EncounterMask & (1 << DATA_MARWYN))) // first event
                {
                    Map::PlayerList const& pl = instance->GetPlayers();
                    if (WaveNumber || NextWaveTimer)
                    {
                        bool allDead = true;
                        bool outOfRange = false;
                        for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                            if (Player* p = itr->GetSource())
                            {
                                if (p->IsGameMaster())
                                    continue;
                                if (p->IsAlive())
                                    allDead = false;
                                if (p->GetExactDist2d(&CenterPos) > MAX_DIST_FROM_CENTER_IN_COMBAT)
                                {
                                    outOfRange = true;
                                    break;
                                }
                            }
                        if (allDead || outOfRange)
                            DoWipe1();
                    }
                    else if (!ResumeFirstEventTimer)
                    {
                        bool allInRangeAndAlive = (instance->GetPlayersCountExceptGMs() > 0);
                        for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
                            if (Player* p = itr->GetSource())
                                if (!p->IsGameMaster() && (p->GetExactDist2d(&CenterPos) > MAX_DIST_FROM_CENTER_TO_START || !p->IsAlive()))
                                {
                                    allInRangeAndAlive = false;
                                    break;
                                }
                        if (allInRangeAndAlive)
                        {
                            ResumeFirstEventTimer = 1;
                            ResumeFirstEventStep = 0;
                        }
                    }
                }
            }
            else
                CheckPlayersTimer -= diff;

            if (NextWaveTimer)
            {
                if (NextWaveTimer <= diff)
                {
                    NextWaveTimer = 0;
                    AddWave1();
                }
                else
                    NextWaveTimer -= diff;
            }

            if (ResumeFirstEventTimer)
            {
                if (ResumeFirstEventTimer <= diff)
                {
                    switch (ResumeFirstEventStep) // After a wipe
                    {
                        case 0:
                            if (Creature* pFalric = instance->GetCreature(NPC_FalricGUID))
                            {
                                pFalric->UpdatePosition(5274.9f, 2039.2f, 709.319f, 5.4619f, true);
                                pFalric->StopMovingOnCurrentPos();
                                pFalric->SetVisible(true);
                                if (pFalric->IsAlive())
                                {
                                    pFalric->GetMotionMaster()->MovePoint(0, FalricMovePos);
                                    if (Aura* a = pFalric->AddAura(SPELL_SHADOWMOURNE_VISUAL, pFalric))
                                        a->SetDuration(8000);
                                }
                            }
                            if (Creature* pMarwyn = instance->GetCreature(NPC_MarwynGUID))
                            {
                                pMarwyn->UpdatePosition(5343.77f, 1973.86f, 709.319f, 2.35173f, true);
                                pMarwyn->StopMovingOnCurrentPos();
                                pMarwyn->SetVisible(true);
                                if (pMarwyn->IsAlive())
                                {
                                    pMarwyn->GetMotionMaster()->MovePoint(0, MarwynMovePos);
                                    if (Aura* a = pMarwyn->AddAura(SPELL_SHADOWMOURNE_VISUAL, pMarwyn))
                                        a->SetDuration(8000);
                                }
                                pMarwyn->AI()->Talk(EMOTE_MARWYN_INTRO_SPIRIT);
                            }
                            ++ResumeFirstEventStep;
                            ResumeFirstEventTimer = 7500;
                            break;
                        case 1:
                            if (Creature* pFalric = instance->GetCreature(NPC_FalricGUID))
                            {
                                if (pFalric->IsAlive())
                                {
                                    pFalric->AI()->Talk(SAY_FALRIC_INTRO_2); // Between wave 1 and 4
                                }
                                else
                                {
                                    if (Creature* marwyn = instance->GetCreature(NPC_MarwynGUID))
                                    {
                                        marwyn->AI()->Talk(SAY_MARWYN_WIPE_AFTER_FALRIC); // Between wave 6 and 9
                                    }
                                }
                            }
                            SetData(ACTION_SHOW_TRASH, 1);
                            ResumeFirstEventStep = 0;
                            ResumeFirstEventTimer = 0;
                            NextWaveTimer = 7000;
                            break;
                        default:
                            for (uint8 i = 0; i < NUM_OF_TRASH; ++i)
                                if (Creature* c = instance->GetCreature(NPC_TrashGUID[i]))
                                    c->SetVisible(false);
                            if (Creature* falric = instance->GetCreature(NPC_FalricGUID))
                                falric->SetVisible(false);
                            if (Creature* marwyn = instance->GetCreature(NPC_MarwynGUID))
                                marwyn->SetVisible(false);
                            ResumeFirstEventStep = 0;
                            ResumeFirstEventTimer = 0;
                            break;
                    }
                }
                else
                    ResumeFirstEventTimer -= diff;
            }
            if (outroStep)
            {
                if (outroTimer <= diff)
                {
                    switch (outroStep)
                    {
                        case 1:
                            if (Creature* lk = instance->GetCreature(NPC_LichKingGUID))
                            {
                                lk->UpdatePosition(PathWaypoints[PATH_WP_COUNT - 2], true);
                                lk->StopMovingOnCurrentPos();
                                lk->RemoveAllAuras();
                                lk->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_CUSTOM_SPELL_02);
                                if (!lk->IsVisible())
                                    lk->SetVisible(true);
                                if (Creature* leader = instance->GetCreature(NPC_LeaderGUID))
                                {
                                    leader->UpdatePosition(PathWaypoints[PATH_WP_COUNT - 1], true);
                                    leader->StopMovingOnCurrentPos();
                                    leader->RemoveAllAuras();
                                    leader->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_READY1H);
                                    if (!leader->IsVisible())
                                        leader->SetVisible(true);
                                    lk->CastSpell(leader, SPELL_HARVEST_SOUL, false);
                                }
                            }
                            ++outroStep;
                            outroTimer = 500;
                            break;
                        case 2:
                            {
                                uint32 entry = GetTeamIdInInstance() == TEAM_ALLIANCE ? GO_THE_SKYBREAKER : GO_ORGRIMS_HAMMER;
                                T1 = sTransportMgr->CreateTransport(entry, 0, instance);

                                ++outroStep;
                                outroTimer = GetTeamIdInInstance() == TEAM_ALLIANCE ? 10000 : 10500;
                            }
                            break;
                        case 3:
                            if (T1)
                                T1->EnableMovement(false);
                            if (Creature* c = instance->GetCreature(NPC_ShipCaptainGUID))
                                c->AI()->Talk(GetTeamIdInInstance() == TEAM_ALLIANCE ? SAY_FIRE_ALLY : SAY_FIRE_HORDE);
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                            {
                                c->RemoveAllAuras();
                                c->CastSpell(c, SPELL_GUNSHIP_CANNON_FIRE_PERIODIC, true);
                            }
                            if (Creature* c = instance->GetCreature(NPC_LichKingGUID))
                            {
                                c->InterruptNonMeleeSpells(true);
                                c->RemoveAllAuras();
                            }
                            ++outroStep;
                            outroTimer = 5000;
                            break;
                        case 4:
                            HandleGameObject(GO_CaveInGUID, false);
                            ++outroStep;
                            outroTimer = 3000;
                            break;
                        case 5:
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                                c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            if (Creature* c = instance->GetCreature(NPC_LichKingGUID))
                            {
                                c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                                c->GetThreatMgr().ClearAllThreat();
                                c->CombatStop(true);
                                c->InterruptNonMeleeSpells(true);
                                c->SetVisible(false);
                            }
                            if (instance->IsHeroic())
                                instance->ToInstanceMap()->PermBindAllPlayers();
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                                c->CastSpell(c, SPELL_ACHIEVEMENT_CHECK, true);
                            ++outroStep;
                            outroTimer = 1000;
                            break;
                        case 6:
                            if (T1)
                                T1->EnableMovement(true);
                            ++outroStep;
                            outroTimer = 3500;
                            break;
                        case 7:
                            if (T1)
                                T1->EnableMovement(false);
                            if (Creature* leader = instance->GetCreature(NPC_LeaderGUID))
                            {
                                uint8 index = GetTeamIdInInstance() == TEAM_ALLIANCE ? 0 : 1;
                                for (uint8 i = 0; i < 3; ++i)
                                    if (StairsPos[index][i].GetPositionX())
                                        if (GameObject* go = leader->SummonGameObject(GetTeamIdInInstance() == TEAM_ALLIANCE ? GO_STAIRS_ALLIANCE : GO_STAIRS_HORDE, StairsPos[index][i].GetPositionX(), StairsPos[index][i].GetPositionY(), StairsPos[index][i].GetPositionZ(), StairsPos[index][i].GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 86400, false))
                                            go->SetGameObjectFlag(GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                //Position pos = TeamIdInInstance == TEAM_ALLIANCE ? AllyPortalPos : HordePortalPos;
                                //leader->SummonGameObject(GO_PORTAL_TO_DALARAN, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 86400);
                                //pos = TeamIdInInstance == TEAM_ALLIANCE ? AllyChestPos : HordeChestPos;
                                //leader->SummonGameObject(instance->GetSpawnMode() ? GO_CHEST_HEROIC : GO_CHEST_NORMAL, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 86400);
                            }
                            ++outroStep;
                            outroTimer = 1000;
                            break;
                        case 8:
                            if (Creature* c = instance->GetCreature(NPC_ShipCaptainGUID))
                                c->AI()->Talk(GetTeamIdInInstance() == TEAM_ALLIANCE ? SAY_ONBOARD_ALLY : SAY_ONBOARD_HORDE);
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                            {
                                c->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                                c->GetMotionMaster()->MovePoint(0, WalkCaveInPos);
                                c->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP); // need gossip ID 10931
                            }
                            ++outroStep;
                            outroTimer = 6000;
                            break;
                        case 9:
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                                c->AI()->Talk(GetTeamIdInInstance() == TEAM_ALLIANCE ? SAY_JAINA_FINAL_1 : SAY_SYLVANA_FINAL);
                            HandleGameObject(GO_CaveInGUID, true);
                            ++outroStep;
                            outroTimer = 11000;
                            break;
                        case 10:
                            ++outroStep;
                            outroTimer = 0;
                            for (Map::PlayerList::const_iterator itr = instance->GetPlayers().begin(); itr != instance->GetPlayers().end(); ++itr)
                                if (Player* p = itr->GetSource())
                                    p->KilledMonsterCredit(NPC_WRATH_OF_THE_LICH_KING_CREDIT);
                            if (GetTeamIdInInstance() == TEAM_ALLIANCE)
                                if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                                {
                                    c->AI()->Talk(SAY_JAINA_FINAL_2);
                                    outroTimer = 10000;
                                }
                            break;
                        case 11:
                            if (Creature* c = instance->GetCreature(NPC_LeaderGUID))
                                c->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                            ++outroStep;
                            outroTimer = 300 * 1000;
                            break;
                        case 12:
                            outroStep = 0;
                            outroTimer = 0;
                            if (T1)
                                T1->setActive(false);
                            break;
                    }
                }
                else
                    outroTimer -= diff;
            }
        }
    };
};

void AddSC_instance_halls_of_reflection()
{
    new instance_halls_of_reflection();
}
