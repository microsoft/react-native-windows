# ChakraCore Debugging with Visual Studio
This project creates a fake ChakraCore.dll which acts as a shim and translates calls from the ChakraCore to ChakraRT.
Since ChakraRT supports in-process script debugging in VS, this means you would be then be able to attach to process
in "Script" debugging mode and be able to debug React Native JavaScript bundles.

You can clone the repo and build the ChakraCoreShim project yourself to get the shim ChakraCore.dll, or you can check
\\\\officefile\public\denizdem\ChakraCore2RTShim and just grab the fake dlls from there. You then need to overwrite the
actual ChakraCore.dll on your machine. Check Modules window of Visual Studio to see where ChakraCore is getting loaded from,
and replace that dll with the shim.

That is all you have to do. This shim dll will make all React Native instances and Chakra contexts debuggable by default.
Once the script is loaded into the context, find the script file in Solution Explorer, where you can start putting
breakpoints.


## Debugging Script Load Time
Script file will appear in VS Solution Explorer window only after the script is fully loaded (through JsRun or JsParse),
which makes debugging initial script load time a bit harder. There is a simple fix though. Adding the statement
"debugger;" (without the quotes) to the very top of your React Native bundle.js file would cause Visual Studio debugger
to automatically break-in before anything after the statement executes. Diff would look like:

```bash
1				(function(global) {
2
3		+		  debugger;
4		+
5				global.__DEV__ = true;
6
7				global.__BUNDLE_START_TIME__ = global.nativePerformanceNow ? global.nativePerformanceNow() : Date.now();
8				})(typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : this);
9				(function(global) {
```


## Is Remote Debugging in Chrome dead?
No. Chrome offers a really nice debugger that offers many capabilities Visual Studio script debugger does not.
Besides, ChakraCore is really used only in Win32 platforms, so other platforms would need to continue to use
Chrome debugger anyway.

Also, this solution works only on Windows 10, where ChakraRT is present. In prior versions of Windows, we would have to
use remote debugging through Chrome even for Win32.


## Cool, can I start shimming to other JS engines now?
You definitely can, using the same technique. In fact, ChakraCoreShim project also implements shimming to legacy
Chakra (the one called as jsrt9) as well, given signatures are very similar between ChakraRT and legacy Chakra.

This does not work with React Native bundles though, given legacy Chakra dll has not been geting any investments lately
and does not support the latest developments JavaScript world had. It barfs when trying to parse/compile the bundle.
But the redirection/shim works.

In order to test this out, you need to do two things. Both are configurable through the project properties in VS:
1) Remove USE_EDGEMODE_JSRT prom the preprocessor defines of the project, which would cause the shim to use legacy
Chakra APIs, not Microsoft Edge mode Chakra ones.
2) Remove chakrart.lib from the linked libs of the project and add jsrt.lib.
