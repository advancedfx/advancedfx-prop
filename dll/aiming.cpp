
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
#include "register.h"

#include "aiming.h"
#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

REGISTER_CVAR(aim_oneway, "0", 0);
REGISTER_CVAR(aim_snapto, "1", 0);
REGISTER_CVAR(aim_lingertime, "50", 0);
REGISTER_CVAR(aim_onlyvisible, "0", 0);

// Our aiming singleton
Aiming g_Aiming;

void AnglesFromTo(Vector &from, Vector &to, Vector &angles);
float TraceLine(cl_entity_t *target);
float clamp(float i, float min, float max);

#define PI 3.14159265f

void Aiming::addAimLayer(int iSlot, int iEnt)
{
	std::list<int>::iterator iter = m_AimLayers.begin();
	int i = 0;

	// Shift the iterator up to the right position
	while (i < iSlot && iter != m_AimLayers.end())
	{
		i++;
		iter++;
	}

	m_AimLayers.insert(iter, iEnt);
	pEngfuncs->Con_Printf("Added entity %d at slot %d\n", iEnt, i);
}

void Aiming::removeAimLayer(int iSlot)
{
	std::list<int>::iterator iter = m_AimLayers.begin();
	int i = 0;

	// Shift the iterator up to the right position
	while (i < iSlot && iter != m_AimLayers.end())
	{
		i++;
		iter++;
	}

	if (iter == m_AimLayers.end())
	{
		pEngfuncs->Con_Printf("No such slot\n");
		return;
	}
	
	m_AimLayers.erase(iter);
	pEngfuncs->Con_Printf("Removed entity at slot %d\n", i);
}

void Aiming::showAimLayers()
{
	int i = 0;
	for (std::list<int>::iterator iter = m_AimLayers.begin(); iter != m_AimLayers.end(); iter++)
		pEngfuncs->Con_Printf(" %02d. Ent #%d\n", i++, *iter);
}

void Aiming::Start()
{
	m_iHighestSlot = 9999;
	m_bActive = true;

	memset(m_ActiveTimes, 0, sizeof(m_ActiveTimes));
	memset(m_VisibleTimes, 0, sizeof(m_VisibleTimes));
	memset(m_LastMsgNums, 0, sizeof(m_LastMsgNums));
	memset(m_EntityStates, 0, sizeof(m_EntityStates));

	// Loop through all entities in the list and record their initial state
	// This fixes the fact that we aim at inactive things briefly at first
	for (std::list<int>::iterator iter = m_AimLayers.begin(); iter != m_AimLayers.end(); iter++)
	{
		int i = (*iter);

		// Point to the right entity
		cl_entity_t *them = pEngfuncs->GetEntityByIndex(i);

		// If they exist them take the actual values
		if (them)
			m_LastMsgNums[i] = them->curstate.messagenum;
	}
}

void Aiming::Stop()
{
	m_bActive = false;
}

void Aiming::LookAtCurrentEntity()
{
	cl_entity_t *them = pEngfuncs->GetEntityByIndex(m_iCurrentEntity);

	if (them)
	{
		Vector angles;

		AnglesFromTo(ppmove->origin, them->origin, angles);

		float cangles[3];

		pEngfuncs->GetViewAngles(cangles);
		cangles[0] = angles.y;
		cangles[1] = angles.x;
		pEngfuncs->SetViewAngles(cangles);
	}
}

