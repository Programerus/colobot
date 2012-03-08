// autosafe.cpp

#define STRICT
#define D3D_OVERLOADS

#include <windows.h>
#include <stdio.h>
#include <d3d.h>

#include "struct.h"
#include "D3DEngine.h"
#include "D3DMath.h"
#include "global.h"
#include "event.h"
#include "misc.h"
#include "iman.h"
#include "math3d.h"
#include "particule.h"
#include "light.h"
#include "terrain.h"
#include "camera.h"
#include "object.h"
#include "interface.h"
#include "button.h"
#include "robotmain.h"
#include "window.h"
#include "sound.h"
#include "displaytext.h"
#include "cmdtoken.h"
#include "auto.h"
#include "autosafe.h"



#define OPEN_DELAY		8.0f		// dur�e de l'ouverture




// Constructeur de l'objet.

CAutoSafe::CAutoSafe(CInstanceManager* iMan, CObject* object)
						  : CAuto(iMan, object)
{
	int		i;

	CAuto::CAuto(iMan, object);

	for ( i=0 ; i<4 ; i++ )
	{
		m_bKey[i] = FALSE;
		m_keyParti[i] = -1;
	}

	m_bLock = FALSE;
	m_lastParticule = 0.0f;
	m_channelSound = -1;
	Init();
}

// Destructeur de l'objet.

CAutoSafe::~CAutoSafe()
{
	CAuto::~CAuto();
}


// D�truit l'objet.

void CAutoSafe::DeleteObject(BOOL bAll)
{
	CObject*	pObj;

	pObj = SearchVehicle();
	if ( pObj != 0 )
	{
		pObj->DeleteObject();
		delete pObj;
	}

	if ( m_channelSound != -1 )
	{
		m_sound->FlushEnvelope(m_channelSound);
		m_sound->AddEnvelope(m_channelSound, 0.0f, 1.0f, 1.0f, SOPER_STOP);
		m_channelSound = -1;
	}

	CAuto::DeleteObject(bAll);
}


// Initialise l'objet.

void CAutoSafe::Init()
{
	m_time = 0.0f;
	m_timeVirus = 0.0f;
	m_lastParticule = 0.0f;

	m_countKeys   = 0;
	m_actualAngle = 0.0f;
	m_finalAngle  = 0.0f;

	m_phase    = ASAP_WAIT;  // attend ...
	m_progress = 0.0f;
	m_speed    = 1.0f/1.0f;

	CAuto::Init();
}


// Gestion d'un �v�nement.

