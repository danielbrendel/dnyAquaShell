<h1 align="center">
    <img src="dnyAquaShell/logo.png" width="100"/><br/>
    AquaShell
</h1>

<p align="center">
    A scripting and automation shell for Windows<br/>
    (C) 2017 - 2025 by Daniel Brendel<br/>
    Released under the MIT license
</p>

## Information

**Version**: 1.0\
**Codename**: dnyAquaShell\
**Contact**: dbrendel1988(at)gmail(dot)com\
**GitHub**: https://github.com/danielbrendel/

## Description
AquaShell is an extendable scripting shell for Windows using dnyScriptInterpreter. The shell can be used either in interactive mode or just as a host for running DNYS script files. The functionality of the shell can be extended via native DLL plugins. It is best suited for automation tasks as well as complex script development.

## Example code
```
# Example script code
# Demonstrate recursive function calls

const MAX_COUNT int <= 10;

function recursive void(count int)
{
  if (%count, -ls, %MAX_COUNT) {
    ++ count;
    print "Count value: %count";
    call recursive(%count) => void;
  };
};

call recursive(0) => void;

print "Done.";

pause;
```

## Basic commands:
* require (library) # Attempts to load a plugin library. Only needed when not running in interactive mode. Use `__ALL__` to load all available plugins.
* exec (scriptfile) (opt:args) # Executes a script file. In the script use %argc for the argument count and %argv[n] for the actual argument value. Using %n, where n is the argument index value, is also possible.
* sys (string) (opt:storage var) # Attempts to run the expression as a windows command. If a result var is provided the output is stored in it, else the output gets echoed.
* run (file) (args) (dir) # Attempts to launch a specified file
* cwd (dir) # Changes the current working directory
* gwd (result var) # Stores the current working directory in the result var
* pause # Pause current script execution
* listlibs # Lists all shell plugin libraries
* quit # Exists the shell

## Helper constants and variables
* DNYAS_SHELL_VERSION: Contains the current product version as string
* DNYAS_SHELL_VERNUM: Contains the current product version as a numeric value, in a concatenated manner
* DNYAS_PLATFORM_NAME: The platform name of the product build, either x64 or x86
* DNYAS_BUILD_TYPE: The build type of the product, either Release or Debug
* DNYAS_IS_INTERACTIVE_MODE: Boolean value that indicates whether or not the current script code runs within the interactive mode
* DNYAS_BASE_PATH: Contains the base path of the shell
* DNYAS_PLUGIN_PATH: Contains the path to the plugin directory
* DNYAS_DEFSCRIPTS_PATH: Contains the path to the default scripts directory
* CR: Represents a carriage return character
* LF: Represents a line feed character
* TAB: Represents a tab character
* QUOT_SINGLE: Represents a single quotation character
* QUOT_DOUBLE: Represents a double quotation character
* void: Can be used with commands or functions if you want to dismiss the result value.

## Helper commands:
* getscriptpath (result var) # Stores the full path of the current executed script in the result var
* getscriptname (result var) # Stores the full file name of the current executed script in the result var
* debug (text content) # Prints a system debug message
* textview (file) # Prints the text contents of the file to the standard output
* random (start) (end) (result var) # Generates a random number within the range and stores it to the result var
* sleep (milliseconds) # Pauses the main thread for N milliseconds
* bitop (operation) (array of operands) (result var) # Performs either a bitwise OR, AND or XOR operation and stores it to the result var
* gettickcount (result var) # Stores the number  of milliseconds that have elapsed since system start into the result var
* timestamp (result var) # Stores the current system timestamp into the result var
* fmtdatetime (format string) (opt:timestamp) (result var) # Creates a formatted datetime string and stores it into the result var
* getsystemerror (result var) # Stores the last system error code of the shell process context into the result var
* setsystemerror (code) # Sets the last system error code of the shell process context
* threadfunc (function name) (opt:args) # Executes a function in a separate thread. Optionally an array of arguments can be passed
* hideconsole # Hides the console window for the rest of the execution. Should only be used in scripts that don't need a console window

## Multiline support:
* \< # Opens the editor in multiline mode
* (code)
* \> # Closes the multiline mode and executes the script code

## Init and unload script:
* You can place a script named 'init.dnys' in the 'scripts' directory of the base directory
  which will get executed when the shell is in loading progress. There you can place
  initialization code
* You can place a script named 'unload.dnys' in the 'scripts' directory of the base directory
  which will get executed when the shell gets unloaded. There you can place cleanup code

## Executing script files

Script files can be executed by running

```
dnyAquaShell.exe "path/to/a/script.dnys" [opt:args]
```

Arguments are optional, but can be provided if required.

