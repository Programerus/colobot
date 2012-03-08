

////////////////////////////////////////////////////////////////////
// routine show()
// utilisable depuis le programme �crit en CBot

// ex�cution
BOOL	rShow( CBotVar* pVar, CBotVar* pResult, int& Exception, void* pUser )
{
	CString		s;

	while ( pVar != NULL )
	{
		CString	ss;
		ss.LoadString( TX_TYPENAMES + pVar->GivType() );
		s += ss + " ";

		ss = pVar->GivName();
		if (ss.IsEmpty()) ss = "<sans nom>";
		s += ss + " = ";

		s += pVar->GivValString();
		s += "\n";
		pVar = pVar->GivNext();
	}

	AfxMessageBox(s, MB_OK|MB_ICONINFORMATION);

	return	TRUE;			// pas d'interruption
}

CBotTypResult	cShow( CBotVar* &pVar, void* pUser)
{
	if ( pVar == NULL ) return CBotTypResult(5028);
	return CBotTypResult(0);		// tous param�tres accept�s, void en retour
}


////////////////////////////////////////////////////////////////////
// routine print()
// utilisable depuis le programme �crit en CBot

// ex�cution
BOOL	rPrintLn( CBotVar* pVar, CBotVar* pResult, int& Exception, void* pUser )
{
	CString		s;

	CTestCBotApp* pApp = (CTestCBotApp*)AfxGetApp();
	CEdit*		 pEdit = pApp->m_pConsole;

	if (pEdit == NULL) return TRUE;
	pEdit->GetWindowText(s);

	while ( pVar != NULL )
	{
		if ( !s.IsEmpty() ) s += " ";
		s += pVar->GivValString();
		pVar = pVar->GivNext();
	}
	s += "\r\n";

	pEdit->SetWindowText(s);
	pEdit->SetSel(s.GetLength(), s.GetLength());
	pEdit->SetFocus();
	return	TRUE;			// pas d'interruption
}

BOOL	rPrint( CBotVar* pVar, CBotVar* pResult, int& Exception, void* pUser )
{
	CString		s;

	CTestCBotApp* pApp = (CTestCBotApp*)AfxGetApp();
	CEdit*		 pEdit = pApp->m_pConsole;

	if (pEdit == NULL) return TRUE;
	pEdit->GetWindowText(s);

	while ( pVar != NULL )
	{
		if ( !s.IsEmpty() ) s += " ";
		s += pVar->GivValString();
		pVar = pVar->GivNext();
	}

	pEdit->SetWindowText(s);
	pEdit->SetSel(s.GetLength(), s.GetLength());
	pEdit->SetFocus();
	return	TRUE;			// pas d'interruption
}

CBotTypResult	cPrint( CBotVar* &pVar, void* pUser)
{
	return CBotTypResult(0);				// tous param�tres accept�s, un entier en retour
}


//////////////////////////////////////////////////////////////////
// class CPoint pour essayer

// ex�cution
BOOL	rCPoint( CBotVar* pThis, CBotVar* pVar, CBotVar* pResult, int& Exception )
{
	CString		s;

	if ( pVar == NULL )return TRUE;			// constructeur sans param�tres est ok

	CBotVar*	pX = pThis->GivItem("x");
	pX->SetValFloat( pVar->GivValFloat() );
	pVar	= pVar->GivNext();

	CBotVar*	pY = pThis->GivItem("y");
	pY->SetValFloat( pVar->GivValFloat() );
	pVar	= pVar->GivNext();

	return	TRUE;			// pas d'interruption
}

CBotTypResult cCPoint( CBotVar* pThis, CBotVar* &pVar)
{
	// ok si aucun param�tres !
	if ( pVar == NULL ) return CBotTypResult(0);

	// param�tre de type num�rique svp
	if ( pVar->GivType() > CBotTypDouble ) return CBotTypResult(5011);
	pVar	= pVar->GivNext();

	// il doit y avoir un second param�tre
	if ( pVar == NULL ) return 5028;
	// �galement de type num�rique
	if ( pVar->GivType() > CBotTypDouble )return CBotTypResult(5011);
	pVar	= pVar->GivNext();

	// et pas plus de 2 param�tres svp
	if ( pVar != NULL ) return CBotTypResult(5026);

	return CBotTypResult(0);		// cette fonction retourne void
}


