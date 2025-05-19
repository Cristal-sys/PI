#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<cmath>
#include<vector>

struct Variable {

std::string name="0";//name

long double value=0;

long double sigma=0;

long double standard_deviation=0;

long double standard_deviation_mean=0;

};

/* @statement
 * decl when we're atributing
 * e.g. x+y is func but f=x+y is a func_assign
 *
 * service is for other system services like fit(x, y)
 */
enum class statement{var, var_assign, incompl_var_decl, func, func_assign, service}; 

std::string adition_chomper(std::stringstream&);
std::string multiplication_chomper(std::stringstream&);
bool is_there_free_plus_or_minus(std::string&);
bool is_there_free_multiply(std::string&);
bool is_nested(std::string&);
bool are_brackets_consistent(std::string&);
std::string unnest_expression(std::string&);
int how_many_free_operators(std::string&);
int how_many_bracket_chunks(std::string&);
bool is_it_math_function_format(std::string&);
std::string chomp_until_next_free_operator(std::stringstream&);
long double expression_evaluator(std::string);
long double operacao(long double, char, long double);
long double math_function(std::string&, long double);
long double function_to_double(std::string& f, std::vector<Variable>& parameters);
long double partial(std::string& f, std::string variable, std::vector<Variable>& parameters);
long double uncertainty(std::string& f, std::vector<Variable>& parameters);
void read_variable(std::fstream& file, std::vector<Variable>& parameters);
void read_txt_table(std::fstream& file, std::vector<Variable>& parameters);
void read_simple_txt_table(std::fstream& file, std::vector<Variable>& parameters);
void read_function(std::fstream& file, std::vector<std::string>& functions);
bool receptionist(std::string& filename, std::string& command);
std::string get_edited_table_name(std::string& filename);
void write(std::fstream& output, std::string command, std::vector<Variable>& parameters, std::vector<std::string>& functions);
long double angular_right(std::vector<Variable>& parameters, long double n);
long double linear_right(std::vector<Variable>& parameters, long double n);
long double angular_sigma_right(std::vector<Variable>& parameters, long double n);
long double linear_sigma_right(std::vector<Variable>& parameters, long double n);
long double error(std::vector<Variable>& parameters);
long double a_2(std::vector<Variable>& parameters, long double n);
long double a_1(std::vector<Variable>& parameters, long double n);
long double a_0(std::vector<Variable>& parameters, long double n);
long double a_2_sigma(std::vector<Variable>& parameters, long double n);
long double a_1_sigma(std::vector<Variable>& parameters, long double n);
long double a_0_sigma(std::vector<Variable>& parameters, long double n);
long double error_parabola(std::vector<Variable>& parameters);
bool is_operator(char);
bool is_operator_or_parenthesis(char);
void read_operand(std::string& function, std::string& c, int& i);
bool is_line_variable(std::fstream&);
void print_remaining_file(std::fstream&);
bool is_bracket_next(std::fstream&);
bool is_parameter_in_function(std::string&, std::string&);

const std::string euler="2.718281828459045235";
const std::string pi="3.141592653589793238";

int main(){
std::string filename,final_file_name, command;
std::vector<Variable> parameters;//low-ky I think it would make a huge diff in speed for large data sets to use a tree data structure 
std::vector<std::string> functions;
bool should_we_quit=false;

while(true){

should_we_quit=receptionist(filename, command);

if(should_we_quit){
	return 0;
}

std::fstream file(filename, std::ios::in);
while(!file.is_open()){
    std::cout<<"\033[31m"<<"\nfile does not exist >~<\n"<<"\033[0m";
    file.close();
    should_we_quit=receptionist(filename, command);
    if(should_we_quit){
	    return 0;
    }
    file.open(filename, std::ios::in);
}

final_file_name=get_edited_table_name(filename);
std::fstream output(final_file_name, std::ios::out);


if(command.find("simple") != std::string::npos){
	read_simple_txt_table(file, parameters);
}else{read_txt_table(file, parameters);}

read_function(file, functions);

write(output, command, parameters, functions);

file.close();
output.close();
filename="";
final_file_name="";
command="";
parameters.clear();
functions.clear();

std::cout<<"\033[32m"<<"file analysed successfully! :3 \n\n"<<"\033[0m";

}
}
std::string adition_chomper(std::stringstream& expression){//takes a non operator-initialized expression and reads it until the next non bracket-nested + or - operator

char c;
std::string little_expression="";
int b_count=0;
while(expression>>c){

	if(c=='('){
		b_count++;
	}else if(c==')'){
		b_count--;
	}

	little_expression=little_expression+c;
	if(b_count==0 && (expression.peek()=='+' || expression.peek()=='-')){
		break;
	}

}

return little_expression;

}