BOOL CAutoSafe::EventProcess(const Event &event)
{
	CObject*	pObj;
	D3DVECTOR	pos, speed;
	FPOINT		dim;
	int			i, count;

	CAuto::EventProcess(event);

	if ( m_engine->RetPause() )  return TRUE;
	if ( event.event != EVENT_FRAME )  return TRUE;

	m_progress += event.rTime*m_speed;
	m_timeVirus -= event.rTime;

	if ( m_object->RetVirusMode() )  // contamin� par un virus ?
	{
		if ( m_timeVirus <= 0.0f )
		{
			m_timeVirus = 0.1f+Rand()*0.3f;
		}
		return TRUE;
	}

	EventProgress(event.rTime);

	if ( !m_bLock )
	{
		pObj = SearchVehicle();
		if ( pObj != 0 )
		{
			pObj->SetLock(TRUE);  // objet pas encore utilisable
			m_main->CreateShortcuts();
			m_bLock = TRUE;
		}
	}

	if ( m_phase == ASAP_WAIT )
	{
		if ( m_progress >= 1.0f )
		{
			count = CountKeys();  // compte les cl�s pr�sentes
			if ( count != m_countKeys )
			{
				m_countKeys = count;

				if ( count == 0 )  m_finalAngle =   0.0f*PI/180.0f;
				if ( count == 1 )  m_finalAngle =   5.0f*PI/180.0f;
				if ( count == 2 )  m_finalAngle =  10.0f*PI/180.0f;
				if ( count == 3 )  m_finalAngle =  15.0f*PI/180.0f;
				if ( count == 4 )  m_finalAngle = 120.0f*PI/180.0f;

				if ( count == 4 )  // toutes les cl�s ?
				{
					LockKeys();

					m_channelSound = m_sound->Play(SOUND_MANIP, m_object->RetPosition(0), 1.0f, 0.25f, TRUE);
					m_sound->AddEnvelope(m_channelSound, 1.0f, 2.00f, OPEN_DELAY, SOPER_STOP);

					m_phase    = ASAP_OPEN;
					m_progress = 0.0f;
					m_speed    = 1.0f/OPEN_DELAY;
					return TRUE;
				}
				else
				{
					m_channelSound = m_sound->Play(SOUND_MANIP, m_object->RetPosition(0), 1.0f, 0.25f, TRUE);
					m_sound->AddEnvelope(m_channelSound, 1.0f, 0.35f, 0.5f, SOPER_STOP);
				}
			}

			m_phase    = ASAP_WAIT;
			m_progress = 0.0f;
			m_speed    = 1.0f/1.0f;
		}
	}

	if ( m_phase == ASAP_OPEN )
	{
		if ( m_progress < 1.0f )
		{
			DownKeys(m_progress);

			if ( m_lastParticule+m_engine->ParticuleAdapt(0.05f) <= m_time )
			{
				m_lastParticule = m_time;

				for ( i=0 ; i<10 ; i++ )
				{
					pos = m_object->RetPosition(0);
					pos.x += (Rand()-0.5f)*10.0f;
					pos.z += (Rand()-0.5f)*10.0f;
					speed.x = (Rand()-0.5f)*4.0f;
					speed.z = (Rand()-0.5f)*4.0f;
					speed.y = Rand()*15.0f;
					dim.x = Rand()*6.0f+4.0f;
					dim.y = dim.x;
					m_particule->CreateParticule(pos, speed, dim, PARTIBLUE, 1.0f, 0.0f, 0.0f);
				}

				pos = m_object->RetPosition(0);
				pos.x += (Rand()-0.5f)*10.0f;
				pos.z += (Rand()-0.5f)*10.0f;
				speed.x = (Rand()-0.5f)*4.0f;
				speed.z = (Rand()-0.5f)*4.0f;
				speed.y = Rand()*10.0f;
				dim.x = Rand()*3.0f+2.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTIGLINT, 1.0f, 0.0f, 0.0f);

				for ( i=0 ; i<4 ; i++ )
				{
					pos = m_keyPos[i];
					speed.x = (Rand()-0.5f)*2.0f;
					speed.z = (Rand()-0.5f)*2.0f;
					speed.y = 1.0f+Rand()*1.0f;
					dim.x = Rand()*1.5f+1.5f;
					dim.y = dim.x;
					m_particule->CreateParticule(pos, speed, dim, PARTISMOKE3, 4.0f, 0.0f, 0.0f);
				}
			}
		}
		else
		{
			DeleteKeys();

			pObj = SearchVehicle();
			if ( pObj != 0 )
			{
				pObj->SetLock(FALSE);  // objet utilisable
				m_main->CreateShortcuts();
			}

			m_object->FlushCrashShere();
			m_object->SetGlobalSphere(D3DVECTOR(0.0f, 0.0f, 0.0f), 0.0f);

			m_sound->Play(SOUND_FINDING, m_object->RetPosition(0));

			m_phase    = ASAP_FINISH;
			m_progress = 0.0f;
			m_speed    = 1.0f/100.0f;
		}
	}

	if ( m_phase == ASAP_FINISH )
	{
		if ( m_progress >= 1.0f )
		{
			m_phase    = ASAP_FINISH;
			m_progress = 0.0f;
			m_speed    = 1.0f/100.0f;
		}
	}

	// Ouvre ou ferme les portes.
	if ( m_actualAngle != m_finalAngle )
	{
		if ( m_actualAngle < m_finalAngle )
		{
			m_actualAngle += (105.0f*PI/180.0f)*event.rTime/OPEN_DELAY;
			if ( m_actualAngle > m_finalAngle )  m_actualAngle = m_finalAngle;
		}
		else
		{
			m_actualAngle -= (105.0f*PI/180.0f)*event.rTime/OPEN_DELAY;
			if ( m_actualAngle < m_finalAngle )  m_actualAngle = m_finalAngle;
		}
		m_object->SetAngleZ(1,  m_actualAngle);
		m_object->SetAngleZ(2, -m_actualAngle);
	}

	// Fait clignotter les cl�s.
	speed = D3DVECTOR(0.0f, 0.0f, 0.0f);
	dim.x = 2.0f;
	dim.y = dim.x;
	for ( i=0 ; i<4 ; i++ )
	{
		if ( m_phase != ASAP_WAIT || !m_bKey[i] || Mod(m_time, 1.0f) < 0.4f )
		{
			if ( m_keyParti[i] != -1 )
			{
				m_particule->DeleteParticule(m_keyParti[i]);
				m_keyParti[i] = -1;
			}
		}
		else
		{
			if ( m_keyParti[i] == -1 )
			{
				pos = m_keyPos[i];
				pos.y += 2.2f;
				m_keyParti[i] = m_particule->CreateParticule(pos, speed, dim, PARTISELY, 1.0f, 0.0f, 0.0f);
			}
		}
	}

	return TRUE;
}


