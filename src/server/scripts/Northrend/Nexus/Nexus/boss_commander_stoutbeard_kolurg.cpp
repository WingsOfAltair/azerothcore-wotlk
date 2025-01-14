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

#include "CreatureScript.h"
#include "ScriptedCreature.h"
#include "nexus.h"

enum Spells
{
    SPELL_BATTLE_SHOUT              = 31403,
    SPELL_CHARGE                    = 60067,
    SPELL_FRIGHTENING_SHOUT         = 19134,
    SPELL_WHIRLWIND                 = 38618
};

enum Events
{
    EVENT_BATTLE_SHOUT              = 1,
    EVENT_FRIGHTENING_SHOUT         = 2,
    EVENT_WHIRLWIND                 = 3,
    EVENT_COMMANDER_CHARGE          = 4,
    EVENT_KILL_TALK                 = 5
};

enum Says
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2
};

struct boss_commander_stoutbeard : public BossAI
{
    boss_commander_stoutbeard(Creature* creature) : BossAI(creature, DATA_COMMANDER_EVENT) {}

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(SAY_AGGRO);

        events.ScheduleEvent(EVENT_BATTLE_SHOUT, 0ms);
        events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, 10s);
        events.ScheduleEvent(EVENT_WHIRLWIND, 15s);
        events.ScheduleEvent(EVENT_COMMANDER_CHARGE, 1s);
        me->RemoveAllAuras();
    }

    void KilledUnit(Unit*) override
    {
        if (events.GetNextEventTime(EVENT_KILL_TALK) == 0)
        {
            Talk(SAY_KILL);
            events.ScheduleEvent(EVENT_KILL_TALK, 6s);
        }
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_BATTLE_SHOUT:
            me->CastSpell(me, SPELL_BATTLE_SHOUT, true);
            events.ScheduleEvent(EVENT_BATTLE_SHOUT, 2min);
            break;
        case EVENT_FRIGHTENING_SHOUT:
            me->CastSpell(me->GetVictim(), SPELL_FRIGHTENING_SHOUT, false);
            events.ScheduleEvent(EVENT_FRIGHTENING_SHOUT, 15s, 20s);
            break;
        case EVENT_WHIRLWIND:
            me->CastSpell(me, SPELL_WHIRLWIND, false);
            events.ScheduleEvent(EVENT_WHIRLWIND, 16s);
            break;
        case EVENT_COMMANDER_CHARGE:
            if (Unit* target = SelectTarget(SelectTargetMethod::MinDistance, 0, 25.0f))
                me->CastSpell(target, SPELL_CHARGE, false);
            events.ScheduleEvent(EVENT_COMMANDER_CHARGE, 20s);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_commander_stoutbeard()
{
    RegisterNexusCreatureAI(boss_commander_stoutbeard);
}