std::string multiplication_chomper(std::stringstream& expression){//takes a non operator-initialized expression and reads it until the next non bracket-nested * operator

char c;
std::string little_expression="";
int b_count=0;
while(expression>>c){

	if(c=='('){
		b_count++;
	}else if(c==')'){
		b_count--;
	}

	little_expression=little_expression+c;
	if(b_count==0 && expression.peek()=='*'){
		break;
	}

}

return little_expression;

}

bool is_there_free_plus_or_minus(std::string& expression){

int i=0, len=expression.length();
bool output=false;
for(int j=0; j<len; j++){
	if(expression[j]=='('){
		i++;
	}
	if(expression[j]==')'){
		i--;
	}
	if(i==0 && (expression[j]=='+'|| expression[j]=='-')){
		output=true;
	}
}

return output;

}

bool is_there_free_multiply(std::string& expression){

int i=0, len=expression.length();
bool output=false;
for(int j=0; j<len; j++){
	if(expression[j]=='('){
		i++;
	}
	if(expression[j]==')'){
		i--;
	}
	if(i==0 && expression[j]=='*'){
		output=true;
	}
}

return output;

}

bool is_nested(std::string& expression){

	int i=1, b_count=0, len=expression.length();

	if(len<2){
		return false;
	}
	if(expression[0]=='(' && expression[len-1]==')'){
	while(i<len-1){
		if(expression[i]=='('){
			b_count++;
		}
		if(expression[i]==')'){
			b_count--;
		}
		if(b_count<0){
			return false;
		}
		i++;
	}

	if(b_count==0){
	return true;
	}
}

return false;

}

bool are_brackets_consistent(std::string& expression){//assumes we're receiving a non empty string

	int b_count=0, len=expression.length();
	for(int j=0; j<len; j++){
		if(expression[j]=='('){
			b_count++;
		}
		if(expression[j]==')'){
			b_count--;
		}
		if(b_count<0){
			return false;
		}
	}
	return b_count==0 ? true : false;

}

std::string unnest_expression(std::string& expression){

	std::string output="";
	int len=expression.length();

	if(expression=="()"){
		return "";
	}
	if(is_nested(expression)){

		for(int i=1; i<len-1; i++){
			output=output+expression[i];
		}
		return unnest_expression(output);
	}
	return expression;
	

}


int how_many_free_operators(std::string& expression){

	int b_count=0, operators=0, len=expression.length();
	for(int j=0; j<len; j++){

		if(expression[j]=='('){
			b_count++;
		}
		if(expression[j]==')'){
			b_count--;
		}
		if(is_operator(expression[j]) && b_count==0){
			operators++;
		}
	}
return operators;
}

int how_many_bracket_chunks(std::string& expression){

	int chunks=0, b_count=0, len=expression.length();
	
	for(int j=0; j<len; j++){

		if(expression[j]=='('){
			b_count++;
		}
		if(expression[j]==')'){
			b_count--;
		}
		if(b_count==0 && expression[j]==')'){
			chunks++;
		}
	}
	return chunks;
}

bool is_it_math_function_format(std::string& expression){//assumes to be bracket-consisted, eg, sin)(2) considers sin) to be a function name

	std::string function_name;
	int len=expression.length(), b_count=0, j=0;
	for(; j<len && expression[j]!='('; j++){
		function_name=function_name+expression[j];
	}
	if(j==len || j==len-1){
		return false;
	}
	for(; j<len; j++){
		if(expression[j]=='('){
			b_count++;
		}
		if(expression[j]==')'){
			b_count--;
		}
		if(b_count==0){
			return j==len-1 ? true : false;
		}
	}
	return false;

}

std::string chomp_until_next_free_operator(std::stringstream& expression_stream){ //reads the expression until the next free operator, leaving it intact and returning 

	std::string output="";						        	//the the string read until the operator. returns empty string when started by operator.
	char c;									//if expression_stream is initially empty, returns empy string.
	int b_count=0;
	while(expression_stream>>c){

		output=output+c;

		if(c=='('){
			b_count++;
		}
		if(c==')'){
			b_count--;
		}
		if(b_count==0 && is_operator(expression_stream.peek())){
			return output;
		}
	}
return output;

}

