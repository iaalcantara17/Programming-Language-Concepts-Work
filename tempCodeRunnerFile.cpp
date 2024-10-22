LexItem op = Parser::GetNextToken(in, line);
    	while (op == EXP) 
		{
        	sFactorResult = SFactor(in, line);
        	if (!sFactorResult) 
			{
            	ParseError(line, "Missing factor after exponentiation operator.");
            	return false;
        	}
       		op = Parser::GetNextToken(in, line);
    	}

    	Parser::PushBackToken(op);

		return true;
	}

	bool SFactor(istream &in, int &line) 
	{
    	LexItem op = Parser::GetNextToken(in, line);

    	if (op == PLUS || op == MINUS) 
		{
        	bool factorResult = Factor(in, line);
        	if (!factorResult) 
			{
            	ParseError(line, "Missing factor after unary operator.");
            	return false;
        	}
		} 

		else 
		{
        	Parser::PushBackToken(op);
        	bool factorResult = Factor(in, line);
        	if (!factorResult) 
			{
            	ParseError(line, "Invalid factor.");
            	return false;
        	}
    	}

    	return true;
	}

	bool Factor(istream &in, int &line) 
	{
    	 item = Parser::GetNextToken(in, line);

    	if (item == IDENT || item == ICONST || item == RCONST || item == SCONST) 
		{
        	return true;
    	} 

		else if (item == LPAREN) 
		{
        	bool exprResult = Expr(in, line);
        	if (!exprResult) 
			{
            	ParseError(line, "Missing expression inside parentheses.");
            	return false;
        	}

        	LexItem closeParen = Parser::GetNextToken(in, line);
        	if (closeParen != RPAREN) 
			{
            ParseError(line, "Missing closing parenthesis after expression.");
            return false;
        	}

        	return true;
    	} 
		
		else 
		{
        	ParseError(line, "Invalid factor.");
        	return false;
    	}
	}

	//PrintStmt:= PRINT *, ExpreList 
	bool PrintStmt (istream& in, int& line) 
	{
	LexItem t;
	
	t = Parser::GetNextToken (in, line);
	
 	if ( t != DEF ) 
	{
		ParseError (line, "Print statement syntax error.");
		return false;
	}

	t = Parser::GetNextToken (in, line);
	
	if ( t != COMMA ) 
	{
		ParseError (line, "Missing Comma.");
		return false;
	}

	bool ex = ExprList (in, line);
	
	if ( !ex ) 
	{
		ParseError (line, "Missing expression after Print Statement");
		return false;
	}

	return ex;
	}
	//End of PrintStmt

	//ExprList:= Expr {,Expr}
	bool ExprList (istream& in, int& line) 
	{
	bool status = false;
	
	status = Expr (in, line);
	if (!status)
	{
		ParseError (line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken (in, line);
	
	if (tok == COMMA) 
	{
		status = ExprList (in, line);
	}

	else if(tok.GetToken() == ERR)
	{
		ParseError (line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken (tok);
		return true;
	}
	return status;
	}

//End of ExprList