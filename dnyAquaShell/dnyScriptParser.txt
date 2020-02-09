dnyScriptParser developed by Daniel Brendel
======================================

(C) 2017-2018 by Daniel Brendel

Version: 0.1
Contact: dbrendel1988<at>gmail<dot>com
GitHub: https://github.com/danielbrendel

Licence: Creative Commons Attribution-NoDerivatives 4.0 International

Feature overview:
*****************
+ Scripting Engine for application-embedded scripting
+ User-friendly scripting syntax
+ Interface between script engine and engine user (data, code)
+ Default internal commands
+ Sticks to C++ STD in order to provide compatibility
 
Syntax:
*******
The script language is a command-based language. This makes the syntax be as follows:
<cmd_ident> <expression2> <expression3> <expression n...>;
Now 'cmd_ident' is the identifier of a specific command. 'expression2' - 'expression n'
are considered arguments to a specific command. Each command must be ended with a semicolon (";"). 
In order to allow complex scripts it is neccessary to summarize/collect expressions with collector characters. 
These are quotation marks ("") and brackets ("()" and "{}"). This allows you to format your command call
as follows:
myCommandIdent "argument1" (argument2a, arguments3a, ...) { complex argument ("test1", "test2");};
Each command or empty line can be noted with a '#<comment>' to comment your scripts.
When writing your scripts you should pay attention to a good coding-style which offers the reader
to have a better glance at your scripts. Some commands require an argument expression and a code expression.
Altough not neccessarly required you should put argument expressions in () brackets and code expressions
in {} brackets (note: it depends on the command provider whether argument or code expressions are required. 
Most of the times you will just encounter parameterized commands). See the example below:
#The following function implementation works, but looks like bad style
function myFunction string "arg1 string, arg2 string" "print %arg1%arg2;";
You can see that you couldn't include spacing between the argument accessor code. A better example would be:
function myFunction string(arg1 string, arg2 string)
{
print "%arg1 %arg2"; #This is a comment
};
As you can see some commands may require arguments provided as an array list, separated by a commas,
in a single expression.

Examples:
*********

1) Using comments
Comments are indicated by a #-character. Everything what follows
a comment is ignored by the parser. 

2) Declaring global and local variables
declare name type;
There are the following system types available:
- bool -> A boolean value (true and false)
- int -> A signed integer which is 64 bits in size
- float -> A signed float which is 64 bits in size
- string -> A string
Additionally custom variable data types can be used.
A declaration then is applied as follows:
declare name customtypename;
Variables registered via 'declare' are always global variables. In order to use
local temporary variables in functions you need to use the 'local' keyword. 
local name datatype;
Local function variables are automatically removed after the function has finished.
If you have a local and a global variable with the same name then the local variable
is prefered. As a result of that clean naming is advised.

3) Assigning values to variables
declare name1 int;
set name1 <= 100;
local name2 string;
set name2 <= "This is a test";

4) Creating functions
function myFunction type(arguments)
{
#code
result 0;
};
Functions can either return a value or not. Also providing arguments is optional.
The function arguments can handle the same types as shown in the variables section.
If a function shall not return a value then the returntype shall be set as 'void'.
function myVoidFunction void()
{
};
function myFloatFunction float()
{
result 0.1;
}
function myComplexFunction int(arg1 string, arg2 float, arg3 bool)
{
result 0;
}
The functions are called as follows:
call myVoidFunction();
call myFloatFunction() => myFloatVar;
call myComplexFunction("test1", 0.1, true) => myIntVar;
You can also use local variables in functions:
function myMoreComplexFunction float(arg1 float)
{
local temp float;
set temp <= %arg1;
*= temp %arg1;
result %temp;
};

5) Commands
The script user can implement own script commands which can retrieve arguments
and also return a value. 
#A void command:
myCommand "stringarg"
#A value-returning command:
declare myStorageVar type;
myRetValCommand "stringarg" myStorageVar
For void-commands you need to implement the IVoidCommandInterface class and for
value-returning commands you need to implement the IResultCommandInterface<type> class.
Note that both type of commands can return a boolean value to indicate errors. These
errors are handled solely by the script engine and you should only indicate errors
if the script execution is considered uncontinueable. If you want to return values which 
do not affect further script execution and which shall be handled by the script code then 
you should stick to the result-command interface. It supports all dny-data types.

Internal command reference:
***************************
const constname consttype <= value; //Registers a constant with the given name, type and value
declare varname vartype; //Registers a global variable with the associated type
set varname <= value; //Assigns a value to a global/local variable
undeclare varname; //Removes a global variable
function name rettype(paramters) {code} //Defines a function
local varname vartype; //Registers a local function variable inside a function with the associated type
result value; //Sets the result value of the associated function
call funcname(arguments) => resultvar //Calls a function with paramters (if required) and stores the result (if required)
if (cond1, operation, cond2) {code} <elseif (cond1, operation, cond2) {code}> <else {code}> //Performs an if-elseif-else evaluation. Elseif's and else's are optional. 'operation' can either be -eq(equal), -nt(not equal), -ls(less than), -lse(less than or equal), -gr(greater than) and -gre(greater than or equal).
for (varname, startvalue, endvalue, step) {code} //Performs a for-loop with positive or negative values (use -inc for 1 and -dec for -1 or a step value)
while (cond1, operation, cond2) {code} //Performs a while-loop. 'operation' can either be -eq(equal), -nt(not equal), -ls(less than), -lse(less than or equal), -gr(greater than) and -gre(greater than or equal).
execute scriptfile; //Executes a script file relative to the base directory
print text; //Prints text to the standard output if set by script user
exit; //Aborts script execution
++ var; //Increments a variable
-- var; //Decrements a variable
+= var value; //Adds the value to the variable
-= var value; //Subtracts the value from the variable
*= var value; //Multiplicates the value to the variable
/= var value; //Divides the value from the variable
+ var value1 value2; //Stores the sum to the variable
- var value1 value2; //Stores the difference to the variable
* var value1 value2; //Stores the product to the variable
/ var value1 value2; //Stores the quotient to the variable
! boolvar; //Reverses the boolean value of the variable
 
Usage:
******
- Instantiate an object of CScriptingInterface object
- Set a script base directory with SetScriptBaseDirectory() method if required
- Set a standard output routine with SetStandardOutput() method if required
- Register custom data types with RegisterDataType() if required
- Register commands with RegisterCommand() method or variables with RegisterCVar() method 
  (both kind of objects can also be unregistered if not needed anymore). Implement classes
  derived from IVoidCommandInterface interface for void-commands and IResultCommandInterface<(
  dnyDataType)> interface for commands that return a value.
- Execute scripts via ExecuteScript() method or code via ExecuteCode() method
- Use the methods BeginFunctionCall(), ExecuteFunction() and EndFunctionCall() to call a 
  script function. If required you can push functions via the PushFunctionParam() methods
  and query function result data via the QueryFunctionResultAs*() methods.
- Possible operation status values can be obtained via GetErrorInformation() method. It returns
  a class object instance which can be used to query the operation status code and an additional
  error message if the status was set to an error indicating value

System requirements:
********************
+ ISO-conform C++ compiler

Changelog:
**********
# Version 0.1:
	+ (Initial release)