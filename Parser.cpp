/**********************************************
* Парсер-калькулятор, оперирует переменными,  *
* булевыми выражениями и прочей канителью     *
*          ( Устарело. Soon... )              *
***********************************************/

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <map>
using namespace std;
enum types{DELIMITER=1, VARIABLE, NUMBER};
const int NUMVARS=1000;
const int SYNTAX = 0, BRACKETS = 1, EMPTY_EXPRESSION = 2, NOT_FOUND = 3;

class parser {
    char *vr;
    bool f_error;
    char *exp_ptr;
    char token[80];
    char tok_type;
    double num[NUMVARS];
    string var[NUMVARS];
    int index_var=0;
    map<string, double> vars;
    
    int new_var( string s ){                                   
        if( index_var >= NUMVARS ){
            serror(NOT_FOUND);
        }
        var[index_var] = s;
        return index_var++;
    }
    
    void set_var( string s, double a ){                            
        for( int i=0; i<NUMVARS; i++ )
            if( var[i] == s ){
                num[i] = a;
                return;
            }
        num[new_var(s)] = a;
        return;
    }
    
    double pop_var( string s ){                                   
        for( int i=0; i<NUMVARS; i++ )
            if( var[i] == s ){
                return num[i];
            }
        return 0;
    }
    
    void eval_exp_4(double &result){
        register char op;
        string t;
        double temp;
        
        eval_exp_3(result);
        while((op=*token) == '|'){
            t = token;
            get_token();
            eval_exp_3(temp);
            if(t == "||"){
                result = (result || temp);
            }else{
                serror(SYNTAX);
            }
        }
    }

    void eval_exp_3(double &result){
        register char op;
        string t;
        double temp;
        
        eval_exp_2(result);
        while((op=*token) == '&'){
            t = token;
            get_token();
            eval_exp_2(temp);
            if(t == "&&"){
                result = (result && temp);
            }else{
                serror(SYNTAX);
            }
        }
    }

    void eval_exp_2(double &result){
        register char op;
        string t;
        double temp;
        
        eval_exp_1(result);
        while((op=*token) == '=' || op == '!'){
            t = token;
            get_token();
            eval_exp_1(temp);
            if(t == "=="){
                result = (result == temp?1:0);
            }else if(t == "!="){
                result = (result != temp?1:0);
            }else{
                serror(SYNTAX);
            }
        }
    }

    void eval_exp_1(double &result){
        register char op;
        string t;
        double temp;
        
        eval_exp2(result);
        while((op=*token) == '>' || op == '<'){
            t = token;
            get_token();
            eval_exp2(temp);
            switch(op){
                case '>': 
                    if(t == ">="){
                        result = (result>=temp?1:0);
                    }else{ 
                        result = (result>temp?1:0);
                    }
                break;
                case '<': 
                    if(t == "<="){
                        result = (result<=temp?1:0);
                    }else{
                        result = (result<temp?1:0);
                    }
                break;
                default: 
                    serror(SYNTAX);
            }
        }
    }

    void eval_exp1(double &result){                           
        string slot;
        char  ttok_type;
        char temp_token[80];
        
        if(tok_type == VARIABLE){                                       
            strcpy(temp_token, token);                               
            ttok_type = tok_type;                                         
            slot = token;                                              
            get_token();                                               
            register char left = *token;
            if(*token != '='){                                       
                if((*(token+1) == '=') && (strchr("+-*/%^", *token))){
                    get_token();                                                   
                    eval_exp1(result);                                              
                    //vars.insert(pair<string, double>(slot, result));             
                    //cout<<"\n\n----\n"<<find_var(slot.c_str())<<result<<"\n";
                    //cout<<"\n-\nleft-"<< left<<"\n";                             
                    char* l = (char*)slot.c_str();//---------------------
                    switch((char)left){
                        case '+':
                            result = pop_var(slot) + result;
                            set_var( slot, result );
                        break;
                        case '-':
                            result = pop_var(slot) - result;
                            set_var( slot, result );
                        break;
                        case '*':
                            result = pop_var(slot) * result;
                            set_var( slot, result );
                        break;
                        case '/':
                            result = pop_var(slot) / result;
                            set_var( slot, result ); 
                        break;
                        case '%':
                            result = (double)( (int)pop_var(slot) % (int)result );
                            set_var( slot, result );
                        break;
                        case '^':
                            double ex = pop_var(slot), r = pop_var(slot);
                            for(int t=(int)result-1 ; t>0 ; --t){
                                r=r*ex;
                            }
                            result = r;
                            set_var( slot, result );
                        break;
                    }
                    map<string, double>::iterator p;                  
                    p = vars.find((char*)slot.c_str());               
                    if(p != vars.end()){
                        cout<<"\n-\n"<< p->second<<"\n"; 
                    }
                    return;                              
                }else{
                    putback();                           
                    strcpy(token, temp_token);           
                    tok_type = ttok_type;                
                }
            }else{                                     
                cout<<"!=\n";
                get_token();                           
                eval_exp1(result);                     
                set_var( slot, result );               
                return;                                
            }
        }
        eval_exp_4(result);                      
    }
    