bool Aiming::getValidTarget(Vector &outTarget)
{
	cl_entity_t *them = NULL;

	bool bActive, bVisible, bLowerPriority, bFirstEntity = true;
	float vis;
	int msg;

	// Some booleans
	bool bOneWay = !(aim_oneway->value == 0);
	bool bViewOnly = !(aim_onlyvisible->value == 0);

	// The time we linger before we move on!
	int iLingerTime = (int) aim_lingertime->value;

	// Now this is code to get the right entity to aim at!
	std::list<int>::iterator iter = m_AimLayers.begin();

	int slot_position = 0;

	// Loop through all entities in the list
	for (std::list<int>::iterator iter = m_AimLayers.begin(); iter != m_AimLayers.end(); iter++)
	{
		// What we need
		//	If the entity meets our targetting criteria (vis/active) we store the
        //  coords for it, which is what gets returned.
		//  When the entity starts failing, we keep returning the last updated
		//  coords until the linger time runs out, after which we just return false
		//  and the cameraman can do whatever

		// Reset the state of this entity
		bActive = bVisible = bLowerPriority = false;

		int i = (*iter);

		// Point to the right entity
		them = pEngfuncs->GetEntityByIndex(i);

		// If they exist them take the actual values
		if (them)
		{
			vis = TraceLine(them);
			msg = them->curstate.messagenum;
		}
		// Otherwise use their last known message and assume they're not visible
		else
		{
			vis = 0;
			msg = m_LastMsgNums[i];
		}

		// And is this up or down the list?
		bLowerPriority = (slot_position > m_iHighestSlot);

		// This entity was active within the last few frames
		// m_EntityStates[i] == ES_DEAD means that the entity has died since it has been tracked
		if (m_EntityStates[i] != ES_DEAD && (m_LastMsgNums[i] != msg || m_ActiveTimes[i] > 0))
		{
			if (m_LastMsgNums[i] != msg)
				m_ActiveTimes[i] = 5;

			// We don't care about whether it is visible, so assume it is
			if (!bViewOnly)
			{
				m_LastPositions[i] = them->origin;
				m_VisibleTimes[i] = iLingerTime;
			}
			// Actually we only look at visible entities
			else if (bViewOnly && (vis == 1.0f || m_VisibleTimes[i] > 0))
			{
				if (vis == 1.0f)
					m_VisibleTimes[i] = iLingerTime;

				m_LastPositions[i] = them->origin;
			}
		}
		// This entity is not active
		else
		{
			// If a previously targetted entity has died then don't retarget it
			if (m_EntityStates[i] == ES_TARGET)
				m_EntityStates[i] = ES_DEAD;
		}

		// By now if m_VisibleTimes[i] > 0 then we will have a location which we want to
		// aim at, calculated from either this frame or a previous one
		if (m_VisibleTimes[i] > 0)
		{
			// It's the first entity we've searched for and we're allowed to pick it
			if (bFirstEntity && (!bOneWay || !bLowerPriority))
			{
				outTarget = m_LastPositions[i];
				bFirstEntity = false;

				m_EntityStates[i] = ES_TARGET;
			}

			m_VisibleTimes[i]--;
		}

		// We just need to decrement this however
		if (m_ActiveTimes[i] > 0)
			m_ActiveTimes[i]--;

		// Now we just update their values
		m_LastMsgNums[i] = msg;

		slot_position++;
	}
	
	// Return whether we found an entity
	return !bFirstEntity;
}

void Aiming::aim()
{
	const float flRealAimMaxSpeed = 3.0f;
	const float flRealAimAccel = 20.0f;
	const float flRealAimDeaccel = 10.0f;

	Vector target(0, 0, 0);

	// Nothing to aim at here
	if (!getValidTarget(target))
		return;

	Vector idealaim, reaim;
	AnglesFromTo(ppmove->origin, /*them->origin*/ target, idealaim);

	// Are we using snapto or nice aiming
	if (aim_snapto->value != 0)
	{
		// Simple aim
		float cangles[3];
		pEngfuncs->GetViewAngles(cangles);
		cangles[0] = idealaim.y;
		cangles[1] = idealaim.x;
		pEngfuncs->SetViewAngles(cangles);
	}
	else
	{
		float angles[3];
		pEngfuncs->GetViewAngles(angles);

		reaim.x = idealaim.x - angles[1];
		reaim.y = idealaim.y - angles[0];

		// For when angles are on the 359..0 crossover
		if (reaim.x > 180.0f)
			reaim.x -= 360.0f;
		else if (reaim.x < -180.0f)
			reaim.x += 360.0f;

		float reaim_distance = reaim.Length();

		// Lets just get it working first
		if (reaim_distance > 0)
		{	
			// Are we speeding up or slowing down
			if (reaim_distance > flRealAimAccel)
			{
				m_flRealAimSpeed += flRealAimMaxSpeed / flRealAimAccel;
			}
			else
			{
				// Max speed depends on the distance
				m_flRealAimSpeed = clamp(m_flRealAimSpeed, 0.5f, (flRealAimMaxSpeed * (reaim_distance / flRealAimAccel)));
			}

			// Make sure we can't go above the max speed
			m_flRealAimSpeed = clamp(m_flRealAimSpeed, 0, flRealAimMaxSpeed);

			// Get the percentage of the distance we can go clamped by max speed
			float pc = clamp(reaim_distance, -m_flRealAimSpeed, m_flRealAimSpeed) / reaim_distance;

			// Resize all the components
			reaim = reaim * pc;

			// Now shift our view
			angles[1] += reaim.x;
			angles[0] += reaim.y;

			pEngfuncs->SetViewAngles(angles);
		}
		else
		{
			m_flRealAimSpeed -= flRealAimMaxSpeed / flRealAimDeaccel;
			m_flRealAimSpeed = clamp(m_flRealAimSpeed, 0, flRealAimMaxSpeed);
		}
	}
}

