// global.h

#ifndef _GLOBAL_H_
#define	_GLOBAL_H_


#define BUILD_FACTORY		(1<<0)		// usine
#define BUILD_DERRICK		(1<<1)		// derrick
#define BUILD_CONVERT		(1<<2)		// convertisseur
#define BUILD_RADAR			(1<<3)		// radar
#define BUILD_ENERGY		(1<<4)		// fabrique � pile
#define BUILD_NUCLEAR		(1<<5)		// centrale nucl�aire
#define BUILD_STATION		(1<<6)		// station de recharge
#define BUILD_REPAIR		(1<<7)		// centre de r�paration
#define BUILD_TOWER			(1<<8)		// tour de d�fense
#define BUILD_RESEARCH		(1<<9)		// centre de recherche
#define BUILD_LABO			(1<<10)		// laboratoire
#define BUILD_PARA			(1<<11)		// paratonnerre
#define BUILD_INFO			(1<<12)		// borne d'information
#define BUILD_GFLAT			(1<<16)		// montre le sol plat
#define BUILD_FLAG			(1<<17)		// met/enl�ve drapeau de couleur


// Ne pas changer les valeurs � cause des sauvegardes (bits=...).

#define RESEARCH_TANK		(1<<0)		// chenilles
#define RESEARCH_FLY		(1<<1)		// ailes
#define RESEARCH_CANON		(1<<2)		// canon
#define RESEARCH_TOWER		(1<<3)		// tour de d�fense
#define RESEARCH_ATOMIC		(1<<4)		// nucl�aire
#define RESEARCH_THUMP		(1<<5)		// thumper
#define RESEARCH_SHIELD		(1<<6)		// bouclier
#define RESEARCH_PHAZER		(1<<7)		// canon phazer
#define RESEARCH_iPAW		(1<<8)		// pattes des insectes
#define RESEARCH_iGUN		(1<<9)		// canon des insectes
#define RESEARCH_RECYCLER	(1<<10)		// recycleur
#define RESEARCH_SUBM		(1<<11)		// sous-marin
#define RESEARCH_SNIFFER	(1<<12)		// sniffeur

extern long		g_id;					// identificateur unique
extern long		g_build;				// b�timents constructibles
extern long		g_researchDone;			// recherches effectu�es
extern long		g_researchEnable;		// recherches accessbles
extern float	g_unit;					// facteur de conversion



#endif //_GLOBAL_H_