long double expression_evaluator(std::string expression){

    std::stringstream expression_stream{expression};
    long double operand1, operand2, total=0;
    char operador, c;
    if(expression==""){
	    return nan("");
    }

    if(!are_brackets_consistent(expression)){return nan("");}
    
    if(is_there_free_plus_or_minus(expression)){
	    if (!(expression_stream.peek()=='+' || expression_stream.peek()=='-')){
		    total=total+expression_evaluator(adition_chomper(expression_stream));
	    }
    while(expression_stream.peek()=='+' || expression_stream.peek()=='-'){
	    expression_stream>>c;
		    if(c=='+'){
		    total=total+expression_evaluator(adition_chomper(expression_stream));
		}
	    if(c=='-'){
	    	total=total-expression_evaluator(adition_chomper(expression_stream));
		
		}
		}

		return total;
    }

    if(is_there_free_multiply(expression)){
    total=1;
    total=total*expression_evaluator(multiplication_chomper(expression_stream));
    while(expression_stream>>c){
    total=total*expression_evaluator(multiplication_chomper(expression_stream));

    }

return total;
}

   if(is_nested(expression)){

	   return expression_evaluator(unnest_expression(expression));

   }
   int free_operators_number=how_many_free_operators(expression);
   int bracket_chunks_number=how_many_bracket_chunks(expression);
   if(free_operators_number>1){
	   return nan("");
   }
   if(free_operators_number==1){
	   std::string expression1, expression2;
	   operand1=expression_evaluator(chomp_until_next_free_operator(expression_stream));
	   expression_stream>>operador;
	   operand2=expression_evaluator(chomp_until_next_free_operator(expression_stream));
	   return operacao(operand1, operador, operand2);
}
   if(free_operators_number==0){

	   if(bracket_chunks_number>1){
		   return nan("");
	   }
	   if(bracket_chunks_number==1 && !is_it_math_function_format(expression)){
		   return nan("");
	   }
	   if(bracket_chunks_number==1 && is_it_math_function_format(expression)){
	   
		   std::string function_name, argument;
		   int len=expression.length(), j=0, b_count=0;
		   for(; j<len && expression[j]!='('; j++){
			   function_name=function_name+expression[j];
		   }
		   for(; j<len; j++){
  			   argument=argument+expression[j];
		   }
		   return math_function(function_name, expression_evaluator(argument));

	   }
	   if(bracket_chunks_number==0){
		   return expression_stream>>total ? total : nan("");
   }

}

return nan("");
}

long double operacao(long double operand1, char operador, long double operand2)
{
    if(operador=='+')
    {
        return operand1+operand2;
    }

    if(operador=='-')
    {
        return operand1-operand2;
    }

    if(operador=='*')
    {
        return operand1*operand2;
    }

    if(operador=='/')
    {
        return operand1/operand2;
    }

    if(operador=='^')
    {
        return powl(operand1, operand2);
    }

    return nan("");
}

long double math_function(std::string& function_name, long double argument){

	if(function_name=="sen"||function_name=="Sen"||function_name=="sin"||function_name=="Sin"){
		return sinl(argument);
	}

	if(function_name=="asen"||function_name=="aSen"||function_name=="asin"||function_name=="aSin"){
		return asinl(argument);
	}

	if(function_name=="cos"||function_name=="Cos"){
		return cosl(argument);
	}

	if(function_name=="acos"||function_name=="aCos"){
		return acosl(argument);
	}

	if(function_name=="tan"||function_name=="Tan"||function_name=="tg"||function_name=="Tg"){
		return tanl(argument);
	}

	if(function_name=="atan"||function_name=="aTan"||function_name=="atg"||function_name=="aTg"){
		return atanl(argument);
	}

	if(function_name=="exp"){
		return expl(argument);
	}

	if(function_name=="ln"){
		return logl(argument);
	}

return nan("");

}


