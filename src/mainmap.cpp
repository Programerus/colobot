// * This file is part of the COLOBOT source code
// * Copyright (C) 2001-2008, Daniel ROUX & EPSITEC SA, www.epsitec.ch
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.// mainmap.cpp

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
#include "iman.h"
#include "interface.h"
#include "map.h"
#include "image.h"
#include "group.h"
#include "slider.h"
#include "scroll.h"
#include "window.h"
#include "mainmap.h"



#define ZOOM_MIN	1.0f
#define ZOOM_MAX	16.0f



// Constructeur de l'application carte.

CMainMap::CMainMap(CInstanceManager* iMan)
{
	m_iMan = iMan;
	m_iMan->AddInstance(CLASS_MAP, this);

	m_interface = (CInterface*)m_iMan->SearchInstance(CLASS_INTERFACE);
	m_event     = (CEvent*)m_iMan->SearchInstance(CLASS_EVENT);
	m_engine    = (CD3DEngine*)m_iMan->SearchInstance(CLASS_ENGINE);

	m_mapMode = 1;
	m_bFixImage = FALSE;
}

// Destructeur de l'application robot.

CMainMap::~CMainMap()
{
}


// Cr�e la mini-carte et les boutons correspondants.

void CMainMap::CreateMap()
{
	CWindow*	pw;
	FPOINT		pos, dim;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )
	{
		pos.x = 0.0f;
		pos.y = 0.0f;
		dim.x = 0.0f;
		dim.y = 0.0f;
		pw = m_interface->CreateWindows(pos, dim, 10, EVENT_WINDOW1);
	}

	dim.x = 10.0f/640.0f;
	dim.y = 10.0f/480.0f;
	pos.x = 10.0f/640.0f;
	pos.y = 10.0f/480.0f;
	pw->CreateMap   (pos, dim, 2, EVENT_OBJECT_MAP);
	pw->CreateSlider(pos, dim, 0, EVENT_OBJECT_MAPZOOM);

	DimMap();
}

// Indique si la mini-carte doit afficher une image fixe.

void CMainMap::SetFixImage(char *filename)
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	pw->DeleteControl(EVENT_OBJECT_MAPZOOM);
	m_bFixImage = TRUE;

	pm->SetFixImage(filename);
}

// Choix des couleurs du sol et de l'eau pour la mini-carte.

void CMainMap::FloorColorMap(D3DCOLORVALUE floor, D3DCOLORVALUE water)
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm != 0 )
	{
		pm->SetFloorColor(floor);
		pm->SetWaterColor(water);
	}
}

// Montre ou cache la mini-carte.

void CMainMap::ShowMap(BOOL bShow)
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	if ( bShow )
	{
		DimMap();
	}
	else
	{
		pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
		if ( pm != 0 )
		{
			pm->ClearState(STATE_VISIBLE);
		}

		ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
		if ( ps != 0 )
		{
			ps->ClearState(STATE_VISIBLE);
		}
	}
}

// Dimensions de la mini-carte.

void CMainMap::DimMap()
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;
	FPOINT		pos, dim;
	float		value;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;
	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	pm->SetState(STATE_VISIBLE, (m_mapMode != 0));

	dim.x = 100.0f/640.0f;
	dim.y = 100.0f/480.0f;
	pos.x = 540.0f/640.0f;
	pos.y =   0.0f/480.0f;
	pm->SetPos(pos);
	pm->SetDim(dim);

	ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
	if ( ps != 0 )
	{
		ps->SetState(STATE_VISIBLE, (m_mapMode != 0));

		dim.x = SCROLL_WIDTH;
		dim.y =  66.0f/480.0f;
		pos.x = 523.0f/640.0f;
		pos.y =   3.0f/480.0f;
		ps->SetPos(pos);
		ps->SetDim(dim);

		value = pm->RetZoom();
		value = (value-ZOOM_MIN)/(ZOOM_MAX-ZOOM_MIN);
		value = powf(value, 0.5f);
		ps->SetVisibleValue(value);
		ps->SetArrowStep(0.2f);
	}
}

// Retourne le zoom actuel de la mini-carte.

float CMainMap::RetZoomMap()
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;

	pw = (CWindow*)pw->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return ZOOM_MIN;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return ZOOM_MIN;

	ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
	if ( ps == 0 )  return ZOOM_MIN;

	return pm->RetZoom();
}

// Zoom la mini-carte d'un facteur quelconque.

void CMainMap::ZoomMap(float zoom)
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;
	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
	if ( ps == 0 )  return;

	if ( zoom < ZOOM_MIN )  zoom = ZOOM_MIN;
	if ( zoom > ZOOM_MAX )  zoom = ZOOM_MAX;
	pm->SetZoom(zoom);

	DimMap();
}

// Zoom la mini-carte selon le slider.

void CMainMap::ZoomMap()
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;
	float		zoom;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;
	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
	if ( ps == 0 )  return;

	zoom = ps->RetVisibleValue();
	zoom = powf(zoom, 2.0f);
	zoom = ZOOM_MIN+zoom*(ZOOM_MAX-ZOOM_MIN);
	pm->SetZoom(zoom);

	DimMap();
}

// Active ou d�sactive la carte.

void CMainMap::MapEnable(BOOL bEnable)
{
	CWindow*	pw;
	CMap*		pm;
	CSlider*	ps;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm != 0 )
	{
		pm->SetEnable(bEnable);
	}

	ps = (CSlider*)pw->SearchControl(EVENT_OBJECT_MAPZOOM);
	if ( ps != 0 )
	{
		ps->SetState(STATE_ENABLE, bEnable);
	}
}

// Sp�cifie le type de l'ic�ne pour l'objet s�lectionn�.

void CMainMap::SetToy(BOOL bToy)
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	pm->SetToy(bToy);
}

// Sp�cifie les param�tres lors de l'usage d'une image fixe.

void CMainMap::SetFixParam(float zoom, float ox, float oy, float angle,
						   int mode, BOOL bDebug)
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return;

	pm->SetZoom(zoom);
	pm->SetOffset(ox, oy);
	pm->SetAngle(angle);
	pm->SetMode(mode);
	pm->SetDebug(bDebug);
}

// Met � jour la mini-carte suite � un changement du terrain.

void CMainMap::UpdateMap()
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm != 0 )
	{
		pm->UpdateTerrain();
	}
}

// Indique si la mini-carte est visible.

BOOL CMainMap::RetShowMap()
{
	return ( m_mapMode != 0 );
}

// Indique si la mini-carte affiche une image fixe.

BOOL CMainMap::RetFixImage()
{
	return m_bFixImage;
}


// D�tecte l'objet vis� dans la mini-carte.

CObject* CMainMap::DetectMap(FPOINT pos, BOOL &bInMap)
{
	CWindow*	pw;
	CMap*		pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return 0;

	bInMap = FALSE;
	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm == 0 )  return 0;
	return pm->DetectObject(pos, bInMap);
}


// Indique l'objet survol� par la souris.

void CMainMap::SetHilite(CObject* pObj)
{
	CWindow*	pw;
	CMap*	pm;

	pw = (CWindow*)m_interface->SearchControl(EVENT_WINDOW1);
	if ( pw == 0 )  return;

	pm = (CMap*)pw->SearchControl(EVENT_OBJECT_MAP);
	if ( pm != 0 )
	{
		pm->SetHilite(pObj);
	}
}


