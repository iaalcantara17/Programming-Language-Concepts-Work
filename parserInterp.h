#include "parserInterp.h"
#include <stack>
#include <map>
#include <iostream>
#include <queue>
#include <string>
using namespace std;

map<string, bool> variableMap;
map<string, Token> tokenMap;

map<string, Value> valuesMap;
queue <Value>* valuesList;

namespace Parser {
    bool tokStored = false;
    LexItem	storedToken;

    static LexItem GetNextToken(istream& in, int& line) {
        if (tokStored==true) { //if true, take it out of stored and return
            tokStored = false;
            return storedToken;
        }
        return getNextToken(in, line);
    }

    static bool PushBackToken(LexItem& tok) {
        storedToken= tok;
        if (tokStored==true) {
            return false;
        }
        tokStored = true;
        return true;
    }
}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string message)
{
    ++error_count;
    cout << line << ": " << message << endl;
}

bool PrintStmt(istream& in, int& line) {
    LexItem t;
    t = Parser::GetNextToken(in, line);
    valuesList = new queue<Value>;

    if (t != DEF) {
        ParseError(line, "Print statement syntax error.");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != COMMA) {
        ParseError(line, "Missing Comma.");
        return false;
    }

    bool ex = ExprList(in, line);
    if (!ex) {
        ParseError(line, "Missing expression after Print Statement");
        return false;
    }

    //taking all out and emptying
    while (!valuesList->empty()) {
        cout << valuesList->front();
        valuesList->pop();
    }
    cout<< "\n";
    return ex;
}

bool ExprList(istream& in, int& line) {
    bool status = false;
    Value valuesListProd;

    status = Expr(in, line, valuesListProd);
    if (!status) {
        ParseError(line, "Missing Expression");
        return false;
    }
    valuesList->push(valuesListProd);
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {

        status = ExprList(in, line);

    }
    else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else {
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}

bool Prog(istream& in, int& line) {
    bool dl = false, sl = false;
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == PROGRAM) {
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == IDENT) {
            dl = Decl(in, line);
            if (!dl)
            {
                ParseError(line, "Incorrect Declaration in Program");
                return false;
            }
            sl = Stmt(in, line);
            if (!sl)
            {
                ParseError(line, "Incorrect Statement in program");
                return false;
            }
            tok = Parser::GetNextToken(in, line);

            if (tok.GetToken() == END) {
                tok = Parser::GetNextToken(in, line);

                if (tok.GetToken() == PROGRAM) {
                    tok = Parser::GetNextToken(in, line);

                    if (tok.GetToken() == IDENT) {
                        cout << "(DONE)" << endl;
                        return true;
                    }
                    else
                    {
                        ParseError(line, "Missing Program Name");
                        return false;
                    }
                }
                else
                {
                    ParseError(line, "Missing PROGRAM at the End");
                    return false;
                }
            }
            else
            {
                ParseError(line, "Missing END of Program");
                return false;
            }
        }
        else
        {
            ParseError(line, "Missing Program name");
            return false;
        }
    }
    else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    else
    {
        ParseError(line, "Missing Program keyword");
        return false;
    }
}


