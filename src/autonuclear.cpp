// autonuclear.cpp

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
#include "window.h"
#include "sound.h"
#include "displaytext.h"
#include "cmdtoken.h"
#include "auto.h"
#include "autonuclear.h"



#define NUCLEAR_DELAY		30.0f		// dur�e de la g�n�ration




// Constructeur de l'objet.

CAutoNuclear::CAutoNuclear(CInstanceManager* iMan, CObject* object)
						  : CAuto(iMan, object)
{
	CAuto::CAuto(iMan, object);

	m_channelSound = -1;
	Init();
}

// Destructeur de l'objet.

CAutoNuclear::~CAutoNuclear()
{
	CAuto::~CAuto();
}


// D�truit l'objet.

void CAutoNuclear::DeleteObject(BOOL bAll)
{
	CObject*	fret;

	if ( !bAll )
	{
		fret = SearchUranium();
		if ( fret != 0 )
		{
			fret->DeleteObject();  // d�truit le m�tal
			delete fret;
		}
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

void CAutoNuclear::Init()
{
	D3DMATRIX*	mat;

	m_time = 0.0f;
	m_timeVirus = 0.0f;
	m_lastParticule = 0.0f;

	mat = m_object->RetWorldMatrix(0);
	m_pos = Transform(*mat, D3DVECTOR(22.0f, 4.0f, 0.0f));

	m_phase    = ANUP_WAIT;  // attend ...
	m_progress = 0.0f;
	m_speed    = 1.0f/2.0f;

	CAuto::Init();
}


// Gestion d'un �v�nement.

BOOL CAutoNuclear::EventProcess(const Event &event)
{
	CObject*	fret;
	D3DMATRIX*	mat;
	D3DVECTOR	pos, goal, speed;
	FPOINT		dim, rot;
	float		angle;
	int			i, max;

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

	if ( m_phase == ANUP_WAIT )
	{
		if ( m_progress >= 1.0f )
		{
			fret = SearchUranium();  // uranium � transformer ?
			if ( fret == 0 || SearchVehicle() )
			{
				m_phase    = ANUP_WAIT;  // attend encore ...
				m_progress = 0.0f;
				m_speed    = 1.0f/2.0f;
			}
			else
			{
				fret->SetLock(TRUE);  // uranium plus utilisable

				SetBusy(TRUE);
				InitProgressTotal(1.5f+NUCLEAR_DELAY+1.5f);
				UpdateInterface();

				m_sound->Play(SOUND_OPEN, m_object->RetPosition(0), 1.0f, 1.4f);

				m_phase    = ANUP_CLOSE;
				m_progress = 0.0f;
				m_speed    = 1.0f/1.5f;
			}
		}
	}

	if ( m_phase == ANUP_CLOSE )
	{
		if ( m_progress < 1.0f )
		{
			angle = (1.0f-m_progress)*(135.0f*PI/180.0f);
			m_object->SetAngleZ(1, angle);
		}
		else
		{
			m_object->SetAngleZ(1, 0.0f);

			mat = m_object->RetWorldMatrix(0);
			max = (int)(10.0f*m_engine->RetParticuleDensity());
			for ( i=0 ; i<max ; i++ )
			{
				pos.x = 27.0f;
				pos.y =  0.0f;
				pos.z = (Rand()-0.5f)*8.0f;
				pos = Transform(*mat, pos);
				speed.y = 0.0f;
				speed.x = 0.0f;
				speed.z = 0.0f;
				dim.x = Rand()*1.0f+1.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTICRASH);
			}

			m_sound->Play(SOUND_CLOSE, m_object->RetPosition(0), 1.0f, 1.0f);

			m_channelSound = m_sound->Play(SOUND_NUCLEAR, m_object->RetPosition(0), 1.0f, 0.1f, TRUE);
			m_sound->AddEnvelope(m_channelSound, 1.0f, 1.0f, NUCLEAR_DELAY-1.0f, SOPER_CONTINUE);
			m_sound->AddEnvelope(m_channelSound, 0.0f, 1.0f, 2.0f, SOPER_STOP);

			m_phase    = ANUP_GENERATE;
			m_progress = 0.0f;
			m_speed    = 1.0f/NUCLEAR_DELAY;
		}
	}

	if ( m_phase == ANUP_GENERATE )
	{
		if ( m_progress < 1.0f )
		{
			if ( m_lastParticule+m_engine->ParticuleAdapt(0.10f) <= m_time )
			{
				m_lastParticule = m_time;

				pos = m_object->RetPosition(0);
				pos.y += 30.0f;
				pos.x += (Rand()-0.5f)*6.0f;
				pos.z += (Rand()-0.5f)*6.0f;
				speed.y = Rand()*15.0f+15.0f;
				speed.x = 0.0f;
				speed.z = 0.0f;
				dim.x = Rand()*8.0f+8.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTICRASH);

				pos = m_pos;
				speed.x = (Rand()-0.5f)*20.0f;
				speed.y = (Rand()-0.5f)*20.0f;
				speed.z = (Rand()-0.5f)*20.0f;
				dim.x = 2.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTIBLITZ, 1.0f, 0.0f, 0.0f);
			}
		}
		else
		{
			fret = SearchUranium();
			if ( fret != 0 )
			{
				fret->DeleteObject();  // d�truit l'uranium
				delete fret;
				m_object->SetPower(0);
			}

			CreatePower();  // cr�e la pile atomique

			max = (int)(20.0f*m_engine->RetParticuleDensity());
			for ( i=0 ; i<max ; i++ )
			{
				pos = m_pos;
				pos.x += (Rand()-0.5f)*3.0f;
				pos.y += (Rand()-0.5f)*3.0f;
				pos.z += (Rand()-0.5f)*3.0f;
				speed.y = 0.0f;
				speed.x = 0.0f;
				speed.z = 0.0f;
				dim.x = Rand()*2.0f+2.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTIBLUE, Rand()*5.0f+5.0f, 0.0f, 0.0f);
			}

			m_sound->Play(SOUND_OPEN, m_object->RetPosition(0), 1.0f, 1.4f);

			m_phase    = ANUP_OPEN;
			m_progress = 0.0f;
			m_speed    = 1.0f/1.5f;
		}
	}

	if ( m_phase == ANUP_OPEN )
	{
		if ( m_progress < 1.0f )
		{
			angle = m_progress*(135.0f*PI/180.0f);
			m_object->SetAngleZ(1, angle);
		}
		else
		{
			m_object->SetAngleZ(1, 135.0f*PI/180.0f);

			SetBusy(FALSE);
			UpdateInterface();

			m_displayText->DisplayError(INFO_NUCLEAR, m_object);

			m_phase    = ANUP_WAIT;
			m_progress = 0.0f;
			m_speed    = 1.0f/2.0f;
		}
	}

	return TRUE;
}


