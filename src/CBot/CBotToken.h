////////////////////////////////////////////////////////////////////
// interpr�teur pour le language CBot du jeu COLOBOT


// un programme �crit est tout d'abord transform� en une liste de tokens
// avant d'aborder le compilateur proprement dit
// par exemple 
// int var = 3 * ( pos.y + x )
// est d�compos� en (chaque ligne est un token)
//		int
//		var
//		=
//		3
//		*
//		(
//		pos.y
//		+
//		x
//		)


extern BOOL IsOfType(CBotToken* &p, int type1, int type2 = -1);
extern BOOL IsOfTypeList(CBotToken* &p, int type1, ...);
