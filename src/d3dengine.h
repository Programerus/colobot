// D3DEngine.h

#ifndef _D3DENGINE_H_
#define	_D3DENGINE_H_

#include "D3DApp.h"



class CInstanceManager;
class CObject;
class CLight;
class CText;
class CParticule;
class CWater;
class CCloud;
class CBlitz;
class CPlanet;
class CSound;
class CTerrain;


#define D3DMAXOBJECT		1200
#define D3DMAXSHADOW		500
#define D3DMAXGROUNDSPOT	100


enum D3DTypeObj
{
	TYPENULL		= 0,		// object inexistant
	TYPETERRAIN		= 1,		// terrain
	TYPEFIX			= 2,		// objet fixe
	TYPEVEHICULE	= 3,		// objet mobile
	TYPEDESCENDANT	= 4,		// partie d'un objet mobile
	TYPEQUARTZ		= 5,		// objet fixe de type quartz
	TYPEMETAL		= 6,		// objet fixe de type m�talique
};

enum D3DTypeTri
{
	D3DTYPE6T		= 1,		// triangles
	D3DTYPE6S		= 2,		// surfaces
};

enum D3DMaping
{
	D3DMAPPINGX		= 1,
	D3DMAPPINGY		= 2,
	D3DMAPPINGZ		= 3,
	D3DMAPPING1X	= 4,
	D3DMAPPING1Y	= 5,
	D3DMAPPING1Z	= 6,
};

enum D3DMouse
{
	D3DMOUSEHIDE	= 0,		// pas de souris
	D3DMOUSENORM	= 1,
	D3DMOUSEWAIT	= 2,
	D3DMOUSEEDIT	= 3,
	D3DMOUSEHAND	= 4,
	D3DMOUSECROSS	= 5,
	D3DMOUSESHOW	= 6,
	D3DMOUSENO		= 7,
	D3DMOUSEMOVE	= 8,		// +
	D3DMOUSEMOVEH	= 9,		// -
	D3DMOUSEMOVEV	= 10,		// |
	D3DMOUSEMOVED	= 11,		// /
	D3DMOUSEMOVEI	= 12,		// \ 
	D3DMOUSESCROLLL	= 13,		// <<
	D3DMOUSESCROLLR	= 14,		// >>
	D3DMOUSESCROLLU	= 15,		// ^
	D3DMOUSESCROLLD	= 16,		// v
	D3DMOUSETARGET	= 17,
};

enum D3DShadowType
{
	D3DSHADOWNORM	= 0,
	D3DSHADOWWORM	= 1,
};


#define D3DSTATENORMAL	0		// mat�riaux opaque normal
#define D3DSTATETTb		(1<<0)	// transparent selon texture (noir=rien)
#define D3DSTATETTw		(1<<1)	// transparent selon texture (blanc=rien)
#define D3DSTATETD		(1<<2)	// transparent selon couleur diffuse
#define D3DSTATEWRAP	(1<<3)	// texture wrapp�e
#define D3DSTATECLAMP	(1<<4)	// texture bord�e d'une couleur unie
#define D3DSTATELIGHT	(1<<5)	// texture lumineuse (ambiance max)
#define D3DSTATEDUALb	(1<<6)	// double texturage noir
#define D3DSTATEDUALw	(1<<7)	// double texturage blanc
#define D3DSTATEPART1	(1<<8)	// partie 1 (ne pas changer, dans .MOD !)
#define D3DSTATEPART2	(1<<9)	// partie 2
#define D3DSTATEPART3	(1<<10)	// partie 3
#define D3DSTATEPART4	(1<<11)	// partie 4
#define D3DSTATE2FACE	(1<<12)	// mode double-face
#define D3DSTATEALPHA	(1<<13)	// image avec canal alpha
#define D3DSTATESECOND	(1<<14)	// utilise tjrs 2�me �tage de texturage
#define D3DSTATEFOG		(1<<15)	// force le brouillard
#define D3DSTATETCb		(1<<16)	// transparent selon couleur (noir=rien)
#define D3DSTATETCw		(1<<17)	// transparent selon couleur (blanc=rien)


typedef struct
{
	D3DVERTEX2		triangle[3];
	D3DMATERIAL7	material;
	int				state;
	char			texName1[20];
	char			texName2[20];
}
D3DTriangle;


typedef struct
{
	int				totalPossible;
	int				totalUsed;
	D3DMATERIAL7	material;
	int				state;
	D3DTypeTri		type;		// D3DTYPE6x
	D3DVERTEX2		vertex[1];
}
D3DObjLevel6;

