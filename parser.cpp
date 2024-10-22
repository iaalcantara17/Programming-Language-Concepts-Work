//Israel Alcantara

#include "parser.h"

#include <vector>
#include <string>
#include <unordered_map>


map <string, bool> defVar;
map <string, Token> SymTable;
string s;
bool tempVar;
bool vardef = false;

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

bool Prog(istream& in, int& line) 
{
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok != PROGRAM) 
    {
        ParseError(line, "PROGRAM keyword missing.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok != IDENT) 
    {
        ParseError(line, "Program identifier missing.");
        return false;
    }

    while (true) 
    {
        Parser::PushBackToken(tok);
        if (!Decl(in, line))
            break;
    }

    while (true) 
    {
        Parser::PushBackToken(tok);
        if (!Stmt(in, line))
            break;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok != END) 
    {
        ParseError(line, "END keyword missing.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok != PROGRAM) 
    {
        ParseError(line, "PROGRAM keyword missing after END.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok != IDENT) 
    {
        ParseError(line, "Program identifier missing after END PROGRAM.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != DONE) 
    {
        ParseError(line, "END keyword missing at the end of the program.");
        return false;
    }

    cout << "(DONE)" << endl;
    return true;
}

bool Decl(istream& in, int& line) 
{
	LexItem tok = Parser::GetNextToken(in, line);

	if (!Type(in, line)) 
	{
    	ParseError(line, "Invalid type declaration.");
    	return false;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok != DCOLON) 
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
bool Type(istream& in, int& line) 
{
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == INTEGER || tok == REAL || tok == CHARACTER) 
	{
    	// Check for optional length specifier
    	tok = Parser::GetNextToken(in, line);

    	if (tok == LPAREN) 
		{
           	// Length specifier present, check for LEN and ICONST
           	tok = Parser::GetNextToken(in, line);

           	if (tok != LEN) 
			{
              	ParseError(line, "Expected LEN keyword.");
              	return false;
           	}

           	tok = Parser::GetNextToken(in, line);

          	if (tok != EQ) 
			{
               	ParseError(line, "Missing equal sign after LEN.");
              	return false;
          	}

          	tok = Parser::GetNextToken(in, line);

         	if (tok != ICONST) 
			{
               	ParseError(line, "Invalid length specifier.");
               	return false;
        	}

        	tok = Parser::GetNextToken(in, line); // Consume closing parenthesis
        } 
			
			else 
		{
			Parser::PushBackToken(tok);
        }

        return true;
    }

    ParseError(line, "Invalid type specifier.");
    return false;
}

bool VarList(istream& in, int& line) 
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

		else if (t == DCOLON) 
		{
	    	Parser::PushBackToken(t);
        	break;
        } 

		else if (t == ASSOP) 
		{
            if (!Expr(in, line)) 
			{
            	ParseError(line, "Invalid expression in variable initialization.");
            	return false;
           	}
        }

		else 
		{
            ParseError(line, "Invalid token in variable list.");
            return false;
        }
    }

    return true;
}

bool Stmt(istream& in, int& line) 
{
	LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IDENT) 
	{
    	Parser::PushBackToken(tok);
    	return AssignStmt(in, line);
    }

	else if (tok == IF) 
	{
        Parser::PushBackToken(tok);
        return SimpleIfStmt(in, line);
    } 
	
	else if (tok == PRINT) 
	{
        Parser::PushBackToken(tok);
        return PrintStmt(in, line);
    }

	else 
	{
    	ParseError(line, "Invalid statement.");
    	return false;
    }
}

bool BlockIfStmt(istream& in, int& line) 
{
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok != IF) 
    {
        Parser::PushBackToken(tok);
        return false; 
    }

    if (!RelExpr(in, line)) 
    {
        ParseError(line, "Invalid relational expression in IF statement.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != THEN) 
    {
        ParseError(line, "Missing THEN keyword after IF condition.");
        return false;
    }

    while (true) 
    {
        tok = Parser::GetNextToken(in, line);
        if (tok == END) 
        {
            tok = Parser::GetNextToken(in, line);
            if (tok == IF) 
            {
                return true;
            } 
            
            else 
            {
                ParseError(line, "Missing IF at End of IF statement.");
                return false;
            }
        }
        
        Parser::PushBackToken(tok);

        if (!Stmt(in, line)) 
        {
            return false;
        }
    }

    return true;
}

bool SimpleIfStmt(istream& in, int& line) 
{
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok != IF) 
    {
        Parser::PushBackToken(tok);
        return false; 
    }

    if (!RelExpr(in, line)) 
    {
        ParseError(line, "Invalid relational expression in IF statement.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != THEN) 
    {
        ParseError(line, "Missing THEN keyword after IF condition.");
        return false;
    }

    if (!Stmt(in, line)) 
    {
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
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok != IDENT) 
    {
        Parser::PushBackToken(tok);
        return false; 
    }

    tok = Parser::GetNextToken(in, line);
    
    if (tok != ASSOP)
    {
        ParseError(line, "Missing assignment operator in assignment statement.");
        return false;
    }

    if (!Expr(in, line)) 
    {
        ParseError(line, "Missing expression in assignment statement.");
        return false;
    }

    return true;
}		

bool RelExpr(istream& in, int& line) 
{
    if (!Expr(in, line))
        return false;

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == DONE) 
    {
        ParseError(line, "Missing right parenthesis in relational expression.");
        return false;
    }

    if (tok == GTHAN || tok == LTHAN || tok == EQ) 
    {
        if (!Expr(in, line))
            return false;
    } 
    
    else 
    {
        ParseError(line, "Missing relational operator in relational expression.");
        return false;
    }

    return true;
}

bool Var(istream& in, int& line) 
{
	LexItem t = Parser::GetNextToken(in, line);

	t = id_or_kw("", line);
    
    if (t == ERR)
    {
        ParseError(line, "Missing identifier after var");
        return false;
    }

	return true;
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

bool MultExpr(istream& in, int& line) 
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
	while (op == POW) 
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
    	bool factorResult = Factor(in, line, 0);
    	if (!factorResult) 
		{
        	ParseError(line, "Missing factor after unary operator.");
           	return false;
    	}
	} 

	else 
	{
    	Parser::PushBackToken(op);
    	bool factorResult = Factor(in, line, 0);
    	if (!factorResult) 
		{
        	ParseError(line, "Invalid factor.");
        	return false;
    	}
	}

	return true;
}

bool Factor(istream& in, int& line, int sign) 
{
	 LexItem item = Parser::GetNextToken(in, line);

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
bool PrintStmt(istream& in, int& line) 
{
    LexItem tok;
    tok = Parser::GetNextToken(in, line);

    if (tok != PRINT) 
    {
        Parser::PushBackToken(tok);
        return false; 
    }
    
    tok = Parser::GetNextToken(in, line);
    
    if (tok != DEF)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Syntax error in print statement.");
        return false;
    }
    
    tok = Parser::GetNextToken(in, line);
    if (tok != COMMA) 
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Syntax error due to a missing comma.");
        return false;
    }
    
    bool someExpr = ExprList(in, line);
    
    if(!someExpr)
    {
        ParseError(line, "Missing or incomplete expression after Print Statement.");
        return false;
    }
    
    return someExpr;
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