bool Decl(istream& in, int& line) {
    bool status = false;
    LexItem tok;
    string strLen;

    LexItem t = Parser::GetNextToken(in, line);

    if (t == INTEGER || t == REAL || t == CHARACTER) {
        tok = t;

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == DCOLON) {
            status = VarList(in, line, t);

            if (status)
            {
                status = Decl(in, line);
                if (!status)
                {
                    ParseError(line, "Declaration Syntactic Error.");
                    return false;
                }
                return status;
            }
            else
            {
                ParseError(line, "Missing Variables List.");
                return false;
            }
        }
        else if (t == CHARACTER && tok.GetToken() == LPAREN)
        {
            tok = Parser::GetNextToken(in, line);

            if (tok.GetToken() == LEN)
            {
                tok = Parser::GetNextToken(in, line);

                if (tok.GetToken() == ASSOP)
                {
                    tok = Parser::GetNextToken(in, line);

                    if (tok.GetToken() == ICONST)
                    {
                        strLen = tok.GetLexeme();

                        tok = Parser::GetNextToken(in, line);
                        if (tok.GetToken() == RPAREN)
                        {
                            tok = Parser::GetNextToken(in, line);
                            if (tok.GetToken() == DCOLON)
                            {
                                status = VarList(in, line, t, stoi(strLen));

                                if (status)
                                {
                                    status = Decl(in, line);
                                    if (!status)
                                    {
                                        ParseError(line, "Declaration Syntactic Error.");
                                        return false;
                                    }
                                    return status;
                                }
                                else
                                {
                                    ParseError(line, "Missing Variables List.");
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            ParseError(line, "Missing Right Parenthesis for String Length definition.");
                            return false;
                        }

                    }
                    else
                    {
                        ParseError(line, "Incorrect Initialization of a String Length");
                        return false;
                    }
                }
            }
        }
        else
        {
            ParseError(line, "Missing Double Colons");
            return false;
        }
    }
    Parser::PushBackToken(t);
    return true;
}

bool VarList(istream& in, int& line, LexItem& idtok, int strlen) {
    bool status = false, exprstatus = false;
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT)
    {


        identstr = tok.GetLexeme();
        if (variableMap.find(identstr) != variableMap.end()) {
            ParseError(line, "Variable Redefinition");
            return false;
        }
        variableMap[identstr] = false;

        if (idtok == CHARACTER) {

            variableMap[identstr] = true;

            string str(strlen, ' ');

            Value ret(str);

            valuesMap[identstr] = ret;
            tokenMap[identstr] = idtok.GetToken();

        }

    }
    else
    {

        ParseError(line, "Missing Variable Name");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok == ASSOP)
    {
        Value ret;
        exprstatus = Expr(in, line, ret);
        if (!exprstatus)
        {
            ParseError(line, "Incorrect initialization for a variable.");
            return false;
        }
        variableMap[identstr] = true;

        tokenMap[identstr] = idtok.GetToken();


        if (idtok == CHARACTER) {
            if (ret.GetString().length() > strlen) {
                string temp = ret.GetString().substr(0, strlen);
                valuesMap[identstr] = Value(temp);
            }
            else if (ret.GetString().length() < strlen) {
                int spacesToAdd = strlen - ret.GetString().length();

                string space(spacesToAdd, ' ');
                string temp = ret.GetString() + space;
                valuesMap[identstr] = Value(temp);

            }
            else {
                valuesMap[identstr] = ret;
            }
        }
        else if (idtok == REAL) {
            if (ret.IsInt()) {
                double newReal = ret.GetInt();
                valuesMap[identstr] = Value(newReal);
            }
        }
        else {
            valuesMap[identstr] = ret;
        }


        tok = Parser::GetNextToken(in, line);

        if (tok == COMMA) {

            status = VarList(in, line, idtok, strlen);

        }
        else
        {
            Parser::PushBackToken(tok);
            return true;
        }
    }
    else if (tok == COMMA) {

        status = VarList(in, line, idtok, strlen);
    }
    else if (tok == ERR)
    {
        ParseError(line, "Unrecognized Input Pattern");

        return false;
    }
    else {

        Parser::PushBackToken(tok);
        return true;
    }

    return status;
}

bool Stmt(istream& in, int& line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken()) {

        case PRINT:
            status = PrintStmt(in, line);

            if (status)
                status = Stmt(in, line);
            break;

        case IF:
            status = BlockIfStmt(in, line);
            if (status)
                status = Stmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            if (status)
                status = Stmt(in, line);
            break;


        default:
            Parser::PushBackToken(t);
            return true;
    }

    return status;
}