typedef struct
{
	int				totalPossible;
	int				totalUsed;
	int				reserve;
	D3DObjLevel6*	table[1];
}
D3DObjLevel5;

typedef struct
{
	int				totalPossible;
	int				totalUsed;
	float			min, max;
	D3DObjLevel5*	table[1];
}
D3DObjLevel4;

typedef struct
{
	int				totalPossible;
	int				totalUsed;
	int				objRank;
	D3DObjLevel4*	table[1];
}
D3DObjLevel3;

typedef struct
{
	int				totalPossible;
	int				totalUsed;
	char			texName1[20];
	char			texName2[20];
	D3DObjLevel3*	table[1];
}
D3DObjLevel2;

typedef struct
{
	int				totalPossible;
	int				totalUsed;
	D3DObjLevel2*	table[1];
}
D3DObjLevel1;


typedef struct
{
	char		bUsed;			// TRUE -> objet existe
	char		bVisible;		// TRUE -> objet visible
	char		bDrawWorld;		// TRUE -> dessine derri�re l'interface
	char		bDrawFront;		// TRUE -> dessine devant l'interface
	int			totalTriangle;	// nb de triangles utilis�s
	D3DTypeObj	type;			// type de l'objet (TYPE*)
	D3DMATRIX	transform;		// matrice de transformation
	float		distance;		// distance point de vue - origine
	D3DVECTOR	bboxMin;		// bounding box de l'objet
	D3DVECTOR	bboxMax;		// (l'origine 0;0;0 est tjrs incluse)
	float		radius;			// rayon de la sph�re � l'origine
	int			shadowRank;		// rang de l'ombre associ�e
	float		transparency;	// transparence de l'objet (0..1)
}
D3DObject;

typedef struct
{
	char		bUsed;			// TRUE -> objet existe
	char		bHide;			// TRUE -> ombre invisible (objet port� par ex.)
	int			objRank;		// rang de l'objet
	D3DShadowType type;			// type de l'ombre
	D3DVECTOR	pos;			// position pour l'ombre
	D3DVECTOR	normal;			// normale au terrain
	float		angle;			// angle de l'ombre
	float		radius;			// rayon de l'ombre
	float		intensity;		// intensit� de l'ombre
	float		height;			// hauteur depuis le sol
}
D3DShadow;

typedef struct
{
	char		bUsed;			// TRUE -> objet existe
	D3DCOLORVALUE color;		// couleur de l'ombre
	float		min, max;		// altitudes min/max
	float		smooth;			// zone de transition
	D3DVECTOR	pos;			// position pour l'ombre
	float		radius;			// rayon de l'ombre
	D3DVECTOR	drawPos;		// position pour l'ombre dessin�e
	float		drawRadius;		// rayon de l'ombre dessin�e
}
D3DGroundSpot;

typedef struct
{
	char		bUsed;			// TRUE -> objet existe
	char		bDraw;			// TRUE -> marque dessin�e
	int			phase;			// 1=croissance, 2=fixe, 3=d�croissance
	float		delay[3];		// d�lais pour les 3 phases
	float		fix;			// temps fixe
	D3DVECTOR	pos;			// position pour marques
	float		radius;			// rayon des marques
	float		intensity;		// intensit� couleur
	D3DVECTOR	drawPos;		// position pour marques dessin�es
	float		drawRadius;		// rayon des marques dessin�es
	float		drawIntensity;	// intensit� dessin�e
	int			dx, dy;			// dimensions table
	char*		table;			// pointeur � la table
}
D3DGroundMark;



class CD3DEngine
{
public:
	CD3DEngine(CInstanceManager *iMan, CD3DApplication *app);
	~CD3DEngine();

	void		SetD3DDevice(LPDIRECT3DDEVICE7 device);
	LPDIRECT3DDEVICE7 RetD3DDevice();

	void		SetTerrain(CTerrain* terrain);

	BOOL		WriteProfile();

	void		SetPause(BOOL bPause);
	BOOL		RetPause();

	void		SetMovieLock(BOOL bLock);
	BOOL		RetMovieLock();

	void		SetShowStat(BOOL bShow);
	BOOL		RetShowStat();

	void		SetRenderEnable(BOOL bEnable);