    void putback(){                            
        char *t;                               
        t = token;                             
        for(;*t;t++){
            exp_ptr--;                         
        }
    }

    void eval_exp2(double &result){
        register char op;
        double temp;
        
        eval_exp3(result);
        while((op=*token) == '+' || op == '-'){
            get_token();
            eval_exp3(temp);
            
            switch(op){
                case '-':
                    result = result-temp;
                break;
                case '+':
                    result += temp;
                break;
                default:
                    serror(SYNTAX);
            }
        }
    }

    void eval_exp3(double &result){
        register char op;
        double temp;
        
        eval_exp4(result);
        while((op=*token) == '*' || op == '/' || op == '%'){
            get_token();
            eval_exp4(temp);
            switch(op){
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

    void eval_exp4(double &result){
        double temp, ex;
        register int t;
        
        eval_exp5(result);
        if(*token == '^'){
            get_token();
            eval_exp4(temp);
            ex = result;
            if(temp == 0.0){
                result = 1.0;
                return;
            }
            for( t=(int)temp-1 ; t>0 ; --t ){
                result = result * (double)ex;
            }
        }
    }

    void eval_exp5(double &result){
        register char op;
        op = 0;
        if((tok_type == DELIMITER)&& *token == '+' || *token == '-' || *token == '!'){
            op=*token;
            get_token();
        }
        eval_exp6(result);
        if(op == '-'){
            result = (-result);
        }else if(op == '!'){
            result = (result==0?1:0);
        }
    }

    void eval_exp6(double &result){
        if((*token=='(')){
            get_token();
            eval_exp2(result);
            if(*token != ')'){
                serror(BRACKETS);
            }
            get_token();
        }else{
            atom(result);
        }
    }

    void atom(double &result){                 
        switch(tok_type){                      
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

    void get_token(){
        register char *temp;
        tok_type = 0;
        temp=token;
        *temp = '\0';
        
        if(!*exp_ptr){
            return;
        }
        while(isspace(*exp_ptr)){
            ++exp_ptr;
        }
        if(strchr("+-*/%^=()><!&|", *exp_ptr)){
            tok_type = DELIMITER;
            *temp++ = *exp_ptr++;
            if(((*exp_ptr == '=')||
                (*exp_ptr == '&')||
                (*exp_ptr == '|'))&&
                ((*(exp_ptr-1)    != '(')||
                    (*(exp_ptr-1) != ')'))){    
                *temp++ = *exp_ptr++;
            }
        }else if((isalpha(*exp_ptr))||(*exp_ptr == '_')){
            while((isalnum(*exp_ptr))||(*exp_ptr == '_')){
                *temp++ = *exp_ptr++;
            }
            tok_type = VARIABLE;
        }
        else if(isdigit(*exp_ptr)){
            while(isdigit(*exp_ptr)){
                *temp++ = *exp_ptr++;
            }
            tok_type = NUMBER;
        }else{
            serror(SYNTAX);
        }
        *temp = '\0';
    }

    void serror(int error){         
        f_error = true;             
        static char *e[]={          
            "Syntax error",
            "Imbalance brackets",
            "Empty expression",
            "Variable not found"
        };
        cout << e[error] << endl;   
    }

    int isdelim(char c){            
        return (strchr(" +-/*%^=()", c) || c==9 || c=='\r' || c==0 ? 1 : 0 );
    }

    
public:
    parser(){                       
        exp_ptr=NULL;               
    }

    double eval_exp(char *exp){     
        f_error = false;            
    
        double result;              
        exp_ptr = exp;              
        get_token();                
        if(!*token){                
            serror(EMPTY_EXPRESSION);
            return 0.0;              
        }
        eval_exp1(result);           
        if(*token){
            serror(SYNTAX);          
        }
        return result;               
    }

    bool wat_error(){                
        return f_error;
    }
};

int main(int argc, char** argv) {
    double otvet;
    char expstr[80];
    cout<<"If u need to exit press '.'\n";
    parser ob;
    for(;;){
        cin.getline(expstr,79);
        if((*expstr == '.')){
            break;
        }
        res=ob.eval_exp(expstr);
        if(!ob.wat_error()){
            cout << "#=>" << res << endl;
        }
    }
    return 0;
}
