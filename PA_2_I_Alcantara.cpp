//Israel Alcantara

#include "parser.h"
#include <map>
#include <string>
#include <iostream>

using namespace std;

map<string, bool> defVar;
map<string, Token> SymTable;

string s;

bool tempVar;
bool vardef = false;

namespace Parser 
{
    bool pushed_back = false;
    LexItem pushed_token;

    static LexItem GetNextToken (istream& in, int& line) 
    {
        if( pushed_back ) 
        {
            pushed_back = false;
            return pushed_token;
        }
        
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) 
    {
        if( pushed_back ) 
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

void ParseError(int line, string msg) 
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool PrintStmt(istream& in, int& line) 
{
    LexItem someTokenOfMine;
    someTokenOfMine = Parser::GetNextToken(in, line);
    
    if (someTokenOfMine != PRINT) 
    {
        Parser::PushBackToken(someTokenOfMine);
        return false;
    }

    someTokenOfMine = Parser::GetNextToken(in, line);
    
    if(someTokenOfMine != DEF) 
    {
        Parser::PushBackToken(someTokenOfMine);
        ParseError(line, "Sytax error in print statment.");
        return false;
    }

    someTokenOfMine = Parser::GetNextToken(in, line);

    if(someTokenOfMine != COMMA) 
    {
        Parser::PushBackToken(someTokenOfMine);
        ParseError(line, "Sytax error due to missing comma.");
        return false;
    }

    bool someExpr = ExprList(in, line);

    if(!someExpr) 
    {
        ParseError(line, "Missing or incomplete expression after Print Statement");
        return false;
    }
    
    return someExpr;
}

bool Decl(istream& in, int& line) 
{
    bool someBool = Type(in, line);
    
    if (!someBool) 
    {
        return false;
    }

    LexItem someToken = Parser::GetNextToken(in, line);
    if (someToken == DCOLON) 
    {
        bool sBool = VarList(in, line);
        if (!sBool) 
        {
            ParseError(line, "Missing Expression from DCOLON");
            return false;
        }
        
        return true;
    }
    
    else 
    {
        Parser::PushBackToken(someToken);
        return false;
    }
}

bool VarList(istream& in, int& line) 
{
    LexItem someToken;
    
    bool randomBool = false;
    
    someToken = Parser::GetNextToken(in, line);
    
    string d = someToken.GetLexeme();
    
    Parser::PushBackToken(someToken);
    
    randomBool = Var(in, line);
    
    if (!randomBool) 
    {
        ParseError(line, "Missing var experssion");
        return false;
    }
    
    if (vardef) 
    {
        ParseError(line, "Variable Redefinition");
        return false;
    }
    
    someToken = Parser::GetNextToken(in, line);
    
    if (someToken == COMMA) 
    {
        randomBool = VarList(in, line);
        
        if (!randomBool) 
        {
            ParseError(line, "variable expected after comma");
            return false;
        }
    }
    
    else if (someToken == ASSOP) 
    {
        randomBool = Expr(in, line);
        cout << "Initialization of the variable " << d << " in the declaration statement." << endl;
        
        if (tempVar) 
        {
            cout << "Definition of Strings with length of " << s << " in declaration statement." << endl;
        }
        
        if (!randomBool) 
        {
            ParseError(line, "20");
            return false;
        }
        
        someToken = Parser::GetNextToken(in, line);
        
        if (someToken == COMMA) 
        {
            randomBool = VarList(in, line);
            
            if (!randomBool) 
            {
                ParseError(line, "Something went wrong!");
                return false;
            }
            
            return true;
        }
        
        Parser::PushBackToken(someToken);
    }
    
    else 
    {
        Parser::PushBackToken(someToken);
    }
    
    return randomBool;
}

bool Stmt(istream& in, int& line)
{
    bool firstBool = false;

    firstBool = AssignStmt(in, line);
    
    if (!firstBool) 
    {
        bool secondBool = false;
        secondBool = BlockIfStmt(in, line);
        
        if (!secondBool)
        {
            bool thirdBool = false;
            thirdBool = PrintStmt(in, line);
            
            if (!thirdBool) 
            {
                return false;
            }
        }
    }
    
    return true;
}

int myVal = 1;

bool BlockIfStmt(istream& in, int& line) 
{
    LexItem myTok = Parser::GetNextToken(in, line);
    
    if (myTok != IF) 
    {
        Parser::PushBackToken(myTok);
        
        return false;
    }

    myTok = Parser::GetNextToken(in, line);
    
    if (myTok != LPAREN) 
    {
        Parser::PushBackToken(myTok);
        ParseError(line, "Expected '(' after 'IF' statement");
        
        return false;
    }

    bool myBoolOne = false;
    myBoolOne = RelExpr(in, line);
    
    if (!myBoolOne) 
    {
        ParseError(line, "Missing expression in 'IF' statement");
        return false;
    }

    myTok = Parser::GetNextToken(in, line);
    
    if (myTok != RPAREN) 
    {
        Parser::PushBackToken(myTok);
        ParseError(line, "Expected ')' after expression in the 'IF' block");
        
        return false;
    }

    myTok = Parser::GetNextToken(in, line);
    
    if (myTok != THEN) 
    {
        cout << "Print statement in a Simple If statement." << endl;
        Parser::PushBackToken(myTok);
        
        return true;
    }
    
    myVal++;
    myBoolOne = BlockIfStmt(in, line);
    myVal--;

    while (true)
    {
        myBoolOne = Stmt(in, line);
        myTok = Parser::GetNextToken(in, line);
        
        if (myTok == END) 
        {
            LexItem nextToken = Parser::GetNextToken(in, line);
            if (nextToken == IF) 
            
            {
                cout << "End of Block If statement at nesting level " << myVal << endl;
                break;
            }
            
            else 
            {
                Parser::PushBackToken(nextToken);
                return false;
            }
        }
        
        else if (myTok == ELSE) 
        {
            while (true) 
            {
                myBoolOne = Stmt(in, line);
                myTok = Parser::GetNextToken(in, line);
                    
                if (myTok == END) 
                {
                    LexItem nextToken = Parser::GetNextToken(in, line);
                    
                    if (nextToken == IF) 
                    {
                        cout << "End of Block If statement at nesting level " << myVal << endl;
                        break;
                    }
                    
                    else 
                    {
                        Parser::PushBackToken(nextToken);
                        ParseError(line, "After 'IF' keyword 'END' is expected");
                        break;
                    }
                }
                
                Parser::PushBackToken(myTok);
                
                if (!myBoolOne) break;
            }
        }
        
        Parser::PushBackToken(myTok);
        
        if (!myBoolOne) break;
    }
    
    return myBoolOne;
}

bool ExprList(istream& in, int& line) 
{
    bool myStatus;

    myStatus = Expr(in, line);
    
    if(!myStatus)
    {
        ParseError(line, "Missing Expression(s)");
        return false;
    }

    LexItem someRandomToken = Parser::GetNextToken(in, line);

    if (someRandomToken == COMMA) 
    {
        myStatus = ExprList(in, line);
    }
    
    else if(someRandomToken.GetToken() == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << someRandomToken.GetLexeme() << ")" << endl;
        
        return false;
    }
        
    else
    {
        Parser::PushBackToken(someRandomToken);
        
        return true;
    }
    
    return myStatus;
}

bool Type(istream& in, int& line) 
{
    LexItem someTok;
    someTok = Parser::GetNextToken(in, line);
    if (someTok == INTEGER || someTok == REAL) 
    {
        return true;
    }
    else if (someTok == CHARACTER) 
    {
        someTok = Parser::GetNextToken(in, line);
        
        if (someTok == LPAREN) 
        {
            someTok = Parser::GetNextToken(in, line);
            
            if (someTok == LEN) 
            {
                someTok = Parser::GetNextToken(in, line);
                
                if (someTok == ASSOP) 
                {
                    someTok = Parser::GetNextToken(in, line);
                    
                    if (someTok == ICONST) 
                    {
                        s = someTok.GetLexeme();
                        
                        someTok = Parser::GetNextToken(in, line);
                        
                        if (someTok == RPAREN) 
                        {
                            tempVar = true;
                            return true;
                        }
                    }
                }
            }
            
            else 
            {
                ParseError(line, "Missing Expression in LEN");
                return false;
            }
        }
        
        Parser::PushBackToken(someTok);
        
        return true;
    }
    
    else 
    {
        Parser::PushBackToken(someTok);
        
        return false;
    }
}

bool Expr(istream& in, int& line) 
{
    bool myBool = false;
    myBool = MultExpr(in, line);
    
    if (!myBool) 
    {
        ParseError(line, "Syntax Error: Missing Expression in expr");
        
        return false;
    }

    LexItem myGoodToken = Parser::GetNextToken(in, line);
    
    if (myGoodToken == PLUS || myGoodToken == MINUS || myGoodToken == CAT) 
    {
        myBool = Expr(in, line);
    }
    
    else 
    {
        Parser::PushBackToken(myGoodToken);
    }

    return myBool;
}

bool AssignStmt(istream& in, int& line) 
{
    bool isAssigned;
    isAssigned = Var(in, line);
    
    if (!isAssigned) 
    {
        return false;
    }

    LexItem myTok = Parser::GetNextToken(in, line);
    
    if (myTok != ASSOP) 
    {
        ParseError(line, "No Assignment Operator");
        return false;
    }

    bool toExpr = Expr(in, line);
    
    if (!toExpr) 
    {
        ParseError(line, "Invalid expression in assignment statement");
        return false;
    }

    return true;
}

bool RelExpr(istream& in, int& line) 
{
    bool myBool = false;
    myBool = Expr(in, line);
    
    if (!myBool)
    {
        ParseError(line, "Missing Expression within the relexpr expression.");
        return false;
    }

    LexItem myToken = Parser::GetNextToken(in, line);
    
    if (myToken == EQ || myToken == LTHAN || myToken == GTHAN) 
    {
        bool again = false;
        again = Expr(in, line);
        
        if (!again) 
        {
            ParseError(line, "Missing Expression in again relexpr statment.");
            
            return false;
        }
        
        return true;
    }
    Parser::PushBackToken(myToken);
    
    return myBool;
}

bool MultExpr(istream& in, int& line) 
{
    bool myBool = false;
    myBool = TermExpr(in, line); // tempExpr boolean value
    
    if (!myBool) 
    {
        ParseError(line, "Syntax Error: Missing Expression in multexpr expression.");
        
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    
    if (t == MULT || t == DIV) 
    {
        myBool = MultExpr(in, line);
    }
    
    else 
    {
        Parser::PushBackToken(t);
        
        return true;
    }

    return myBool;
}

bool TermExpr(istream& in, int& line) 
{
    bool myCurrentTerm = false;
    myCurrentTerm = SFactor(in, line);
    
    if (!myCurrentTerm) 
    {
        ParseError(line, "Syntax Error: Missing Expression in termExpr");
        return false;
    }

    LexItem myToken = Parser::GetNextToken(in, line);
    
    if (myToken == POW) 
    {
        myCurrentTerm = TermExpr(in, line);
        
        if (!myCurrentTerm) 
        {
            ParseError(line, "Syntax Error: Missing Expression after pow statment");
            
            return false;
        }
    }
    
    else 
    {
        Parser::PushBackToken(myToken);
    }

    return myCurrentTerm;
}

bool SFactor(istream& in, int& line) 
{
    bool myFactored;
    myFactored = Factor(in, line, 1);
    
    if (myFactored) 
    {
        return true;
    }
    
    return false;
}

bool Factor(istream& in, int& line, int sign) 
{
    bool someBool;
    LexItem myTok = Parser::GetNextToken(in, line);
    
    if (myTok == MINUS) 
    {
        return Factor(in, line, -sign);
    }
    if (myTok == IDENT) 
    {
        auto it = defVar.find(myTok.GetLexeme());
        if (it != defVar.end()) 
        {
            return true;
        }
        
        ParseError(line, "Error: Use of an undefined variable");
        
        return false;
    }
    if (myTok == ICONST || myTok == RCONST || myTok == SCONST) 
    {
        return true;
    }
    if (myTok == LPAREN) {
        
        someBool = Expr(in, line);

        if (!someBool) 
        {
            ParseError(line, "Error: Missing left LPAREN in statment");
            
            return false;
        }
        
        myTok = Parser::GetNextToken(in, line);
        
        if (myTok == RPAREN) 
        {
            return true;
        }
    }
    
    else 
    {
        Parser::PushBackToken(myTok);
    }

    return false;
}

bool Var(istream& in, int& line) 
{
    LexItem myToken;
    myToken = Parser::GetNextToken(in, line);
    
    if (myToken == IDENT) 
    {
        auto it = defVar.find(myToken.GetLexeme());
        
        if (it != defVar.end()) 
        {
            vardef = true;
        }
        
        defVar[myToken.GetLexeme()] = true;
        
        return true;
    }
    
    else 
    {
        Parser::PushBackToken(myToken);
        
        return false;
    }
}

bool Prog(istream& in, int& line) 
{
    bool myBool = false;
    LexItem myToken = Parser::GetNextToken(in, line);
    
    if (myToken != PROGRAM) 
    {
        ParseError(line, "Expected PROGRAM");
        
        return false;
    }

    myToken = Parser::GetNextToken(in, line);
    
    if (myToken != IDENT) 
    {
        ParseError(line, "Expected IDENT after PROGRAM Prog");
        
        return false;
    }

    while (true) 
    {
        myBool = Decl(in, line);
        
        if (!myBool)
        {
            break;
        }
    }

    while (true) 
    {
        myBool = Stmt(in, line);
        
        if (!myBool) 
        {
            break;
        }
    }

    myToken = Parser::GetNextToken(in, line);
    
    if (myToken != END) 
    {
        ParseError(line, "Expected 'END' keyword");
        
        return false;
    }

    myToken = Parser::GetNextToken(in, line);
    
    if (myToken != PROGRAM) 
    {
        ParseError(line, "Expected 'PROGRAM' keyword after 'END'");
        
        return false;
    }

    myToken = Parser::GetNextToken(in, line);
    
    if (myToken != IDENT) 
    {
        ParseError(line, "Expected identifier after 'END PROGRAM'");
        
        return false;
    }

    cout << "(DONE)" << endl;
    return true;
}