void AnglesFromTo(Vector &from, Vector &to, Vector &angles)
{
	Vector dir = to - from;

	// Store then zero height
	float dz = dir.z;
	
	dir.z = 0;

	// Need this for later
	float length = dir.Length();

	dir = dir.Normalize();

	// This is our forward angle
	Vector vForward(1.0f, 0.0f, 0.0f);

	float dot_product = (dir.x * vForward.x) + (dir.y * vForward.y) + (dir.z * vForward.z);

	float angle = acos(dot_product) * 180.0f / PI;

	if (dir.y < 0)
		angle = 360.0f - angle;

	// This is our pitchup/down
	if (length == 0)
		length = 0.01f;

	float pitch = atan(dz / length) * 180.0f / PI;

	angles.x = angle;
	angles.y = -pitch;
}

float TraceLine(cl_entity_t *target)
{
	// Get coords of entity and us
	float ppmove_origin[3], target_origin[3];

	ppmove->origin.CopyToArray(ppmove_origin);
	target->origin.CopyToArray(target_origin);

	pmtrace_t *tr;
	tr = pEngfuncs->PM_TraceLine(ppmove_origin, target_origin, PM_TRACELINE_ANYVISIBLE, 2, -1); //target->index /*shouldnt matter with studio ignore*/);

	return tr->fraction;
}

float clamp(float i, float min, float max)
{
	if (i < min)
		return min;
	if (i > max)
		return max;
	else
		return i;
}

REGISTER_CMD_FUNC(entity_lookat)
{
	g_Aiming.LookAtCurrentEntity();
}

REGISTER_CMD_FUNC(entity_next)
{
	g_Aiming.nextEntity();
	g_Aiming.LookAtCurrentEntity();
}

REGISTER_CMD_FUNC(entity_prev)
{
	g_Aiming.prevEntity();
	g_Aiming.LookAtCurrentEntity();
}

REGISTER_CMD_FUNC(entity_jump)
{
	if (pEngfuncs->Cmd_Argc() != 2)
	{
		pEngfuncs->Con_Printf("Useage: " PREFIX "entity_jump <entnum>\n");
		return;
	}

	g_Aiming.setEntity(atoi(pEngfuncs->Cmd_Argv(1)));
	g_Aiming.LookAtCurrentEntity();
}

REGISTER_CMD_FUNC_BEGIN(aim)
{
	g_Aiming.Start();
}

REGISTER_CMD_FUNC_END(aim)
{
	g_Aiming.Start();
}

REGISTER_CMD_FUNC(aim_start)
{
	CALL_CMD_BEGIN(aim);
}

REGISTER_CMD_FUNC(aim_stop)
{
	CALL_CMD_END(aim);
}

REGISTER_CMD_FUNC(addaimlayer)
{
	if (pEngfuncs->Cmd_Argc() != 3)
	{
		pEngfuncs->Con_Printf("Usage: " PREFIX "addaimentity <slot> <entid>\n");
		return;
	}

	int slot = atoi(pEngfuncs->Cmd_Argv(1));
	int ent = atoi(pEngfuncs->Cmd_Argv(2));

	g_Aiming.addAimLayer(slot, ent);
}

REGISTER_CMD_FUNC(delaimlayer)
{
	if (pEngfuncs->Cmd_Argc() != 2)
	{
		pEngfuncs->Con_Printf("Usage: " PREFIX "delaimentity <slot>\n");
		return;
	}

	int slot = atoi(pEngfuncs->Cmd_Argv(1));

	g_Aiming.removeAimLayer(slot);
}

REGISTER_CMD_FUNC(viewaimlayers)
{
	g_Aiming.showAimLayers();
}

REGISTER_CMD_FUNC(showentities)
{
	cl_entity_t *pEnt = NULL;

	pEngfuncs->Con_Printf("Showing known entities with models:\n");

	for (int i = 0; i < 1024; i++)
	{
		pEnt = pEngfuncs->GetEntityByIndex(i);

		if(pEnt && pEnt->model)
		{
			int iDistance = (int) (pEnt->origin - ppmove->origin).Length();
			pEngfuncs->Con_Printf("  %03d. %s (dist: %d)\n", i, pEnt->model->name, iDistance);
		}
	}
}
