@ECHO OFF

START %~dp0x64\Debug\Application.exe
START %~dp0x64\Debug\Injector.exe Application.exe Hooking.dll