bool SimpleStmt(istream& in, int& line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch (t.GetToken()) {

        case PRINT:
            status = PrintStmt(in, line);

            if (!status)
            {
                ParseError(line, "Incorrect Print Statement");
                return false;
            }
            cout << "Print statement in a Simple If statement." << endl;
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            if (!status)
            {
                ParseError(line, "Incorrect Assignent Statement");
                return false;
            }
            cout << "Assignment statement in a Simple If statement." << endl;

            break;


        default:
            Parser::PushBackToken(t);
            return true;
    }

    return status;
}

bool BlockIfStmt(istream& in, int& line) {

    bool ex = false, status;
    LexItem t;
    Value r;

    t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }
    ex = RelExpr(in, line, r);
    if (!ex) {
        ParseError(line, "Missing if statement condition");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    if (r.IsBool() && r.GetBool() == false) {

        while (t != END && t != ELSE) {
            t = Parser::GetNextToken(in, line);

        }
        Parser::PushBackToken(t);

    }
    else {



        t = Parser::GetNextToken(in, line);
        if (t != THEN)
        {
            Parser::PushBackToken(t);

            status = SimpleStmt(in, line);
            if (status)
            {
                return true;
            }
            else
            {
                ParseError(line, "If-Stmt Syntax Error");
                return false;
            }

        }

        status = Stmt(in, line);
        if (!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Then-Part");
            return false;
        }
    }
    t = Parser::GetNextToken(in, line);
    if (t == ELSE) {
        status = Stmt(in, line);
        if (!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        else
            t = Parser::GetNextToken(in, line);

    }


    if (t != END) {

        ParseError(line, "Missing END of IF");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t == IF) {
        return true;
    }

    Parser::PushBackToken(t);
    ParseError(line, "Missing IF at End of IF statement");
    return false;
}


bool AssignStmt(istream& in, int& line) {

    bool varstatus = false, status = false;
    LexItem t;
    LexItem h;
    varstatus = Var(in, line, h);

    Value r;
    if (varstatus) {
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP) {
            status = Expr(in, line, r);
            if (!status) {
                ParseError(line, "Missing Expression in Assignment Statment");
                return status;
            }

            variableMap[h.GetLexeme()] = true;
            if (tokenMap[h.GetLexeme()] == CHARACTER) {
                if (r.IsString() == false) {
                    ParseError(line = line -2, "Illegal mixed-mode assignment operation");
                    return false;
                }

                int strlen = valuesMap[h.GetLexeme()].GetString().length();
                string identstr = h.GetLexeme();

                if (r.GetString().length() > strlen) {
                    string temp = r.GetString().substr(0, strlen);
                    valuesMap[identstr] = Value(temp);
                }
                else if (r.GetString().length() < strlen) {
                    int spacesToAdd = strlen - r.GetString().length();

                    string space(spacesToAdd, ' ');
                    string temp = r.GetString() + space;
                    valuesMap[identstr] = Value(temp);

                }
                else {
                    valuesMap[identstr] = r;
                }
            }
            else {
                valuesMap[h.GetLexeme()] = r;

            }





        }
        else if (t.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    }
    else {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}
bool Var(istream& in, int& line, LexItem& idtok) {
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IDENT) {
        idtok = tok;
        identstr = tok.GetLexeme();
        return true;
    }
    else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    return false;
}

bool RelExpr(istream& in, int& line, Value& retVal) {
    bool t1 = Expr(in, line, retVal);
    LexItem tok;
    Value eqVal;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if (tok == EQ || tok == LTHAN || tok == GTHAN)
    {
        t1 = Expr(in, line, eqVal);
        if (!t1)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if (tok == EQ) {
            retVal = retVal == eqVal;
        }
        if (tok == LTHAN) {
            retVal = retVal < eqVal;
        }
        if (tok == GTHAN) {
            retVal = retVal > eqVal;
        }


    }

    if (retVal.IsErr()) {
        ParseError(line, "Illegal operand types for a Relational operation");
        return false;
    }

    return true;
}

bool Expr(istream& in, int& line, Value& retVal) {
    Value l;
    Value r;

    bool t1 = MultExpr(in, line, l);
    LexItem tok;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == PLUS || tok == MINUS || tok == CAT)
    {
        t1 = MultExpr(in, line, r);
        if (!t1)
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if (tok == PLUS) {
            l = l + r;
        }
        if (tok == MINUS) {
            l = l - r;
        }
        if (tok == CAT) {
            l = l.Catenate(r);
        }


        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }

    retVal = l;

    if (retVal.IsErr()) {
        ParseError(line = line -2, "ERRROOOR");
        return false;
    }
    Parser::PushBackToken(tok);
    return true;
}