	HRESULT		OneTimeSceneInit();
	HRESULT		InitDeviceObjects();
	HRESULT		DeleteDeviceObjects();
	HRESULT		RestoreSurfaces();
	HRESULT		Render();
	HRESULT		FrameMove(float rTime);
	void		StepSimul(float rTime);
	HRESULT		FinalCleanup();
	void		AddStatisticTriangle(int nb);
	int			RetStatisticTriangle();
	void		SetHiliteRank(int *rankList);
	BOOL		GetHilite(FPOINT &p1, FPOINT &p2);
	BOOL		GetSpriteCoord(int &x, int &y);
	void		SetInfoText(int line, char* text);
	char*		RetInfoText(int line);
	LRESULT		MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void		FirstExecuteAdapt(BOOL bFirst);
	int			GetVidMemTotal();
	BOOL		IsVideo8MB();
	BOOL		IsVideo32MB();

	BOOL		EnumDevices(char *bufDevices, int lenDevices, char *bufModes, int lenModes, int &totalDevices, int &selectDevices, int &totalModes, int &selectModes);
	BOOL		RetFullScreen();
	BOOL		ChangeDevice(char *device, char *mode, BOOL bFull);

	D3DMATRIX*	RetMatView();
	D3DMATRIX*	RetMatLeftView();
	D3DMATRIX*	RetMatRightView();

	void		TimeInit();
	void		TimeEnterGel();
	void		TimeExitGel();
	float		TimeGet();

	int			RetRestCreate();
	int			CreateObject();
	void		FlushObject();
	BOOL		DeleteObject(int objRank);
	BOOL		SetDrawWorld(int objRank, BOOL bDraw);
	BOOL		SetDrawFront(int objRank, BOOL bDraw);
	BOOL		AddTriangle(int objRank, D3DVERTEX2* vertex, int nb, const D3DMATERIAL7 &mat, int state, char* texName1, char* texName2, float min, float max, BOOL bGlobalUpdate);
	BOOL		AddSurface(int objRank, D3DVERTEX2* vertex, int nb, const D3DMATERIAL7 &mat, int state, char* texName1, char* texName2, float min, float max, BOOL bGlobalUpdate);
	BOOL		AddQuick(int objRank, D3DObjLevel6* buffer, char* texName1, char* texName2, float min, float max, BOOL bGlobalUpdate);
	D3DObjLevel6* SearchTriangle(int objRank, const D3DMATERIAL7 &mat, int state, char* texName1, char* texName2, float min, float max);
	void		ChangeLOD();
	BOOL		ChangeSecondTexture(int objRank, char* texName2);
	int			RetTotalTriangles(int objRank);
	int			GetTriangles(int objRank, float min, float max, D3DTriangle* buffer, int size, float percent);
	BOOL		GetBBox(int objRank, D3DVECTOR &min, D3DVECTOR &max);
	BOOL		ChangeTextureMapping(int objRank, const D3DMATERIAL7 &mat, int state, char* texName1, char* texName2, float min, float max, D3DMaping mode, float au, float bu, float av, float bv);
	BOOL		TrackTextureMapping(int objRank, const D3DMATERIAL7 &mat, int state, char* texName1, char* texName2, float min, float max, D3DMaping mode, float pos, float factor, float tl, float ts, float tt);
	BOOL		SetObjectTransform(int objRank, const D3DMATRIX &transform);
	BOOL		GetObjectTransform(int objRank, D3DMATRIX &transform);
	BOOL		SetObjectType(int objRank, D3DTypeObj type);
	D3DTypeObj	RetObjectType(int objRank);
	BOOL		SetObjectTransparency(int objRank, float value);

	BOOL		ShadowCreate(int objRank);
	void		ShadowDelete(int objRank);
	BOOL		SetObjectShadowHide(int objRank, BOOL bHide);
	BOOL		SetObjectShadowType(int objRank, D3DShadowType type);
	BOOL		SetObjectShadowPos(int objRank, const D3DVECTOR &pos);
	BOOL		SetObjectShadowNormal(int objRank, const D3DVECTOR &n);
	BOOL		SetObjectShadowAngle(int objRank, float angle);
	BOOL		SetObjectShadowRadius(int objRank, float radius);
	BOOL		SetObjectShadowIntensity(int objRank, float intensity);
	BOOL		SetObjectShadowHeight(int objRank, float h);
	float		RetObjectShadowRadius(int objRank);

	void		GroundSpotFlush();
	int			GroundSpotCreate();
	void		GroundSpotDelete(int rank);
	BOOL		SetObjectGroundSpotPos(int rank, const D3DVECTOR &pos);
	BOOL		SetObjectGroundSpotRadius(int rank, float radius);
	BOOL		SetObjectGroundSpotColor(int rank, D3DCOLORVALUE color);
	BOOL		SetObjectGroundSpotMinMax(int rank, float min, float max);
	BOOL		SetObjectGroundSpotSmooth(int rank, float smooth);

