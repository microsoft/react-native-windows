# Universal Windows Platform performance analysis

## Tools
1. [Windows Performance Analyzer](https://msdn.microsoft.com/en-us/windows/desktop/hh448170.aspx) - the only usable tool so far
2. Standard profiler - doesn't give much information, but still could be useful
3. [DotTrace](https://www.jetbrains.com/profiler/) profiler from JetBrains, gives the same level of info as Windows Performance Analyzer
4. [Intel vTune](https://software.intel.com/en-us/vtune) - Does not support UWP or WPF-specific profiling, but gives low-level CPU and GPU breakdowns.

## Windows Performance Analyzer
You should have all the tools downloaded as part of UWP sdk, but in case you don't - download the latest version of the Windows Performance Tools Kit, and install it on your machine. (http://www.microsoft.com/whdc/system/sysperf/perftools.mspx : Windows Performance Tools Kit, v.4.1.1 (QFE))
To utilize this powerful tool you need to turn on application Tracing first.

To do so you need to create an instance of Windows.Foundation.Diagnostic.FileLoggingSession class and add Tracer.Instance channel to this session:

```
       static Tracer()
        {
            loggingSession = new FileLoggingSession("performance");
            loggingSession.AddLoggingChannel(Instance);
        }

```

This will enable application level tracing for react-native code.
You could start and stop application level profiling dynamically by adding and removing the channel.

I think it would make sence to hide tracing under define and add context menu for profiling features later.

You would see the logs in following folder:
~\AppData\Local\Application Data\Packages\%PACKAGE_UID%\LocalState\Logs\
log name would be: Log-performance-*.etl
it rotates every 270Kb or so.

I would recommend you to record additional system level information using xperf tool.
TODO: find out how to scope tool results to specific process (ideally add to the same file session)

To start event recording run:
```
xperf start -on DiagEasy 
```
To stop recording run:
```
xperf stop -d filename.etl
```

Windows Performance Analyzer works with single file so you need to merge all your files:

```
xperf merge *.etl merge.etl
```

After that you could open the resulting merge.etl file in WPA tool and see what happens inside of react code.


## More info
1. https://blogs.msdn.microsoft.com/ntdebugging/2008/04/03/windows-performance-toolkit-xperf/
2. https://msdn.microsoft.com/en-us/library/windows/hardware/hh448170.aspx



 