bool MultExpr(istream& in, int& line, Value& retVal) {

    Value l;
    Value r;
    bool t1 = TermExpr(in, line, l);
    LexItem tok;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == MULT || tok == DIV)
    {
        t1 = TermExpr(in, line, r);

        if (!t1) {
            ParseError(line, "Missing operand after operator");
            return false;
        }


        if (tok == MULT) {
            l = l * r;
        }

        if (tok == DIV) {
            l = l / r;

            if (l.IsErr()) {
                ParseError(line = line - 1, "Run-Time Error-Illegal division by Zero");
                return false;
            }

        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    retVal = l;

    if (retVal.IsErr()) {
        ParseError(line , "ERRROOOR");
        return false;
    }
    Parser::PushBackToken(tok);
    return true;
}

bool TermExpr(istream& in, int& line, Value& retVal) {

    bool t1 = SFactor(in, line, retVal);
    LexItem tok;

    if (!t1) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    while (tok == POW)
    {
        t1 = SFactor(in, line, retVal);

        if (!t1) {
            ParseError(line, "Missing exponent operand");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }

    }
    if (retVal.IsErr()) {
        ParseError(line, "ERRROOOR");
        return false;
    }
    Parser::PushBackToken(tok);
    return true;
}

bool SFactor(istream& in, int& line, Value& retVal) {
    LexItem t = Parser::GetNextToken(in, line);

    bool status;
    int sign = 0;
    if (t == MINUS)
    {
        sign = -1;
    }
    else if (t == PLUS)
    {
        sign = 1;
    }
    else
        Parser::PushBackToken(t);
    status = Factor(in, line, sign, retVal);
    return status;
}

bool Factor(istream& in, int& line, int sign, Value& retVal) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == IDENT) {

        string lexeme = tok.GetLexeme();
        if (!(variableMap.find(lexeme)->second))
        {
            ParseError(line, "Using Undefined Variable");
            return false;
        }

        retVal = valuesMap[lexeme];


        if (sign == 1 || sign == -1) {
            if (valuesMap[lexeme].IsInt()) {
                retVal = Value(sign * (valuesMap[lexeme].GetInt()));
            }
            if (valuesMap[lexeme].IsReal()) {
                retVal = Value(sign * (valuesMap[lexeme].GetReal()));
            }
            if (valuesMap[lexeme].IsString()) {
                ParseError(line, "Illegal Operand Type for Sign Operator");
                return false;
            }
        }
        return true;
    }
    else if (tok == ICONST) {
        if (sign == -1) {
            Value x = Value(stoi(tok.GetLexeme()) * (-1));
            retVal = x;

            return true;
        }
        else {
            Value x = Value(stoi(tok.GetLexeme()));
            retVal = x;

            return true;
        }
    }
    else if (tok == SCONST) {
        Value x = Value(tok.GetLexeme());
        retVal = x;
        return true;
    }
    else if (tok == RCONST) {
        Value x = Value(stod(tok.GetLexeme()));
        retVal = x;
        return true;
    }
    else if (tok == LPAREN) {
        bool ex = Expr(in, line, retVal);
        if (!ex) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if (Parser::GetNextToken(in, line) == RPAREN)
            return ex;
        else
        {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }
    else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }


    return false;
}