# AquaShell (dnyAquaShell) developed by Daniel Brendel

(C) 2017 - 2022 by Daniel Brendel

**Version**: 1.0\
**Contact**: dbrendel1988<at>gmail<dot>com\
**GitHub**: https://github.com/danielbrendel/

Released under the MIT license

This tool is an extendable scripting shell for Windows using dnyScriptInterpreter.
The shell can be used either in interactive mode or just as a host for running
DNYS script files. The functionality of the shell can be extended via native
DLL plugins.

## Basic commands:
* require # Attempts to load a plugin library. Only needed when not running in interactive mode
* exec (scriptfile) # Executes a script file
* sys (string) # Passes the string to the Windows batch system
* run (./) (path> (args) (dir) # Attempts to launch a specified file 
* listlibs # Lists all shell plugin libraries
* quit # Exists the shell

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

## Plugin API:
* Plugins must be written in C++
* A plugin needs to export the functions 'dnyAS_PluginLoad' and 'dnyAS_PluginUnload'
* The first one is called when the plugin gets loaded. There you must implement all
  loading stuff. The function recieves the current shell interface version, a pointer
  to the plugin API class instance and a pointer to a plugin information structure
  where the plugin should save its information strings. If everything goes well then
  the plugin must return true, otherwise false.
* The latter one is called when the plugin gets unloaded. There you can implement
  all cleanup stuff. 
* Please refer to the demo plugin sourcecode in order to view a full documented example

This software is using dnyScriptInterpreter v1.0 developed by Daniel Brendel.
Please refer to dnyScriptInterpreter.md for more details.

## Changelog:
* Version 1.0:
	* (Initial release)