long double function_to_double(std::string& f, std::vector<Variable>& parameters)
{
    std::string expression, c;
    int i=0, limit=f.length();
    while(i<limit){//name
        if(is_operator_or_parenthesis(f[i]))
        {
            expression=expression+f[i];
            i++;
            continue;
        }
        read_operand(f, c, i);
        bool encontrou=false;

        for(Variable variable:parameters){//in simple fit, varible is usually linear and angular,
					  //and are in the end of the vector. Starting search at the end 
					  //is a good optimization
            if(variable.name==c)
            {
                std::stringstream ss;
                std::string aux;
                ss<<std::setprecision(std::numeric_limits<long double>::digits10 + 1)<<variable.value;
                ss>>std::setprecision(std::numeric_limits<long double>::digits10 + 1)>>aux;
                expression=expression+aux;
                encontrou=true;
                break;
            }
        }
        if(encontrou==false && c=="e")
        {
            expression=expression+euler;
        }
        else if(encontrou==false && c=="pi")
        {
            expression=expression+pi;
        }
        else if(encontrou==false)
        {
            expression=expression+c;
        }

    }
return expression_evaluator(expression);
}


long double partial(std::string& f, std::string variable, std::vector<Variable>& parameters)//name
{
    const static long double h=0.00012207031L;

    const int n=parameters.size();
    int i=n-1;
    for(; i>=0; --i){//starting from end, finds more rapidly in simple fits
        if(parameters[i].name==variable)
        {
            break;
        }

    }
    long double auxiliar1, auxiliar2, old_value;
    auxiliar1=function_to_double(f, parameters);
    old_value=parameters[i].value;
    parameters[i].value+=h;
    auxiliar2=function_to_double(f, parameters);
    parameters[i].value=old_value;
    return (auxiliar2-auxiliar1)/h;

}


long double uncertainty(std::string& f, std::vector<Variable>& parameters){

long double var=0;
long double temp;
for(Variable& parameter : parameters){
    if(is_parameter_in_function(parameter.name, f)){
    temp=partial(f, parameter.name, parameters);
    var+=parameter.sigma*parameter.sigma*temp*temp;
    }//avoids calculatin partial ( O(n) best case, O(n²) worst) 
       //for variables that don't appear in the function
}
return sqrtl(var);
}

void read_variable(std::fstream& file, std::vector<Variable>& parameters){

std::string name;
char bracket;
long double n=0;
long double equip_sigma, measure, sum=0, sum_squares=0, mean, sigma, standard_deviation=0, standard_deviation_mean=0;
file>>name>>equip_sigma>>bracket;

while(!is_bracket_next(file))
{
    file>>measure;
    sum+=measure;
    sum_squares+=measure*measure;
    n+=1;
}

file>>bracket;

if(n==1)
{
    mean=sum;
    parameters.push_back({name, mean, equip_sigma});
    return;
}

mean=sum/n;
standard_deviation=sqrtl((sum_squares-mean*mean*n)/(n-1));
standard_deviation_mean=sqrtl((sum_squares-mean*mean*n)/(n*(n-1)));//already takes sigma of the mean
sigma=sqrtl(standard_deviation_mean*standard_deviation_mean+equip_sigma*equip_sigma);//accounts for equip uncertainty
parameters.push_back({name, mean, sigma, standard_deviation, standard_deviation_mean});
return;
}

bool is_line_variable(std::fstream& file){

	std::string buffer;
	char ch;

	if(file>>ch){file.putback(ch);} //ignore leading white spaces

	while(file.peek()!=EOF){
		file.get(ch);
		
		buffer=buffer+ch;
		if(ch=='['){
			
	for(int t=buffer.length()-1; t>=0; t--){
	
		file.putback(buffer[t]);
	}
	return true;
	
		}


		if(ch=='\n'){

	for(int t=buffer.length()-1; t>=0; t--){
	
		file.putback(buffer[t]);
	}
	return false;
		}
	}


	for(int t=buffer.length()-1; t>=0; t--){
	
		file.putback(buffer[t]);
	}
	return false;

}

void read_txt_table(std::fstream& file, std::vector<Variable>& parameters){

while(is_line_variable(file)){
    read_variable(file, parameters);

}

return;
}

void read_simple_txt_table(std::fstream& file, std::vector<Variable>& parameters){

std::string base_name, name_buffer;
long double sigma=0, value=0;
long int i=0;
char bracket_buffer;

file>>base_name>>sigma>>bracket_buffer;

while(!is_bracket_next(file)){

	i++;
	file>>value;
	name_buffer=base_name+std::to_string(i);
	parameters.push_back({name_buffer, value, sigma});
}

file>>bracket_buffer;

i=0;
sigma=0;
value=0;

file>>base_name>>sigma>>bracket_buffer;

while(!is_bracket_next(file)){

	i++;
	file>>value;
	name_buffer=base_name+std::to_string(i);
	parameters.push_back({name_buffer, value, sigma});
}

file>>bracket_buffer;

}