	int			GroundMarkCreate(D3DVECTOR pos, float radius, float delay1, float delay2, float delay3, int dx, int dy, char* table);
	BOOL		GroundMarkDelete(int rank);

	void		Update();
	
	void		SetViewParams(const D3DVECTOR &vEyePt, const D3DVECTOR &vLookatPt, const D3DVECTOR &vUpVec, FLOAT fEyeDistance);

	BOOL		FreeTexture(char* name);
	BOOL		LoadTexture(char* name, int stage=0);
	BOOL		LoadAllTexture();

	void		SetLimitLOD(int rank, float limit);
	float		RetLimitLOD(int rank, BOOL bLast=FALSE);

	void		SetTerrainVision(float vision);

	void		SetGroundSpot(BOOL bMode);
	BOOL		RetGroundSpot();
	void		SetShadow(BOOL bMode);
	BOOL		RetShadow();
	void		SetDirty(BOOL bMode);
	BOOL		RetDirty();
	void		SetFog(BOOL bMode);
	BOOL		RetFog();
	BOOL		RetStateColor();

	void		SetSecondTexture(int texNum);
	int			RetSecondTexture();

	void		SetRankView(int rank);
	int			RetRankView();

	void		SetDrawWorld(BOOL bDraw);
	void		SetDrawFront(BOOL bDraw);

	void		SetAmbiantColor(D3DCOLOR color, int rank=0);
	D3DCOLOR	RetAmbiantColor(int rank=0);

	void		SetWaterAddColor(D3DCOLORVALUE color);
	D3DCOLORVALUE RetWaterAddColor();

	void		SetFogColor(D3DCOLOR color, int rank=0);
	D3DCOLOR	RetFogColor(int rank=0);

	void		SetDeepView(float length, int rank=0, BOOL bRef=FALSE);
	float		RetDeepView(int rank=0);

	void		SetFogStart(float start, int rank=0);
	float		RetFogStart(int rank=0);

	void		SetBackground(char *name, D3DCOLOR up=0, D3DCOLOR down=0, D3DCOLOR cloudUp=0, D3DCOLOR cloudDown=0, BOOL bFull=FALSE, BOOL bQuarter=FALSE);
	void		RetBackground(char *name, D3DCOLOR &up, D3DCOLOR &down, D3DCOLOR &cloudUp, D3DCOLOR &cloudDown, BOOL &bFull, BOOL &bQuarter);
	void		SetFrontsizeName(char *name);
	void		SetOverFront(BOOL bFront);
	void		SetOverColor(D3DCOLOR color=0, int mode=D3DSTATETCb);

	void		SetParticuleDensity(float value);
	float		RetParticuleDensity();
	float		ParticuleAdapt(float factor);

	void		SetClippingDistance(float value);
	float		RetClippingDistance();

	void		SetObjectDetail(float value);
	float		RetObjectDetail();

	void		SetGadgetQuantity(float value);
	float		RetGadgetQuantity();

	void		SetTextureQuality(int value);
	int			RetTextureQuality();

	void		SetTotoMode(BOOL bPresent);
	BOOL		RetTotoMode();

	void		SetLensMode(BOOL bPresent);
	BOOL		RetLensMode();

	void		SetWaterMode(BOOL bPresent);
	BOOL		RetWaterMode();

	void		SetBlitzMode(BOOL bPresent);
	BOOL		RetBlitzMode();

	void		SetSkyMode(BOOL bPresent);
	BOOL		RetSkyMode();

	void		SetBackForce(BOOL bPresent);
	BOOL		RetBackForce();

	void		SetPlanetMode(BOOL bPresent);
	BOOL		RetPlanetMode();

	void		SetLightMode(BOOL bPresent);
	BOOL		RetLightMode();

	void		SetEditIndentMode(BOOL bAuto);
	BOOL		RetEditIndentMode();

	void		SetEditIndentValue(int value);
	int			RetEditIndentValue();

	void		SetSpeed(float speed);
	float		RetSpeed();

	void		SetTracePrecision(float factor);
	float		RetTracePrecision();

	void		SetFocus(float focus);
	float		RetFocus();
	D3DVECTOR	RetEyePt();
	D3DVECTOR	RetLookatPt();
	float		RetEyeDirH();
	float		RetEyeDirV();
	POINT		RetDim();
	void		UpdateMatProj();