// Cr�e toute l'interface lorsque l'objet est s�lectionn�.

BOOL CAutoNuclear::CreateInterface(BOOL bSelect)
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
	pw->CreateGroup(pos, ddim, 110, EVENT_OBJECT_TYPE);

	return TRUE;
}


// Cherche l'objet uranium.

CObject* CAutoNuclear::SearchUranium()
{
	CObject*	pObj;

	pObj = m_object->RetPower();
	if ( pObj == 0 )  return 0;
	if ( pObj->RetType() == OBJECT_URANIUM )  return pObj;
	return 0;
}

// Cherche si un v�hicule est trop proche.

BOOL CAutoNuclear::SearchVehicle()
{
	CObject*	pObj;
	D3DVECTOR	oPos;
	ObjectType	type;
	float		oRadius, dist;
	int			i;

	for ( i=0 ; i<1000000 ; i++ )
	{
		pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
		if ( pObj == 0 )  break;

		type = pObj->RetType();
		if ( type != OBJECT_HUMAN    &&
			 type != OBJECT_MOBILEfa &&
			 type != OBJECT_MOBILEta &&
			 type != OBJECT_MOBILEwa &&
			 type != OBJECT_MOBILEia &&
			 type != OBJECT_MOBILEfc &&
			 type != OBJECT_MOBILEtc &&
			 type != OBJECT_MOBILEwc &&
			 type != OBJECT_MOBILEic &&
			 type != OBJECT_MOBILEfi &&
			 type != OBJECT_MOBILEti &&
			 type != OBJECT_MOBILEwi &&
			 type != OBJECT_MOBILEii &&
			 type != OBJECT_MOBILEfs &&
			 type != OBJECT_MOBILEts &&
			 type != OBJECT_MOBILEws &&
			 type != OBJECT_MOBILEis &&
			 type != OBJECT_MOBILErt &&
			 type != OBJECT_MOBILErc &&
			 type != OBJECT_MOBILErr &&
			 type != OBJECT_MOBILErs &&
			 type != OBJECT_MOBILEsa &&
			 type != OBJECT_MOBILEtg &&
			 type != OBJECT_MOBILEft &&
			 type != OBJECT_MOBILEtt &&
			 type != OBJECT_MOBILEwt &&
			 type != OBJECT_MOBILEit &&
			 type != OBJECT_MOBILEdr &&
			 type != OBJECT_MOTHER   &&
			 type != OBJECT_ANT      &&
			 type != OBJECT_SPIDER   &&
			 type != OBJECT_BEE      &&
			 type != OBJECT_WORM     )  continue;

		if ( !pObj->GetCrashSphere(0, oPos, oRadius) )  continue;
		dist = Length(oPos, m_pos)-oRadius;

		if ( dist < 10.0f )  return TRUE;
	}

	return FALSE;
}