void read_function(std::fstream& file, std::vector<std::string>& functions){

std::string f, buffer, final_f;

while(std::getline(file,f)){

	if(f==""){
		continue;
	}

	std::stringstream f_stream{f};
	
	while(f_stream>>buffer){
		final_f=final_f+buffer;
	}


if(final_f!=""){
	functions.push_back(final_f);
}
final_f="";
}

return;
}

bool receptionist(std::string& filename, std::string& command){

std::cout<<"type exit to leave\nenter file name with table:\n\n";
std::cin>>filename;
if(std::getline(std::cin, command))
{

}

if(filename=="exit"){
	return true;
}

return false;
}

std::string get_edited_table_name(std::string& filename){

std::string new_name;
int len=filename.length();
for(int i=0; i<len; ++i)
{
    if(filename[i]=='.' &&filename[i+1]=='t' &&filename[i+2]=='x' &&filename[i+3]=='t')
    {
        break;
    }
    new_name=new_name+filename[i];
}
return new_name+"_final.txt";
}

void write(std::fstream& output, std::string command, std::vector<Variable>& parameters, std::vector<std::string>& functions){
//first let's process the commands
//mostly those are fits as of now
output<<std::setprecision(std::numeric_limits<long double>::digits10 + 1);

if(command.find("logarithmic") != std::string::npos && command.find("fit") != std::string::npos)
{

	const int n=parameters.size()/2;
	for(int i=0; i<n; i++){
		parameters[i].sigma=parameters[i].sigma/parameters[i].value;
		parameters[i].value=logl(parameters[i].value);
	}

}

if(command.find("linear_fit") != std::string::npos)
{
    long double angular_value, angular_sigma, linear_value, linear_sigma, error_squared;
    const int n=parameters.size()/2;
    angular_value=angular_right(parameters, n);
    angular_sigma=angular_sigma_right(parameters, n);
    linear_value=linear_right(parameters, n);
    linear_sigma=linear_sigma_right(parameters, n);
    error_squared=error(parameters);

    parameters.push_back({"angular", angular_value, angular_sigma});
    parameters.push_back({"linear", linear_value, linear_sigma});
    parameters.push_back({"error", error_squared});
}

if(command.find("quadratic_fit") != std::string::npos)
{
    long double a2, a2_sigma, a1, a1_sigma, a0, a0_sigma, quadratic_error_squared;
    const int n=parameters.size()/2;
    a2=a_2(parameters, n);
    a2_sigma=a_2_sigma(parameters, n);
    a1=a_1(parameters, n);
    a1_sigma=a_1_sigma(parameters, n);
    a0=a_0(parameters, n);
    a0_sigma=a_0_sigma(parameters, n);
    quadratic_error_squared=error_parabola(parameters);

    parameters.push_back({"a_2", a2, a2_sigma});
    parameters.push_back({"a_1", a1, a1_sigma});
    parameters.push_back({"a_0", a0, a0_sigma});
    parameters.push_back({"error", quadratic_error_squared});
}
//now let's write the variables
if(command.find("simple")==std::string::npos){
for(Variable variavel:parameters){
    if(variavel.name=="angular" || variavel.name=="a_2")
    {
        output<<'\n';
    }

    output<<variavel.name<<' '<<variavel.value<<" ± "<<variavel.sigma<<'\n';

}
}else{ 
	long int i=0;
       	const int n=parameters.size();
	while(parameters[i].name!="angular" && parameters[i].name!="a_2"){

		i=i+1;
	}

	for(long int j=i; j<n; j=j+1){

    output<<parameters[j].name<<' '<<parameters[j].value<<" ± "<<parameters[j].sigma<<'\n';
	}
}

//and now the functions

for(std::string f:functions){

output<<'\n'<<f<<"\n\n";
output<<function_to_double(f, parameters)<<" ± "<<uncertainty(f, parameters)<<'\n';

}

return;

}