// Cr�e toute l'interface lorsque l'objet est s�lectionn�.

BOOL CAutoSafe::CreateInterface(BOOL bSelect)
{
	CWindow*	pw;
	FPOINT		pos, ddim;
	float		ox, oy, sx, sy;

	CAuto::CreateInterface(bSelect);

	if ( !bSelect )  return TRUE;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW0);
	if ( pw == 0 )  return FALSE;

	ox = 3.0f/640.0f;
	oy = 3.0f/480.0f;
	sx = 33.0f/640.0f;
	sy = 33.0f/480.0f;

	pos.x = ox+sx*0.0f;
	pos.y = oy+sy*0;
	ddim.x = 66.0f/640.0f;
	ddim.y = 66.0f/480.0f;
	pw->CreateGroup(pos, ddim, 114, EVENT_OBJECT_TYPE);

	return TRUE;
}


// Retourne une erreur li�e � l'�tat de l'automate.

Error CAutoSafe::RetError()
{
	if ( m_object->RetVirusMode() )
	{
		return ERR_BAT_VIRUS;
	}
	return ERR_OK;
}


// Sauve tous les param�tres de l'automate.

BOOL CAutoSafe::Write(char *line)
{
	char	name[100];

	if ( m_phase == ASAP_WAIT )  return FALSE;

	sprintf(name, " aExist=%d", 1);
	strcat(line, name);

	CAuto::Write(line);

	sprintf(name, " aPhase=%d", m_phase);
	strcat(line, name);

	sprintf(name, " aProgress=%.2f", m_progress);
	strcat(line, name);

	sprintf(name, " aSpeed=%.2f", m_speed);
	strcat(line, name);

	return TRUE;
}

// Restitue tous les param�tres de l'automate.

BOOL CAutoSafe::Read(char *line)
{
	if ( OpInt(line, "aExist", 0) == 0 )  return FALSE;

	CAuto::Read(line);

	m_phase = (AutoSafePhase)OpInt(line, "aPhase", ASAP_WAIT);
	m_progress = OpFloat(line, "aProgress", 0.0f);
	m_speed = OpFloat(line, "aSpeed", 1.0f);

	m_lastParticule = 0.0f;

	return TRUE;
}


// Compte le nombre de cl�s pr�sentes.

int CAutoSafe::CountKeys()
{
	CObject*	pObj;
	D3DVECTOR	cPos, oPos;
	FPOINT		rot;
	ObjectType	oType;
	float		dist, angle, limit, cAngle, oAngle;
	int			i, index;

	cPos   = m_object->RetPosition(0);
	cAngle = m_object->RetAngleY(0);

	for ( index=0 ; index<4 ; index++ )
	{
		m_bKey[index] = FALSE;
		m_keyPos[index] = cPos;
	}

	for ( i=0 ; i<1000000 ; i++ )
	{
		pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
		if ( pObj == 0 )  break;

		oType = pObj->RetType();
		if ( pObj->RetTruck() != 0 )  continue;

		if ( oType != OBJECT_KEYa &&
			 oType != OBJECT_KEYb &&
			 oType != OBJECT_KEYc &&
			 oType != OBJECT_KEYd )  continue;

		oPos = pObj->RetPosition(0);
		dist = Length2d(oPos, cPos);
		if ( dist > 20.0f )  continue;

		if ( oType == OBJECT_KEYa )
		{
			limit  = PI*1.0f;
			oAngle = PI*0.0f;
			index  = 0;
		}
		if ( oType == OBJECT_KEYb )
		{
			limit  = PI*0.0f;
			oAngle = PI*1.0f;
			index  = 1;
		}
		if ( oType == OBJECT_KEYc )
		{
			limit  = PI*1.5f;
			oAngle = PI*0.5f;
			index  = 2;
		}
		if ( oType == OBJECT_KEYd )
		{
			limit  = PI*0.5f;
			oAngle = PI*0.0f;
			index  = 3;
		}

		angle = RotateAngle(oPos.x-cPos.x, oPos.z-cPos.z)+cAngle;
		if ( !TestAngle(angle, limit-8.0f*PI/180.0f, limit+8.0f*PI/180.0f) )  continue;

		// D�place la cl� sur la forme du socle.
		rot = RotatePoint(FPOINT(cPos.x, cPos.z), limit-cAngle, FPOINT(cPos.x+16.0f, cPos.z));
		oPos.x = rot.x;
		oPos.z = rot.y;
		oPos.y = cPos.y+1.0f;
		pObj->SetPosition(0, oPos);
		pObj->SetAngleY(0, oAngle+cAngle);
		m_keyPos[index] = oPos;

		m_bKey[index] = TRUE;
	}

	i = 0;
	for ( index=0 ; index<4 ; index++ )
	{
		if ( m_bKey[index] )  i++;
	}
	return i;
}

