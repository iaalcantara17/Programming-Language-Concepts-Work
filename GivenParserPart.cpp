//Israel Alcantara

#include "parser.h"

map <string, bool> defVar;
map <string, Token> SymTable;

namespace Parser 
{
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken (istream& in, int& line) 
	{
		if ( pushed_back ) 
		{
			pushed_back = false;
			return pushed_token;
		}

		return getNextToken (in, line);
	}

	static void PushBackToken (LexItem & t) 
	{
		if ( pushed_back ) 
		{
			abort();
		}

		pushed_back = true;
		pushed_token = t;	
	}
}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError (int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList (istream& in, int& line);

bool Prog(istream &in, int &line)
{
	LexItem t = Parser::GetNextToken(in, line);
	
	if (t != PROGRAM) 
	{
		ParseError(line, "PROGRAM keyword missing.");
        return false;
    }

    t = Parser::GetNextToken(in, line);

    if (t != IDENT) 
	{
        ParseError(line, "Program identifier missing.");
        return false;
    }

    while (true) 
	{
        Parser::PushBackToken(t);
        if (!Decl(in, line))
            break;
    }

    while (true) 
	{
        Parser::PushBackToken(t);
        if (!Stmt(in, line))
            break;
    }

    t = Parser::GetNextToken(in, line);

    if (t != END)
	{
        ParseError(line, "END keyword missing.");
        return false;
    }

    t = Parser::GetNextToken(in, line);

    if (t != PROGRAM) 
	{
        ParseError(line, "PROGRAM keyword missing after END.");
        return false;
    }

    t = Parser::GetNextToken(in, line);

    if (t != IDENT) 
	{
        ParseError(line, "Program identifier missing after END PROGRAM.");
        return false;
    }

    cout << "(DONE)" << endl;
    return true;    
}

    bool Decl(istream &in, int &line) 
	{
		LexItem t = Parser::GetNextToken(in, line);

    	if (!Type(in, line)) 
		{
        	ParseError(line, "Invalid type declaration.");
        	return false;
		}

    	t = Parser::GetNextToken(in, line);

    	if (t != DBLCOLON) 
		{
			ParseError(line, "Double colon (::) missing after type.");
        	return false;
    	}

    	// Parse variable list
    	if (!VarList(in, line)) 
		{
			ParseError(line, "Invalid variable list in declaration.");
        	return false;
    	}

    	return true;
	}

	//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
	bool Type(istream &in, int &line) 
	{
		LexItem t = Parser::GetNextToken(in, line);

    	if (t == INTEGER || t == REAL || t == CHARACTER) 
		{
        	// Check for optional length specifier
        	t = Parser::GetNextToken(in, line);

        	if (t == LPAREN) 
			{
            	// Length specifier present, check for LEN and ICONST
            	t = Parser::GetNextToken(in, line);

            	if (t != LEN) 
				{
                	ParseError(line, "Expected LEN keyword.");
                	return false;
            	}

            	t = Parser::GetNextToken(in, line);

            	if (t != EQUAL) 
				{
                	ParseError(line, "Missing equal sign after LEN.");
                	return false;
            	}

            	t = Parser::GetNextToken(in, line);

            	if (t != ICONST) 
				{
                	ParseError(line, "Invalid length specifier.");
                	return false;
            	}

            	t = Parser::GetNextToken(in, line); // Consume closing parenthesis
        	} 
			else 
			{
				Parser::PushBackToken(t);
        	}

        	return true;
    	}

    	ParseError(line, "Invalid type specifier.");
    	return false;
    }

    bool VarList(istream &in, int &line) 
	{
		LexItem t;

    	while (true) 
		{
	        if (!Var(in, line)) 
			{
            	ParseError(line, "Invalid variable in variable list.");
            	return false;
        	}

        	t = Parser::GetNextToken(in, line);

        	if (t == COMMA) 
			{
            	continue;
        	} 

			else if (t == DBLCOLON) 
			{
	            Parser::PushBackToken(t);
            	break;
        	} 
			else if (t == ASSIGN) 
			{
            	if (!Expr(in, line)) 
				{
                	ParseError(line, "Invalid expression in variable initialization.");
                	return false;
           	 	}
        	} 
			else 
			{
            `	ParseError(line, "Invalid token in variable list.");
            	return false;
        	}
    	}

    	return true;
    }

    bool Stmt(istream &in, int &line) 
	{
		LexItem t = Parser::GetNextToken(in, line);

    	if (t == IDENT) 
		{
        	Parser::PushBackToken(t);
        	return AssignStmt(in, line);
    	} 
		else if (t == IF) 
		{
        	Parser::PushBackToken(t);
        	return BlockIfStmt(in, line);
    	} else if (t == PRINT) 
		{
        	Parser::PushBackToken(t);
        	return PrintStmt(in, line);
    	} 
		else 
		{
        	ParseError(line, "Invalid statement.");
        	return false;
    	}
    }

	bool BlockIfStmt(istream &in, int &line) 
	{
		LexItem ifToken = Parser::GetNextToken(in, line);

    	if (ifToken != IF) 
		{
        	ParseError(line, "Expected IF keyword in Block If statement.");
        	return false;
    	}

    	LexItem openParen = Parser::GetNextToken(in, line);

    	if (openParen != LPAREN) 
		{
        	ParseError(line, "Missing opening parenthesis in Block If statement.");
        	return false;
    	}

    	bool relExprResult = RelExpr(in, line);

    	if (!relExprResult) 
		{
        	ParseError(line, "Invalid relational expression in Block If statement.");
        	return false;
    	}

    	LexItem closeParen = Parser::GetNextToken(in, line);

    	if (closeParen != RPAREN) 
		{
        	ParseError(line, "Missing closing parenthesis in Block If statement.");
        	return false;
    	}

    	LexItem thenToken = Parser::GetNextToken(in, line);

    	if (thenToken != THEN) 
		{
        	ParseError(line, "Missing THEN keyword in Block If statement.");
        	return false;
    	}

    	bool stmtBlockResult = Stmt(in, line);

    	if (!stmtBlockResult) 
		{
        	ParseError(line, "Missing statement block in Block If statement.");
        	return false;
    	}

    	LexItem elseToken = Parser::GetNextToken(in, line);

    	if (elseToken == ELSE) 
		{
        	bool elseStmtBlockResult = Stmt(in, line);

        	if (!elseStmtBlockResult) 
			{
            	ParseError(line, "Missing statement block in ELSE clause of Block If statement.");
            	return false;
        	}
    	} 

		else 
		{
        	Parser::PushBackToken(elseToken);
    	}

    	LexItem endIfToken = Parser::GetNextToken(in, line);

    	if (endIfToken != END) 
		{
        	ParseError(line, "Missing END keyword in Block If statement.");
        	return false;
    	}

    	LexItem endIfIdent = Parser::GetNextToken(in, line);

    	if (endIfIdent != IF) 
		{
        	ParseError(line, "Missing IF keyword after END in Block If statement.");
        	return false;
    	}

 		return true;
	}

	bool SimpleIfStmt(istream& in, int& line) 
	{
    	LexItem ifToken = Parser::GetNextToken(in, line);

    	if (ifToken != IF) 
		{
        	ParseError(line, "Expected IF keyword in Simple If statement.");
        	return false;
    	}

    	LexItem openParen = Parser::GetNextToken(in, line);

    	if (openParen != LPAREN) 
		{
        	ParseError(line, "Missing opening parenthesis in Simple If statement.");
        	return false;
    	}

    	bool relExprResult = RelExpr(in, line);

    	if (!relExprResult) 
		{
        	ParseError(line, "Invalid relational expression in Simple If statement.");
        	return false;
    	}

    	LexItem closeParen = Parser::GetNextToken(in, line);

    	if (closeParen != RPAREN) 
		{
        	ParseError(line, "Missing closing parenthesis in Simple If statement.");
        	return false;
    	}

    	bool simpleStmtResult = SimpleStmt(in, line);

    	if (!simpleStmtResult) 
		{
        	ParseError(line, "Missing statement in Simple If statement.");
        	return false;
    	}

    	return true;
	}

	bool SimpleStmt(istream& in, int& line) 
	{
 		LexItem t = Parser::GetNextToken(in, line);
    	Parser::PushBackToken(t);

    	if (t == IDENT) 
		{
        	return AssignStmt(in, line);
    	} 
		
		else if (t == PRINT) 
		{
        	return PrintStmt(in, line);
    	} 
		
		else 
		{
        	ParseError(line, "Invalid statement in Simple statement.");
        	return false;
   		}
	}

	bool AssignStmt(istream& in, int& line) 
	{
    	LexItem var = Var(in, line);

    	if (var == ERR) 
		{
        	ParseError(line, "Invalid variable in assignment statement.");
        	return false;
    	}

    	LexItem assignOp = Parser::GetNextToken(in, line);

    	if (assignOp != ASSIGN) 
		{
        	ParseError(line, "Missing assignment operator in assignment statement.");
        	return false;
    	}

    	bool exprResult = Expr(in, line);

   		if (!exprResult) 
		{
        	ParseError(line, "Invalid expression in assignment statement.");
        	return false;
    	}

    	return true;
	}		

	bool RelExpr(istream& in, int& line) 
	{
    	bool expr1Result = Expr(in, line);

    	if (!expr1Result) 
		{
        	ParseError(line, "Missing left-hand side expression in relational expression.");
        	return false;
    	}

    	LexItem op = Parser::GetNextToken(in, line);

    	if (op != EQ && op != LT && op != GT) 
		{
        	Parser::PushBackToken(op);
        	return true;
    	}

    	bool expr2Result = Expr(in, line);

    	if (!expr2Result) 
		{
        	ParseError(line, "Missing right-hand side expression in relational expression.");
        	return false;
    	}

   		return true;
	}

	LexItem Var(istream& in, int& line) 
	{
    	LexItem var = Parser::GetNextToken(in, line);
    	if (var == IDENT) 
		{
        	return var;
    	} 
		
		else 
		{
        	return LexItem(ERR);
    	}
	}

	bool Expr(istream &in, int &line) 
	{
    	bool multExprResult = MultExpr(in, line);

    	if (!multExprResult) 
		{
        	ParseError(line, "Missing expression in expression.");
        	return false;
    	}

    	LexItem op = Parser::GetNextToken(in, line);
    	while (op == PLUS || op == MINUS || op == CAT) 
		{
        	multExprResult = MultExpr(in, line);
        	if (!multExprResult) 
			{
            	ParseError(line, "Missing expression after operator in expression.");
            	return false;
        	}
        	op = Parser::GetNextToken(in, line);
    	}

    	Parser::PushBackToken(op);

    	return true;
	}

	bool MultExpr(istream &in, int &line) 
	{
    	bool termExprResult = TermExpr(in, line);

    	if (!termExprResult) 
		{
        	ParseError(line, "Missing term in multiplication expression.");
        	return false;
    	}

    	LexItem op = Parser::GetNextToken(in, line);
    	while (op == MULT || op == DIV) 
		{
        	termExprResult = TermExpr(in, line);
        	if (!termExprResult) 
			{
            	ParseError(line, "Missing term after operator in multiplication expression.");
            	return false;
        	}
        	op = Parser::GetNextToken(in, line);
    	}

    	Parser::PushBackToken(op);

    	return true;
	}

	bool TermExpr(istream &in, int &line) 
	{
    	bool sFactorResult = SFactor(in, line);

    	if (!sFactorResult) 
		{
        	ParseError(line, "Missing factor in term expression.");
        	return false;
    	}

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