long double angular_right(std::vector<Variable>& parameters, long double n){
    long double sum_xy=0, sum_x=0, sum_y=0, sum_x2=0;

    for(int i=0; i<n; ++i){
        sum_xy+=parameters[i].value*parameters[i+n].value;
        sum_x+=parameters[i].value;
        sum_y+=parameters[i+n].value;
        sum_x2+=parameters[i].value*parameters[i].value;
    }

    long double angular=(n*sum_xy-sum_x*sum_y)/(n*sum_x2-sum_x*sum_x);
    return angular;
}

long double linear_right(std::vector<Variable>& parameters, long double n){
    long double sum_xy=0, sum_x=0, sum_y=0, sum_x2=0;

    for(int i=0; i<n; ++i){
        sum_xy+=parameters[i].value*parameters[i+n].value;
        sum_x+=parameters[i].value;
        sum_y+=parameters[i+n].value;
        sum_x2+=parameters[i].value*parameters[i].value;
    }

    long double linear=(sum_x2*sum_y-sum_x*sum_xy)/(n*sum_x2-sum_x*sum_x);
    return linear;
}

long double angular_sigma_right(std::vector<Variable>& parameters, long double n){
    const static long double h=0.00012207031;
    long double var=0,f1, f2, old_value;
    for(int i=0; i<2*n; ++i){
        f1=angular_right(parameters, n);
	old_value=parameters[i].value;
        parameters[i].value+=h;
        f2=angular_right(parameters, n);
        parameters[i].value=old_value;
        var+=parameters[i].sigma*((f2-f1)/h)*parameters[i].sigma*((f2-f1)/h);
    }
    return sqrtl(var);
}

long double linear_sigma_right(std::vector<Variable>& parameters, long double n){
    const static long double h=0.00012207031;
    long double var=0,f1, f2, old_value;
    for(int i=0; i<2*n; ++i){
        f1=linear_right(parameters, n);
	old_value=parameters[i].value;
        parameters[i].value+=h;
        f2=linear_right(parameters, n);
        parameters[i].value=old_value;
        var+=parameters[i].sigma*((f2-f1)/h)*parameters[i].sigma*((f2-f1)/h);
    }
    return sqrtl(var);
}

long double error(std::vector<Variable>& parameters){
    const int n=parameters.size()/2;
    long double X=0, a=angular_right(parameters, n), b=linear_right(parameters, n);
    for(int i=0; i<n; ++i){
        X+=(parameters[i+n].value-(a*parameters[i].value+b))*(parameters[i+n].value-(a*parameters[i].value+b))/*/(parameters[i].sigma*parameters[i].sigma+parameters[i+n].sigma*parameters[i+n].sigma)*/;
    }
    return sqrtl(X/(n-1));
}

long double a_2(std::vector<Variable>& parameters, long double n){
    long double s0=0, s1=0, s2=0, s3=0, s4=0, yx0=0, yx1=0, yx2=0;
    for(int i=0; i<n; ++i){
        s0+=1;
        s1+=parameters[i].value;
        s2+=std::pow(parameters[i].value, 2);
        s3+=std::pow(parameters[i].value, 3);
        s4+=std::pow(parameters[i].value, 4);
        yx0+=parameters[i+n].value;
        yx1+=parameters[n+i].value*parameters[i].value;
        yx2+=parameters[n+i].value*std::pow(parameters[i].value, 2);
    }
    return (yx0*s1*s1*s3-yx0*s1*s2*s2+yx1*s1*s1*s2-yx1*s0*s1*s3+yx2*s0*s1*s2-yx2*s1*s1*s1)/(s0*s1*s2*s4-s0*s1*s3*s3+s1*s1*s2*s3-s1*s1*s1*s4+s1*s1*s2*s3-s1*s2*s2*s2);
}

long double a_1(std::vector<Variable>& parameters, long double n){
    long double s0=0, s1=0, s2=0, s3=0, s4=0, yx0=0, yx1=0, yx2=0;
    for(int i=0; i<n; ++i){
        s0+=1;
        s1+=parameters[i].value;
        s2+=std::pow(parameters[i].value, 2);
        s3+=std::pow(parameters[i].value, 3);
        s4+=std::pow(parameters[i].value, 4);
        yx0+=parameters[i+n].value;
        yx1+=parameters[n+i].value*parameters[i].value;
        yx2+=parameters[n+i].value*std::pow(parameters[i].value, 2);
    }
    return (yx2*s1-yx1*s2-a_2(parameters, n)*(s1*s4-s2*s3))/(s1*s3-s2*s2);
}
long double a_0(std::vector<Variable>& parameters, long double n){
    long double s0=0, s1=0, s2=0, s3=0, s4=0, yx0=0, yx1=0, yx2=0;
    for(int i=0; i<n; ++i){
        s0+=1;
        s1+=parameters[i].value;
        s2+=std::pow(parameters[i].value, 2);
        s3+=std::pow(parameters[i].value, 3);
        s4+=std::pow(parameters[i].value, 4);
        yx0+=parameters[i+n].value;
        yx1+=parameters[n+i].value*parameters[i].value;
        yx2+=parameters[n+i].value*std::pow(parameters[i].value, 2);
    }
    return (yx0-s1*a_1(parameters, n)-s2*a_2(parameters, n))/(s0);
}