// Bloque toutes les cl�s pr�sentes.

void CAutoSafe::LockKeys()
{
	CObject*	pObj;
	D3DVECTOR	cPos, oPos;
	ObjectType	oType;
	float		dist;
	int			i;

	cPos = m_object->RetPosition(0);

	for ( i=0 ; i<1000000 ; i++ )
	{
		pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
		if ( pObj == 0 )  break;

		oType = pObj->RetType();
		if ( pObj->RetTruck() != 0 )  continue;

		if ( oType != OBJECT_KEYa &&
			 oType != OBJECT_KEYb &&
			 oType != OBJECT_KEYc &&
			 oType != OBJECT_KEYd )  continue;

		oPos = pObj->RetPosition(0);
		dist = Length2d(oPos, cPos);
		if ( dist > 20.0f )  continue;

		pObj->SetLock(TRUE);
	}
}

// Fait descendre toutes les cl�s pr�sentes.

void CAutoSafe::DownKeys(float progress)
{
	CObject*	pObj;
	D3DVECTOR	cPos, oPos;
	ObjectType	oType;
	float		dist;
	int			i;

	cPos = m_object->RetPosition(0);

	for ( i=0 ; i<1000000 ; i++ )
	{
		pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
		if ( pObj == 0 )  break;

		oType = pObj->RetType();
		if ( pObj->RetTruck() != 0 )  continue;

		if ( oType != OBJECT_KEYa &&
			 oType != OBJECT_KEYb &&
			 oType != OBJECT_KEYc &&
			 oType != OBJECT_KEYd )  continue;

		oPos = pObj->RetPosition(0);
		dist = Length2d(oPos, cPos);
		if ( dist > 20.0f )  continue;

		oPos.y = cPos.y+1.0f-progress*2.2f;
		pObj->SetPosition(0, oPos);
	}
}

// Supprime toutes les cl�s pr�sentes.

void CAutoSafe::DeleteKeys()
{
	CObject*	pObj;
	D3DVECTOR	cPos, oPos;
	ObjectType	oType;
	float		dist;
	int			i;
	BOOL		bDelete;

	cPos = m_object->RetPosition(0);

	do
	{
		bDelete = FALSE;
		for ( i=0 ; i<1000000 ; i++ )
		{
			pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
			if ( pObj == 0 )  break;

			oType = pObj->RetType();
			if ( pObj->RetTruck() != 0 )  continue;

			if ( oType != OBJECT_KEYa &&
				 oType != OBJECT_KEYb &&
				 oType != OBJECT_KEYc &&
				 oType != OBJECT_KEYd )  continue;

			oPos = pObj->RetPosition(0);
			dist = Length2d(oPos, cPos);
			if ( dist > 20.0f )  continue;

			pObj->DeleteObject();
			delete pObj;
			bDelete = TRUE;
		}
	}
	while ( bDelete );
}

// Cherche le v�hicule dans le coffre-fort.

CObject* CAutoSafe::SearchVehicle()
{
	CObject*	pObj;
	D3DVECTOR	cPos, oPos;
	ObjectType	oType;
	float		dist;
	int			i;

	cPos = m_object->RetPosition(0);

	for ( i=0 ; i<1000000 ; i++ )
	{
		pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
		if ( pObj == 0 )  break;

		oType = pObj->RetType();
		if ( pObj == m_object )  continue;
		if ( pObj->RetTruck() != 0 )  continue;

		oPos = pObj->RetPosition(0);
		dist = Length2d(oPos, cPos);
		if ( dist <= 4.0f )  return pObj;
	}
	return 0;
}



