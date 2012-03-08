///////////////////////////////////////////////////
// expression du genre Op�rande1 > Op�rande2
//					   Op�rande1 != Op�rande2
// etc.

#include "CBot.h"

// divers constructeurs

CBotCompExpr::CBotCompExpr()
{
	m_leftop	=
	m_rightop	= NULL;
	name = "CBotCompExpr";
}

CBotCompExpr::~CBotCompExpr()
{
	delete	m_leftop;
	delete	m_rightop;
}


// compile une instruction de type A < B

CBotInstr* CBotCompExpr::Compile(CBotToken* &p, CBotCStack* pStack)
{
	CBotCStack* pStk = pStack->TokenStack();

	CBotInstr*	left = CBotAddExpr::Compile( p, pStk );		// expression A + B � gauche
	if (left == NULL) return pStack->Return(NULL, pStk);	// erreur

	if ( p->GetType() == ID_HI ||
		 p->GetType() == ID_LO ||
		 p->GetType() == ID_HS ||
		 p->GetType() == ID_LS ||
		 p->GetType() == ID_EQ ||
		 p->GetType() == ID_NE)								// les diverses comparaisons
	{
		CBotCompExpr* inst = new CBotCompExpr();			// �l�ment pour op�ration
		inst->SetToken(p);									// m�morise l'op�ration

		int			 type1, type2;
		type1 = pStk->GetType(CBotTypChar);

		p = p->Next();
		if ( NULL != (inst->m_rightop = CBotAddExpr::Compile( p, pStk )) )	// expression A + B � droite
		{
			type2 = pStk->GetType(CBotTypChar);
			// les r�sultats sont-ils compatibles
			if ( type1 == type2 && type1 != CBotTypBoolean && type1 != CBotTypClass)
			{
				inst->m_leftop = left;
				pStk->SetVar(new CBotVar(NULL, CBotTypBoolean));
															// le r�sultat est un boolean
				return pStack->Return(inst, pStk);
			}
			pStk->SetError(TX_BAD2TYPE, &inst->m_token);
		}

		delete left;
		delete inst;
		return pStack->Return(NULL, pStk);
	}

	return pStack->Return(left, pStk);
}


// fait l'op�ration

BOOL CBotCompExpr::Execute(CBotStack* &pStack)
{
	CBotStack* pStk1 = pStack->AddStack();

	if ( pStk1->GetState() == 0 && !m_leftop->Execute(pStk1) ) return FALSE; // interrompu ici ?

	pStk1->SetState(1);		// op�ration termin�e

	// demande un peu plus de stack pour ne pas toucher le r�sultat de gauche
	CBotStack* pStk2 = pStk1->AddStack();

	if ( !m_rightop->Execute(pStk2) ) return FALSE; // interrompu ici ?

	int		type1 = pStk1->GetType();
	int		type2 = pStk2->GetType();

	CBotVar*	temp   = CBotVar::Create( NULL, MAX(type1, type2) );
	CBotVar*	result = CBotVar::Create( NULL, CBotTypBoolean );

	switch (GetTokenType())
	{
	case ID_LO:
		temp->Lo(pStk1->GetVar(), pStk2->GetVar());		// inf�rieur
		break;
	case ID_HI:
		temp->Hi(pStk1->GetVar(), pStk2->GetVar());		// sup�rieur
		break;
	case ID_LS:
		temp->Ls(pStk1->GetVar(), pStk2->GetVar());		// inf�rieur ou �gal
		break;
	case ID_HS:
		temp->Hs(pStk1->GetVar(), pStk2->GetVar());		// sup�rieur ou �gal
		break;
	case ID_EQ:
		temp->Eq(pStk1->GetVar(), pStk2->GetVar());		// �gal
		break;
	case ID_NE:
		temp->Ne(pStk1->GetVar(), pStk2->GetVar());		// diff�rent
		break;
	}
	result->SetValInt(temp->GetValInt());				// converti le r�sultat
	delete temp;

	pStk2->SetVar(result);				// met le r�sultat sur la pile

	pStk1->Return(pStk2);				// lib�re la pile
	return pStack->Return(pStk1);		// transmet le r�sultat
}

