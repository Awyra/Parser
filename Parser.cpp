/**************************************
*                                     *
*  Parser for simple calculator with  *
*  variables and boolean expressions  *
*                                     *
***************************************/

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <map>
using namespace std;
enum types{DELIMITER = 1, VARIABLE, NUMBER};
const int NUMVARS = 1000;
const int SYNTAX = 0, BRACKETS = 1, EMPTY_EXPRESSION = 2, NOT_FOUND = 3;

class parser {
    char *vr;
    bool f_error;
    char *exp_ptr;
    char token[80];
    char tok_type;
    double num[NUMVARS];
    string var[NUMVARS];
    int index_var = 0;
    map<string, double> vars;

    int new_var(string s) {
        if(index_var >= NUMVARS) {
            serror(NOT_FOUND);
        }
        var[index_var] = s;
        return index_var++;
    }

    void set_var(string s, double a) {
        for(int i = 0; i < NUMVARS; i++)
            if(var[i] == s) {
                num[i] = a;
                return;
            }
        num[new_var(s)] = a;
        return;
    }

    double pop_var(string s) {
        for(int i = 0; i < NUMVARS; i++)
            if(var[i] == s) {
                return num[i];
            }
        return 0;
    }

    void calc_or(double &result) {
        register char op;
        string t;
        double temp;

        calc_and(result);
        while((op=*token) == '|') {
            t = token;
            get_token();
            calc_and(temp);
            if(t == "||") {
                result = result || temp;
            }
            else {
                serror(SYNTAX);
            }
        }
    }

    void calc_and(double &result) {
        register char op;
        string t;
        double temp;

        calc_eq(result);
        while((op=*token) == '&') {
            t = token;
            get_token();
            calc_eq(temp);
            if(t == "&&") {
                result = result && temp;
            }
            else {
                serror(SYNTAX);
            }
        }
    }

    void calc_eq(double &result) {
        register char op;
        string t;
        double temp;

        calc_logic(result);
        while((op=*token) == '=' || op == '!') {
            t = token;
            get_token();
            calc_logic(temp);
            if(t == "==") {
                result = result == temp;
            }else if(t == "!=") {
                result = result != temp;
            }
            else {
                serror(SYNTAX);
            }
        }
    }

    void calc_logic(double &result) {
        register char op;
        string t;
        double temp;

        calc_add(result);
        while((op=*token) == '>' || op == '<') {
            t = token;
            get_token();
            calc_add(temp);
            switch(op) {
                case '>':
                    if(t == ">=") {
                        result = result >= temp;
                    }
                    else {
                        result = result > temp;
                    }
                break;
                case '<':
                    if(t == "<=") {
                        result = result <= temp;
                    }
                    else {
                        result = result < temp;
                    }
                break;
                default:
                    serror(SYNTAX);
            }
        }
    }

    void calc_expr(double &result) {
        string slot;
        char  ttok_type;
        char temp_token[80];

        if(tok_type == VARIABLE) {
            strcpy(temp_token, token);
            ttok_type = tok_type;
            slot = token;
            get_token();
            register char left = *token;
            if(*token != '=') {
                if(*(token+1) == '=' && strchr("+-*/%^", *token)) {
                    get_token();
                    calc_expr(result);
                    //vars.insert(pair<string, double>(slot, result));
                    //cout << "\n\n----\n" << find_var(slot.c_str()) << result << "\n";
                    //cout << "\n-\nleft-" << left << "\n";
                    char* l = (char*)slot.c_str();
                    switch((char)left) {
                        case '+':
                            result = pop_var(slot) + result;
                            set_var(slot, result);
                        break;
                        case '-':
                            result = pop_var(slot) - result;
                            set_var(slot, result);
                        break;
                        case '*':
                            result = pop_var(slot) * result;
                            set_var(slot, result);
                        break;
                        case '/':
                            result = pop_var(slot) / result;
                            set_var(slot, result);
                        break;
                        case '%':
                            result = (double)( (int)pop_var(slot) % (int)result );
                            set_var(slot, result);
                        break;
                        case '^':
                            double ex = pop_var(slot), r = pop_var(slot);
                            for(int t = (int)result - 1 ; t > 0 ; --t){
                                r=r*ex;
                            }
                            result = r;
                            set_var(slot, result);
                        break;
                    }
                    map<string, double>::iterator p;
                    p = vars.find((char*)slot.c_str());
                    if(p != vars.end()) {
                        cout << "\n-\n" << p->second << "\n";
                    }
                    return;
                }
                else {
                    putback();
                    strcpy(token, temp_token);
                    tok_type = ttok_type;
                }
            }
            else {
                cout << "!=\n";
                get_token();
                calc_expr(result);
                set_var(slot, result);
                return;
            }
        }
        calc_or(result);
    }

