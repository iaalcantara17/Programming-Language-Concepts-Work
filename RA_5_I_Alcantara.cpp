//Israel Alcantara

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <map>

#include "lex.h"

using namespace std;

static map <string, Token> keyWords =
    {
        {"PROGRAM", PROGRAM},
        {"END", END},
        {"ELSE", ELSE},
        {"IF", IF},
        {"INTEGER", INTEGER},
        {"REAL", REAL},
        {"CHARACTER", CHARACTER},
        {"PRINT", PRINT},
        {"LEN", LEN}
    };

map <Token, string> tokenStrings = 
{
    {ICONST, "ICONST"},
    {RCONST, "RCONST"},
    {BCONST, "BCONST"},
    {SCONST, "SCONST"},
    {IDENT, "IDENT"},
    {ERR, "ERR"},
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {MULT, "MULT"},
    {DIV, "DIV"},
    {POW, "POW"},
    {ASSOP, "ASSOP"},
    {EQ, "EQ"},
    {LTHAN, "LTHAN"},
    {GTHAN, "GTHAN"},
    {CAT, "CAT"},
    {COMMA, "COMMA"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {DCOLON, "DCOLON"},
    {DOT, "DOT"},
    {DEF, "DEF"},
    {PROGRAM, "PROGRAM"},
    {END, "END"},
    {ELSE, "ELSE"},
    {IF, "IF"},
    {INTEGER, "INTEGER"},
    {REAL, "REAL"},
    {CHARACTER, "CHARACTER"},
    {PRINT, "PRINT"},
    {LEN, "LEN"},
    {THEN, "THEN"}
};

map <Token, string> delim =
{
    {COMMA, "COMMA"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {DCOLON, "DCOLON"},
    {DOT, "DOT"},
    {DEF, "DEF"}
};

map <Token, string> op =
{
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {MULT, "MULT"},
    {DIV, "DIV"},
    {POW, "POW"},
    {ASSOP, "ASSOP"},
    {EQ, "EQ"},
    {LTHAN, "LTHAN"},
    {GTHAN, "GTHAN"},
    {CAT, "CAT"},
};

static map <string, Token> allTokens =
    {
        {"PLUS", PLUS},
        {"MINUS", MINUS},
        {"MULT", MULT},
        {"DIV", DIV},
        {"POW", POW},
        {"ASSOP", ASSOP},
        {"EQ", EQ},
        {"LTHAN", LTHAN},
        {"GTHAN", GTHAN},
        {"CAT", CAT},
        {"COMMA", COMMA},
        {"LPAREN", LPAREN},
        {"RPAREN", RPAREN},
        {"DCOLON", DCOLON},
        {"DOT", DOT},
        {"DEF", DEF}
    };

LexItem id_or_kw(const string& lexeme, int linenum)
{
    auto keyword = keyWords.find(lexeme);
 
    string tokenString = "";
    Token token = IDENT;

    if(keyword == keyWords.end()) 
    {
        return LexItem(token, lexeme, linenum); 
    }
    else
    {
        Token token = keyword->second; 
        return LexItem(token, lexeme, linenum);
    }
}

ostream& operator<<(ostream& out, const LexItem& tok)
{
    if(tok.GetToken() == ICONST || tok.GetToken() == RCONST || tok.GetToken() == BCONST)
    {
        out << tokenStrings[tok.GetToken()] << ": " << '(' << tok.GetLexeme() << ')';
    }
    else if(tok.GetToken() == IDENT)
    {
        out << tokenStrings[tok.GetToken()] << ": " << "\'" << tok.GetLexeme() << '\'';
    }
    else if(tok.GetToken() == SCONST)
    {
        out << tokenStrings[tok.GetToken()] << ": \"" << tok.GetLexeme() << '\"';
    }
    else if(tok.GetToken() == ERR)
    {
        out << tokenStrings[tok.GetToken()] << ": {" << tok.GetLexeme() << "} " << "Error at line no. " << tok.GetLinenum();
    }
    else if (tok.GetToken() == PROGRAM || tok.GetToken() == END || tok.GetToken() == ELSE || tok.GetToken() == IF || tok.GetToken() == INTEGER || tok.GetToken() == REAL || tok.GetToken() == CHARACTER || tok.GetToken() == PRINT || tok.GetToken() == LEN || tok.GetToken() == THEN)
    {
        out <<  tokenStrings[tok.GetToken()] << ": " << "Keyword";
    }
    else 
    {
        Token tokenValue = tok.GetToken();
        auto it = delim.find(tokenValue);

        if (it != delim.end())
        {
            out << delim[tok.GetToken()] << ": Delimiter Character " << "\"" << tok.GetLexeme() << "\"";
        }
        else
        {
            out << op[tok.GetToken()] << ": Operator Symbol " << "\"" << tok.GetLexeme() << "\"";
        }
    }
    return out;
}  