	void		ApplyChange();

	void		FlushPressKey();
	void		ResetKey();
	void		SetKey(int keyRank, int option, int key);
	int			RetKey(int keyRank, int option);

	void		SetJoystick(BOOL bEnable);
	BOOL		RetJoystick();

	void		SetDebugMode(BOOL bMode);
	BOOL		RetDebugMode();
	BOOL		RetSetupMode();

	BOOL		IsVisiblePoint(const D3DVECTOR &pos);

	int			DetectObject(FPOINT mouse);
	void		SetState(int state, D3DCOLOR color=0xffffffff);
	void		SetTexture(char *name, int stage=0);
	void		SetMaterial(const D3DMATERIAL7 &mat);

	void		MoveMousePos(FPOINT pos);
	void		SetMousePos(FPOINT pos);
	FPOINT		RetMousePos();
	void		SetMouseType(D3DMouse type);
	D3DMouse	RetMouseType();
	void		SetMouseHide(BOOL bHide);
	BOOL		RetMouseHide();
	void		SetNiceMouse(BOOL bNice);
	BOOL		RetNiceMouse();
	BOOL		RetNiceMouseCap();

	CText*		RetText();

	BOOL		ChangeColor(char *name, D3DCOLORVALUE colorRef1, D3DCOLORVALUE colorNew1, D3DCOLORVALUE colorRef2, D3DCOLORVALUE colorNew2, float tolerance1, float tolerance2, FPOINT ts, FPOINT ti, FPOINT *pExclu=0, float shift=0.0f, BOOL bHSV=FALSE);
	BOOL		OpenImage(char *name);
	BOOL		CopyImage();
	BOOL		LoadImage();
	BOOL		ScrollImage(int dx, int dy);
	BOOL		SetDot(int x, int y, D3DCOLORVALUE color);
	BOOL		CloseImage();
	BOOL		WriteScreenShot(char *filename, int width, int height);
	BOOL		GetRenderDC(HDC &hDC);
	BOOL		ReleaseRenderDC(HDC &hDC);
	PBITMAPINFO	CreateBitmapInfoStruct(HBITMAP hBmp);
	BOOL		CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

protected:
	void		MemSpace1(D3DObjLevel1 *&p, int nb);
	void		MemSpace2(D3DObjLevel2 *&p, int nb);
	void		MemSpace3(D3DObjLevel3 *&p, int nb);
	void		MemSpace4(D3DObjLevel4 *&p, int nb);
	void		MemSpace5(D3DObjLevel5 *&p, int nb);
	void		MemSpace6(D3DObjLevel6 *&p, int nb);

	D3DObjLevel2* AddLevel1(D3DObjLevel1 *&p1, char* texName1, char* texName2);
	D3DObjLevel3* AddLevel2(D3DObjLevel2 *&p2, int objRank);
	D3DObjLevel4* AddLevel3(D3DObjLevel3 *&p3, float min, float max);
	D3DObjLevel5* AddLevel4(D3DObjLevel4 *&p4, int reserve);
	D3DObjLevel6* AddLevel5(D3DObjLevel5 *&p5, D3DTypeTri type, const D3DMATERIAL7 &mat, int state, int nb);

	BOOL		IsVisible(int objRank);
	BOOL		DetectBBox(int objRank, FPOINT mouse);
	BOOL		DetectTriangle(FPOINT mouse, D3DVERTEX2 *triangle, int objRank, float &dist);
	BOOL		TransformPoint(D3DVECTOR &p2D, int objRank, D3DVECTOR p3D);
	void		ComputeDistance();
	void		UpdateGeometry();
	void		RenderGroundSpot();
	void		DrawShadow();
	void		DrawBackground();
	void		DrawBackgroundGradient(D3DCOLOR up, D3DCOLOR down);
	void		DrawBackgroundImageQuarter(FPOINT p1, FPOINT p2, char *name);
	void		DrawBackgroundImage();
	void		DrawPlanet();
	void		DrawFrontsize();
	void		DrawOverColor();
	BOOL		GetBBox2D(int objRank, FPOINT &min, FPOINT &max);
	void		DrawHilite();
	void		DrawMouse();
	void		DrawSprite(FPOINT pos, FPOINT dim, int icon);

protected:
	CInstanceManager* m_iMan;
	CD3DApplication* m_app;
	LPDIRECT3DDEVICE7 m_pD3DDevice;
	CText*			m_text;
	CLight*			m_light;
	CParticule*		m_particule;
	CWater*			m_water;
	CCloud*			m_cloud;
	CBlitz*			m_blitz;
	CPlanet*		m_planet;
	CSound*			m_sound;
	CTerrain*		m_terrain;