```
dnyAquaShell.exe "path/to/a/script.dnys" "A string with multiple words" 1234 false true 5.23
```

In the context of a script, you can access the actual expressions as well as the argument count as follows:

```
%argc: Amount of arguments
%argv[num]: Actual argument expression
```

If you have added the shell path to your environment PATH variable, then you can also perform actions from any directory as follows:
```
aquashell [args]
```

## Command line arguments

The following command line arguments exist:

* -v: Prints out the version information
* -c "script code": Execute script code that is provided as an argument
* -libs: Lists all available plugins
* -path user|machine: Adds the shell path to your PATH environment variable, either for current user or local machine

You can run them via

```
dnyAquaShell.exe -[cmd] [opt:args]
```

## Plugin API:
Plugins must be written in compatibility with the shell application. 

A plugin needs to export the functions `dnyAS_PluginLoad` and `dnyAS_PluginUnload`.

```cpp
bool dnyAS_PluginLoad(dnyVersionInfo version, IShellPluginAPI* pInterfaceData, plugininfo_s* pPluginInfos);
void dnyAS_PluginUnload(void);
```
The `dnyAS_PluginLoad` function is called when the plugin gets loaded. The function recieves the current shell interface version, a pointer to the plugin API class instance and a pointer to a plugin information structure where the plugin stores its information data. The function must return true on success, otherwise it must return false.

```cpp
IShellPluginAPI* g_pShellPluginAPI;

//Example void-command interface
class IExampleVoidCommandInterface : public IVoidCommandInterface {
public:
	IExampleVoidCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		//Replace all possible variables with their expressions that are used in the command context
		pCodeContext->ReplaceAllVariables(pInterfaceObject);

		//Print the first string-argument of the command
		std::wcout << pContext->GetPartString(1) << std::endl;

		//Return true to indicate that the command executed successfully.
		//Returning false will cause the shell to stop script execution and show an error
		return true;
	}

} g_oExampleVoidCommand;

//Example result-type-command interface
class IExampleResultCommandInterface : public IResultCommandInterface<dnyFloat> {
public:
	IExampleResultCommandInterface() {}

	virtual bool CommandCallback(void* pCodeContext, void* pInterfaceObject)
	{
		ICodeContext* pContext = (ICodeContext*)pCodeContext;

		//Replace all possible variables with their expressions that are used in the command context
		pCodeContext->ReplaceAllVariables(pInterfaceObject);

		//Access the first float-argument, multiplicate it by two, and then store it into the passed result variable of type float
		IResultCommandInterface<dnyFloat>::SetResult(pContext->GetPartFloat(1) * 2);

		//Return true to indicate that the command executed successfully.
		//Returning false will cause the shell to stop script execution and show an error
		return true;
	}
} g_oExampleResultCommand;

plugininfo_s g_sPluginInfos = {
	L"Plugin name",
	L"1.0",
	L"Author name",
	L"Contact info",
	L"Plugin description"
};

bool dnyAS_PluginLoad(dnyVersionInfo version, IShellPluginAPI* pInterfaceData, plugininfo_s* pPluginInfos)
{
	//Called when plugin gets loaded

	if ((!pInterfaceData) || (!pPluginInfos))
		return false;

	//Check version
	if (version != DNY_AS_PRODUCT_VERSION_W) {
		return false;
	}

	//Store interface pointer
	g_pShellPluginAPI = pInterfaceData;

	//Store plugin infos
	memcpy(pPluginInfos, &g_sPluginInfos, sizeof(plugininfo_s));

	//Register example commands
	g_pShellPluginAPI->Cmd_RegisterCommand(L"mycommand", &g_oExampleVoidCommand, CT_VOID);
	g_pShellPluginAPI->Cmd_RegisterCommand(L"mycommand2", &g_oExampleResultCommand, CT_FLOAT);

	return true;
}
```

When the plugin is available, the example commands would be used as follows:

```
# Example of using mycommand
mycommand "This will be printed";

# Example of using mycommand2
global fResultVar float;
mycommand2 50 fResultVar; # fResultVar will then contain 100.0
```

The `dnyAS_PluginUnload` function is called when the plugin gets unloaded. Use it to free all resources, clean up memory, etc.

```cpp
void dnyAS_PluginUnload(void)
{
	//Called when plugin gets unloaded

	g_pShellPluginAPI->Cmd_UnregisterCommand(L"mycommand");
	g_pShellPluginAPI->Cmd_UnregisterCommand(L"mycommand2");
}
```

Please refer to the demo plugin sourcecode in order to view a full documented example.

## Interpreter
This software is using dnyScriptInterpreter v1.0 developed by Daniel Brendel.
Please refer to the script interpreter [readme](interpreter/README.md) for more details.