// Cr�e un objet pile.

void CAutoNuclear::CreatePower()
{
	CObject*		power;
	D3DVECTOR		pos;
	float			angle;

	pos = m_object->RetPosition(0);
	angle = m_object->RetAngleY(0);

	power = new CObject(m_iMan);
	if ( !power->CreateResource(pos, angle, OBJECT_ATOMIC) )
	{
		delete power;
		m_displayText->DisplayError(ERR_TOOMANY, m_object);
		return;
	}

	power->SetTruck(m_object);
	power->SetPosition(0, D3DVECTOR(22.0f, 3.0f, 0.0f));
	m_object->SetPower(power);
}


// Retourne une erreur li�e � l'�tat de l'automate.

Error CAutoNuclear::RetError()
{
	CObject*	pObj;
	ObjectType	type;
//?	TerrainRes	res;

	if ( m_object->RetVirusMode() )
	{
		return ERR_BAT_VIRUS;
	}

//?	res = m_terrain->RetResource(m_object->RetPosition(0));
//?	if ( res != TR_POWER )  return ERR_NUCLEAR_NULL;

//?	if ( m_object->RetEnergy() < ENERGY_POWER )  return ERR_NUCLEAR_LOW;

	pObj = m_object->RetPower();
	if ( pObj == 0 )  return ERR_NUCLEAR_EMPTY;
	if ( pObj->RetLock() )  return ERR_OK;
	type = pObj->RetType();
	if ( type == OBJECT_ATOMIC  )  return ERR_OK;
	if ( type != OBJECT_URANIUM )  return ERR_NUCLEAR_BAD;

	return ERR_OK;
}


// Sauve tous les param�tres de l'automate.

BOOL CAutoNuclear::Write(char *line)
{
	char	name[100];

	if ( m_phase == ANUP_STOP ||
		 m_phase == ANUP_WAIT )  return FALSE;

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

BOOL CAutoNuclear::Read(char *line)
{
	if ( OpInt(line, "aExist", 0) == 0 )  return FALSE;

	CAuto::Read(line);

	m_phase = (AutoNuclearPhase)OpInt(line, "aPhase", ANUP_WAIT);
	m_progress = OpFloat(line, "aProgress", 0.0f);
	m_speed = OpFloat(line, "aSpeed", 1.0f);

	m_lastParticule = 0.0f;

	return TRUE;
}


