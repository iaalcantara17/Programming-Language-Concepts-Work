//Israel Alcantara

#include <stack>
#include <string>

using namespace std;

int precedence(char c)
    {
    if (c == '^')
        return 3;
    
    else if (c == '*' || c == '/')
        return 2;
    
    else if (c == '+' || c == '-')
        return 1;
    
    else
        return -1;
    }

string InfixToPostfix(string infixstr) 
    {
    stack<char> st;
    string postfixstr;

    for (int i = 0; i < infixstr.length(); i++) 
    {
        if (infixstr[i] == ' ') 
        {
            continue;
        }
                                          
        else if (isalpha(infixstr[i])) 
        {
            postfixstr += infixstr[i];
            postfixstr += ' ';
        }
                                          
        else if (infixstr[i] == '(')
            st.push('(');
                                          
        else if (infixstr[i] == ')') 
        {
            while (st.top() != '(') 
            {
                postfixstr += st.top();
                postfixstr += ' ';
                st.pop();
            }
            st.pop();
        }

        else 
        {
            while (!st.empty() && precedence(infixstr[i]) <= precedence(st.top()))
            {
                postfixstr += st.top();
                postfixstr += ' ';
                st.pop();
            }
            st.push(infixstr[i]);
        }
    }

    while (!st.empty()) 
    {
        postfixstr += st.top();
        postfixstr += ' ';
        st.pop();
    }
    
    return postfixstr;
}

string PostfixToInfix(string postfixstr) 
{
    stack<string> st;

    for (int i = 0; i < postfixstr.length(); i++) 
    {
        if (postfixstr[i] == ' ') 
        {
            continue;
        }
                                            
        else if (isalpha(postfixstr[i])) 
        {
            string op(1, postfixstr[i]);
            st.push(op);
        }
        
        else 
        {
            string op1 = st.top(); st.pop();
            string op2 = st.top(); st.pop();
            st.push("(" + op2 + " " + postfixstr[i] + " " + op1 + ")");
        }
    }

    return st.empty() ? "" : st.top();
}