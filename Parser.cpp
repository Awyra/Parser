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
    
    int new_var( string s ){                                            //инит переменных
        if( index_var >= NUMVARS ){
            serror(NOT_FOUND);
        }
        var[index_var] = s;
        return index_var++;
    }
    
    void set_var( string s, double a ){                                 //переинициализация переменных
        for( int i=0; i<NUMVARS; i++ )
            if( var[i] == s ){
                num[i] = a;
                return;
            }
        num[new_var(s)] = a;
        return;
    }
    
    double pop_var( string s ){                                         //достать значение из переменной
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

    void eval_exp1(double &result){                             //принимает ссылку на переменную и инитит её ответом выражения
        string slot;
        char  ttok_type;
        char temp_token[80];
        
        if(tok_type == VARIABLE){                                       //если данный токен - переменная
            strcpy(temp_token, token);                                  //сохраняем в левой переменной значение переменной token
            ttok_type = tok_type;                                           //инитим тип токена
            slot = token;                                                   //инитм строку slot адрессом токена
            get_token();                                                //считываем следующий токен
            register char left = *token;
            if(*token != '='){                                              //если текущий токен не равно
                if((*(token+1) == '=') && (strchr("+-*/%^", *token))){
                    get_token();                                                    //считываем следующий токен
                    eval_exp1(result);                                              //сохраняем в переменную result значение текущего выражения(рекурсия)
                    //vars.insert(pair<string, double>(slot, result));              //приравниваем значение считанному имени переменной результат текущего выражения
                    //cout<<"\n\n----\n"<<find_var(slot.c_str())<<result<<"\n";
                    //cout<<"\n-\nleft-"<< left<<"\n";                              //если была найдена переменная то возвращаем её значение
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
                    map<string, double>::iterator p;                    //инитим итератор
                    p = vars.find((char*)slot.c_str());                 //находим имя переменной в подобии БД и сохраняем адрес в итераторе p//---------------------
                    if(p != vars.end()){
                        cout<<"\n-\n"<< p->second<<"\n";    //если была найдена переменная то возвращаем её значение
                    }
                    return;                                             //возврат
                }else{
                    putback();                                  //возврат к предвидущему токену
                    strcpy(token, temp_token);                  //востанавливаем значение переменной token 
                    tok_type = ttok_type;                           //востанавливаем значение типа токена
                }
            }else{                                      //в противном случае
                cout<<"!=\n";
                get_token();                                //считываем следующий токен
                eval_exp1(result);                          //сохраняем в переменную result значение текущего выражения(рекурсия)
                set_var( slot, result );                    //приравниваем значение считанному имени переменной результат текущего выражения
                return;                                     //возврат
            }
        }
        eval_exp_4(result);                         //записываем в переменную result значение выражения
    }
    
    void putback(){                             //возвращает ссылку на предвидущий токен
        char *t;                                    //инитим ссылку t
        t = token;                                  //инитим ссылку t адресом текущего токена
        for(;*t;t++){
            exp_ptr--;                                  //проходим по всем символам текущего токена паралельно возврат назад по символам лексемы
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

    void atom(double &result){                  //принимает ссылку на переменную хранящую ответ выражения и сохраняет значение токена в эту переменную
        switch(tok_type){                           //проверяем тип считанного токена
            case NUMBER:                                //если число
                result = atof(token);                           //инициализирует переменную result значением token перведённое в числовой аналог
                get_token();                                //считываем следующий токен
                return;                                     //возврат
            case VARIABLE:                              //если переменная
                result = pop_var((string)token);                //инициализирует переменную result значением переменной с именем token
                get_token();                                //считываем следующий токен
                return;                                     //возврат
            default:                                    //в противном случае
                serror(SYNTAX);                                 //вывод ошибки
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
                    (*(exp_ptr-1) != ')'))){                 // аля-lisp
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

    void serror(int error){             //вызывается при ошбке
        f_error = true;                     //присваиваем флагу ошибки true
        static char *e[]={                  //инитим массив ошибок
            "Syntax error",
            "Imbalance brackets",
            "Empty expression",
            "Variable not found"
        };
        cout << e[error] << endl;               //вывод ошибки
    }

    int isdelim(char c){                //если есть знаки пунктуации то возвращает true
        return (strchr(" +-/*%^=()", c) || c==9 || c=='\r' || c==0 ? 1 : 0 );
    }

    
public:
    parser(){                           //конструктор инитит класс
        exp_ptr=NULL;                       //приравниваниваем нулю переменную в которую класс сохраняет выражение 
    }

    double eval_exp(char *exp){         //начало синтаксичекого анализа получает выражение и возвращает его значение, инитится флаг ошибок false
        f_error = false;                        //при каждом использовании синтаксического анализа флаг ошибок инитится false
    
        double result;                      //инитим переменную result в которй сохраним ответ выражения
        exp_ptr = exp;                      //сохраняем переданное выражение в переменную каласса
        get_token();                        //считываем токен
        if(!*token){                        //если введено пустое выражение
            serror(EMPTY_EXPRESSION);                           //вывод выражение пусто
            return 0.0;                         //как ответ выводим 0
        }
        eval_exp1(result);                  //сохраняем ответ выражения в переменной result
        if(*token){
            serror(SYNTAX);             //если что-то осталось после анализа
        }
        return result;                      //возвращаем переменную result в которой содержится ответ выражения
    }

    bool wat_error(){                       //возвращает флаг ошибки
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
