<h1 align="center">
    <img src="dnyAquaShell/logo.png" width="100"/><br/>
    AquaShell
</h1>

<p align="center">
    A scripting and automation shell for Windows<br/>
    (C) 2017 - 2024 by Daniel Brendel<br/>
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
* DNYAS_IS_INTERACTIVE_MODE: Boolean value that indicates whether or not the current script code runs within the interactive mode
* CR: Represents a carriage return character
* LF: Represents a line feed character
* void: Can be used with commands or functions if you want to dismiss the result value.

## Helper commands:
* getscriptpath (result) # Stores the full path of the current executed script in the result var
* getscriptname (result) # Stores the full file name of the current executed script in the result var
* textview (file) # Prints the text contents of the file to the standard output
* random (start) (end) (result var) # Generates a random number within the range and stores it to the result var
* sleep (milliseconds) # Pauses the main thread for N milliseconds
* gettickcount (result var) # Stores the number  of milliseconds that have elapsed since system start into the result var
* getsystemerror (result var) # Stores the last system error code of the shell process context into the result var
* setsystemerror (code) # Sets the last system error code of the shell process context

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
The first one is called when the plugin gets loaded. There you must implement all loading stuff. The function recieves the current shell interface version, a pointer to the plugin API class instance and a pointer to a plugin information structure where the plugin should save its information strings. If everything goes well then the plugin must return true, otherwise false.

Here is an example of a plugin info struct object.

```cpp
plugininfo_s g_sPluginInfos = {
	L"Plugin name",
	L"1.0",
	L"Author name",
	L"Contact info",
	L"Plugin description"
};
```

The latter one is called when the plugin gets unloaded. There you can implement all cleanup stuff. 

Please refer to the demo plugin sourcecode in order to view a full documented example.

## Interpreter
This software is using dnyScriptInterpreter v1.0 developed by Daniel Brendel.
Please refer to the script interpreter [readme](interpreter/README.md) for more details.

## Changelog:
* Version 1.0:
	* (Initial release)