long double a_2_sigma(std::vector<Variable>& parameters, long double n){
    const static long double h=0.00012207031;
    long double var=0, f2, f1, old_value;
    for(int i=0; i<2*n; i++){
	old_value=parameters[i].value;
        parameters[i].value+=h;
        f2=a_2(parameters, n);
        parameters[i].value=old_value;
        f1=a_2(parameters, n);
        var+=((f2-f1)/h)*((f2-f1)/h)*parameters[i].sigma*parameters[i].sigma;
    }
    return sqrtl(var);
}

long double a_1_sigma(std::vector<Variable>& parameters, long double n){
    const static long double h=0.00012207031;
    long double var=0, f2, f1, old_value;
    for(int i=0; i<2*n; i++){
	old_value=parameters[i].value;
        parameters[i].value+=h;
        f2=a_1(parameters, n);
        parameters[i].value=old_value;
        f1=a_1(parameters, n);
        var+=((f2-f1)/h)*((f2-f1)/h)*parameters[i].sigma*parameters[i].sigma;
    }
    return sqrtl(var);
}

long double a_0_sigma(std::vector<Variable>& parameters, long double n){
    const static long double h=0.00012207031;
    long double var=0, f2, f1, old_value;
    for(int i=0; i<2*n; i++){
	old_value=parameters[i].value;
        parameters[i].value+=h;
        f2=a_0(parameters, n);
        parameters[i].value=old_value;
        f1=a_0(parameters, n);
        var+=((f2-f1)/h)*((f2-f1)/h)*parameters[i].sigma*parameters[i].sigma;
    }
    return sqrtl(var);
}

long double error_parabola(std::vector<Variable>& parameters){
    const int n=parameters.size()/2;
    long double X=0, a2=a_2(parameters, n), a1=a_1(parameters, n), a0=a_0(parameters, n);
    for(int i=0; i<n; ++i){
        X+=(parameters[i+n].value-(a2*parameters[i].value*parameters[i].value+a1*parameters[i].value+a0))*(parameters[i+n].value-(a2*parameters[i].value*parameters[i].value+a1*parameters[i].value+a0))/*/(parameters[i].sigma*parameters[i].sigma+parameters[i+n].sigma*parameters[i+n].sigma)*/;
    }
    return sqrtl(X/(n-1));
}

bool is_operator(char c){


if(c=='+')
{
    return true;
}

if(c=='-')
{
    return true;
}

if(c=='*')
{
    return true;
}

if(c=='/')
{
    return true;
}

if(c=='^')
{
    return true;
}

return false;
}

bool is_operator_or_parenthesis(char c){

if(c=='+')
{
    return true;
}

if(c=='-')
{
    return true;
}

if(c=='*')
{
    return true;
}

if(c=='/')
{
    return true;
}

if(c=='^')
{
    return true;
}

if(c=='(')
{
    return true;
}

if(c==')')
{
    return true;
}

return false;

}

void read_operand(std::string& function, std::string& c, int& i){
c="";
int limit=function.length();
while(i<limit && !is_operator_or_parenthesis(function[i])){
c=c+function[i];
i++;
}
return;
}


void print_remaining_file(std::fstream& file){
char ch;
std::cout<<"remaining file:";
while(file.get(ch)){
	printf("%c", ch);
}
}


bool is_bracket_next(std::fstream& file){//only use when bracket (i.e. ] ) is known to exist somewhere next in line

	char ch;
	file>>ch;
	file.putback(ch);
	return ch==']' ? true : false;
}


bool is_parameter_in_function(std::string& var_name, std::string& f){

	return f.find(var_name)!=std::string::npos ? true : false;

}