	int				m_blackSrcBlend[2];
	int				m_blackDestBlend[2];
	int				m_whiteSrcBlend[2];
	int				m_whiteDestBlend[2];
	int				m_diffuseSrcBlend[2];
	int				m_diffuseDestBlend[2];
	int				m_alphaSrcBlend[2];
	int				m_alphaDestBlend[2];

	D3DMATRIX		m_matProj;
	D3DMATRIX		m_matLeftView;
	D3DMATRIX		m_matRightView;
	D3DMATRIX		m_matView;
	float			m_focus;

	D3DMATRIX		m_matWorldInterface;
	D3DMATRIX		m_matProjInterface;
	D3DMATRIX		m_matViewInterface;

	DWORD			m_baseTime;
	DWORD			m_stopTime;
	float			m_absTime;
	float			m_lastTime;
	float			m_speed;
	BOOL			m_bPause;
	BOOL			m_bRender;
	BOOL			m_bMovieLock;

	POINT			m_dim;
	POINT			m_lastDim;
	D3DObjLevel1*	m_objectPointer;
	int				m_objectParamTotal;
	D3DObject*		m_objectParam;
	int				m_shadowTotal;
	D3DShadow*		m_shadow;
	D3DGroundSpot*	m_groundSpot;
	D3DGroundMark	m_groundMark;
	D3DVECTOR		m_eyePt;
	D3DVECTOR		m_lookatPt;
	float			m_eyeDirH;
	float			m_eyeDirV;
	int				m_rankView;
	D3DCOLOR		m_ambiantColor[2];
	D3DCOLOR		m_backColor[2];
	D3DCOLOR		m_fogColor[2];
	float			m_deepView[2];
	float			m_fogStart[2];
	D3DCOLORVALUE	m_waterAddColor;
	int				m_statisticTriangle;
	BOOL			m_bUpdateGeometry;
	char			m_infoText[10][200];
	int				m_alphaMode;
	BOOL			m_bStateColor;
	BOOL			m_bForceStateColor;
	BOOL			m_bGroundSpot;
	BOOL			m_bShadow;
	BOOL			m_bDirty;
	BOOL			m_bFog;
	BOOL			m_bFirstGroundSpot;
	int				m_secondTexNum;
	char			m_backgroundName[50];
	D3DCOLOR		m_backgroundColorUp;
	D3DCOLOR		m_backgroundColorDown;
	D3DCOLOR		m_backgroundCloudUp;
	D3DCOLOR		m_backgroundCloudDown;
	BOOL			m_bBackgroundFull;
	BOOL			m_bBackgroundQuarter;
	BOOL			m_bOverFront;
	D3DCOLOR		m_overColor;
	int				m_overMode;
	char			m_frontsizeName[50];
	BOOL			m_bDrawWorld;
	BOOL			m_bDrawFront;
	float			m_limitLOD[2];
	float			m_particuleDensity;
	float			m_clippingDistance;
	float			m_lastClippingDistance;
	float			m_objectDetail;
	float			m_lastObjectDetail;
	float			m_terrainVision;
	float			m_gadgetQuantity;
	int				m_textureQuality;
	BOOL			m_bTotoMode;
	BOOL			m_bLensMode;
	BOOL			m_bWaterMode;
	BOOL			m_bSkyMode;
	BOOL			m_bBackForce;
	BOOL			m_bPlanetMode;
	BOOL			m_bLightMode;
	BOOL			m_bEditIndentMode;
	int				m_editIndentValue;
	float			m_tracePrecision;

	int				m_hiliteRank[100];
	BOOL			m_bHilite;
	FPOINT			m_hiliteP1;
	FPOINT			m_hiliteP2;

	int				m_lastState;
	D3DCOLOR		m_lastColor;
	char			m_lastTexture[2][50];
	D3DMATERIAL7	m_lastMaterial;

	FPOINT			m_mousePos;
	D3DMouse		m_mouseType;
	BOOL			m_bMouseHide;
	BOOL			m_bNiceMouse;

	LPDIRECTDRAWSURFACE7 m_imageSurface;
	DDSURFACEDESC2	m_imageDDSD;
	WORD*			m_imageCopy;
	int				m_imageDX;
	int				m_imageDY;
};


#endif //_D3DENGINE_H_