    void putback() {
        char *t;
        t = token;
        for(; *t ; t++) {
            exp_ptr--;
        }
    }

    void calc_add(double &result) {
        register char op;
        double temp;

        calc_mul(result);
        while((op=*token) == '+' || op == '-') {
            get_token();
            calc_mul(temp);

            switch(op){
                case '-':
                    result -= temp;
                break;
                case '+':
                    result += temp;
                break;
                default:
                    serror(SYNTAX);
            }
        }
    }

    void calc_mul(double &result) {
        register char op;
        double temp;

        calc_phasing(result);
        while((op=*token) == '*' || op == '/' || op == '%') {
            get_token();
            calc_phasing(temp);
            switch(op) {
                case '*':
                    result *= temp;
                break;
                case '/':
                    result /= temp;
                break;
                case '%':
                    result = (int)result % (int)temp;
                break;
            }
        }
    }

    void calc_phasing(double &result) {
        double temp, ex;
        register int t;

        calc_unary(result);
        if(*token == '^') {
            get_token();
            calc_phasing(temp);
            ex = result;
            if(temp == 0.0) {
                result = 1.0;
                return;
            }
            for(t = (int)temp - 1 ; t > 0 ; --t) {
                result = result * (double)ex;
            }
        }
    }

    void calc_unary(double &result) {
        register char op;
        op = 0;
        if(tok_type == DELIMITER && *token == '+' || *token == '-' || *token == '!') {
            op = *token;
            get_token();
        }
        calc_change_priority(result);
        if(op == '-') {
            result = (-result);
        }
        else if(op == '!') {
            result = result == 0;
        }
    }

    void calc_change_priority(double &result) {
        if(*token == '(') {
            get_token();
            calc_add(result);
            if(*token != ')') {
                serror(BRACKETS);
            }
            get_token();
        }
        else{
            calc_atom(result);
        }
    }

    void calc_atom(double &result) {
        switch(tok_type) {
            case NUMBER:
                result = atof(token);
                get_token();
                return;
            case VARIABLE:
                result = pop_var((string)token);
                get_token();
                return;
            default:
                serror(SYNTAX);
        }
    }

    void get_token() {
        register char *temp;
        tok_type = 0;
        temp = token;
        *temp = '\0';

        if(!*exp_ptr) {
            return;
        }
        while(isspace(*exp_ptr)) {
            ++exp_ptr;
        }
        if(strchr("+-*/%^=()><!&|", *exp_ptr)) {
            tok_type = DELIMITER;
            *temp++ = *exp_ptr++;
            if((*exp_ptr == '=' ||
                *exp_ptr == '&' ||
                *exp_ptr == '|') &&
                  (*(exp_ptr-1) != '(' ||
                   *(exp_ptr-1) != ')')) {
                *temp++ = *exp_ptr++;
            }
        }
        else if(isalpha(*exp_ptr) || *exp_ptr == '_') {
            while(isalnum(*exp_ptr) || *exp_ptr == '_') {
                *temp++ = *exp_ptr++;
            }
            tok_type = VARIABLE;
        }
        else if(isdigit(*exp_ptr)) {
            while(isdigit(*exp_ptr)) {
                *temp++ = *exp_ptr++;
            }
            tok_type = NUMBER;
        }
        else {
            serror(SYNTAX);
        }
        *temp = '\0';
    }

    void serror(int error) {
        f_error = true;
        static char *e[] = {
            "Syntax error",
            "Imbalance brackets",
            "Empty expression",
            "Variable not found"
        };
        cout << e[error] << endl;
    }

    int isdelim(char c) {
        return (strchr(" +-/*%^=()", c) || c==9 || c=='\r' || c==0 );
    }


public:
    parser() {
        exp_ptr = NULL;
    }

    double init(char *exp) {
        f_error = false;

        double result;
        exp_ptr = exp;
        get_token();
        if(!*token) {
            serror(EMPTY_EXPRESSION);
            return 0.0;
        }
        calc_expr(result);
        if(*token) {
            serror(SYNTAX);
        }
        return result;
    }

    bool wat_error() {
        return f_error;
    }
};

int main (int argc, char** argv) {
    double res;
    char expstr[80];
    cout << "If u need to exit press '.'\n";
    parser ob;
    for(;;) {
        cin.getline(expstr, 79);
        if(*expstr == '.') {
            break;
        }
        res = ob.init(expstr);
        if(!ob.wat_error()) {
            cout << "#=>" << res << endl;
        }
    }
    return 0;
}
