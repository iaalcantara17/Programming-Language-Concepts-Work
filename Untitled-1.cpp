// Israel Alcantara

#include "lex.h"
#include <map>
#include <set>

std::map<Token, std::string> Toks =
{
    {IF,            "IF"},
    {ELSE,          "ELSE"},
    {PRINT,         "PRINT"},
    {INTEGER,       "INTEGER"},
    {REAL,          "REAL"},
    {CHARACTER,     "CHARACTER"},
    {END,           "END"},
    {THEN,          "THEN"},
    {PROGRAM,       "PROGRAM"},
    {LEN,           "LEN"},
    {IDENT,         "IDENT"},
    {ICONST,        "ICONST"},
    {RCONST,        "RCONST"},
    {SCONST,        "SCONST"},
    {PLUS,          "PLUS"},
    {MINUS,         "MINUS"},
    {MULT,          "MULT"},
    {DIV,           "DIV"},
    {ASSOP,         "ASSOP"},
    {EQ,            "EQ"},
    {POW,           "POW"},
    {GTHAN,         "GTHAN"},
    {LTHAN,         "LTHAN"},
    {CAT,           "CAT"},
    {COMMA,         "COMMA"},
    {LPAREN,        "LPAREN"},
    {RPAREN,        "RPAREN"},
    {DOT,           "DOT"},
    {DCOLON,        "DCOLON"},
    {DEF,           "DEF"},
    {ERR,           "ERR"},
    {DONE,          "DONE"},
};

std::map<std::string, Token> keywords =
{
    {"IF",          IF},
    {"ELSE",        ELSE},
    {"PRINT",       PRINT},
    {"INTEGER",     INTEGER},
    {"REAL",        REAL},
    {"CHARACTER",   CHARACTER},
    {"END",         END},
    {"THEN",        THEN},
    {"PROGRAM",     PROGRAM},
    {"LEN",         LEN},
};

std::set<std::string> uniqueIdentifiers;
std::set<std::string> uniqueKeywords;
std::set<int> uniqueIntegers;
std::set<double> uniqueReals;
std::set<std::string> uniqueStrings;

ostream& operator<<(ostream& out, const LexItem& tok)
{
    Token type = tok.GetToken();

    out << Toks[type];

    if (type == IDENT)
    {
        out << ": '" << tok.GetLexeme() << "'";
        uniqueIdentifiers.insert(tok.GetLexeme());
    }
    else if (type == SCONST)
    {
        out << ": \"" << tok.GetLexeme() << "\"";
        uniqueStrings.insert(tok.GetLexeme());
    }
    else if (type == ICONST || type == RCONST)
    {
        out << ": (" << tok.GetLexeme() << ")";
        if (type == ICONST)
            uniqueIntegers.insert(std::stoi(tok.GetLexeme()));
        else
            uniqueReals.insert(std::stod(tok.GetLexeme()));
    }
    return out;
}

LexItem id_or_kw(const std::string& lexeme, int linenum)
{
    std::string out;
    int i = 0;
    while (lexeme[i] != '\0')
    {
        out += (char)toupper(lexeme[i++]);
    }

    if (keywords.find(out) == keywords.end())
    {
        return LexItem(IDENT, lexeme, linenum);
    }
    else
    {
        uniqueKeywords.insert(lexeme);
        return LexItem(keywords[out], lexeme, linenum);
    }
}

LexItem getNextToken(std::istream& in, int& linenum)
{
    enum TokState
    {
        START, INID, ININT, INREAL, INSTRING, INCOMMENT,
    } lexstate = START;

    std::string lexeme;
    char ch;

    char stringStartedWith = '\0';

    while (in.get(ch))
    {
        switch (lexstate)
        {
		case START: 
                {
			if (ch == '\n') 
            {
				linenum++;
			}

			if (isspace(ch)) 
				continue;
			
			lexeme = ch;

			if (isalpha(ch)) 
            {
				lexstate = INID;
			} else if (isdigit(ch)) 
            {
				lexstate = ININT;
			} else if (ch == '.') 
            {
				if (isdigit(in.peek())) 
                {
					lexstate = INREAL;
				}
                else 
                {
					return LexItem(DOT, lexeme, linenum);
				}
			} else if (ch == '\'' || ch == '\"') 
            {
				lexstate = INSTRING;
				stringStartedWith = ch;
			} else if (ch == '!') 
            {
				lexstate = INCOMMENT;
			} else 
            {
				Token t;
				switch (ch)
				{
				case '+':
					t = PLUS;
					break;
				
				case '-':
					t = MINUS;
					break;

				case '*': {
					if (in.peek() == '*') 
                    {
						in.get();
						t = POW;
					} else if (in.peek() == ',') 
                    {
						t = DEF;
					} else 
                    {
						t = MULT;
					}
					break;
				}

				case '/': 
                        {
					if (in.peek() == '/') 
                    {
						in.get();
						t = CAT;
					} else 
                    {
						t = DIV;
					}
					break;
				}

				case '=': 
                        {
					if (in.peek() == '=') 
                    {
						in.get();
						t = EQ;
					} else 
                    {
						t = ASSOP;
					}
					break;
				}

				case '<':
					t = LTHAN;
					break;
				
				case '>':
					t = GTHAN;
					break;

				case ',':
					t = COMMA;
					break;

				case '(':
					t = LPAREN;
					break;
				
				case ')':
					t = RPAREN;
					break;

				case ':':
					if (in.peek() == ':') 
                    {
						in.get();
						t = DCOLON;
					} else 
                    {
						t = ERR;
					}
					break;

				default:
					t = ERR;
					break;
				}
				return LexItem(t, lexeme, linenum);
			}
			break;
		}

        case INID:
            if (std::isalnum(ch) || ch == '_')
            {
                lexeme += ch;
            }
            else
            {
                in.putback(ch);
                return id_or_kw(lexeme, linenum);
            }
            break;

        case ININT:
            if (std::isdigit(ch))
            {
                lexeme += ch;
            }
            else if (ch == '.')
            {
                lexstate = INREAL;
                lexeme += ch;
            }
            else
            {
                in.putback(ch);
                return LexItem(ICONST, lexeme, linenum);
            }
            break;

        case INREAL:
            if (std::isdigit(ch))
            {
                lexeme += ch;
            }
            else if (ch == '.')
            {
                lexeme += ch;
                return LexItem(ERR, lexeme, linenum);
            }
            else
            {
                in.putback(ch);
                return LexItem(RCONST, lexeme, linenum);
            }
            break;

        case INSTRING:
			if (ch == '\n') 
            {
				return LexItem(ERR, lexeme, linenum);
			} 
                else if (ch == '\'' || ch == '\"') 
                {
                    if (ch == stringStartedWith) 
                    {
                        lexeme = lexeme.substr(1);
                        return LexItem(SCONST, lexeme, linenum);
				}
				lexeme += ch;
				return LexItem(ERR, lexeme, linenum);
			} 
                else 
                {
                    lexeme += ch;
                }
			break;
			
		case INCOMMENT:
			if (ch == '\n') 
            {
				lexstate = START;
				linenum++;
			}
			break;

        default:
            break;
        }
    }

    if (in.eof())
        return LexItem(DONE, "", linenum);
    return LexItem(ERR, "", linenum);
}