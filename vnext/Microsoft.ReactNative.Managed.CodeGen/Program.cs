// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  public class Program
  {
    /// <summary>
    /// Main entry point
    /// </summary>
    public static async Task<int> Main(string[] args)
    {
      if (Environment.GetEnvironmentVariable("CodeGenDebugOnstart") == "1")
      {
        System.Diagnostics.Debugger.Launch();
      }
      
      using (var cancellationTokenSource = new CancellationTokenSource())
      {
        Console.CancelKeyPress += (_, e) =>
        {
          cancellationTokenSource.Cancel();
          e.Cancel = true;
        };

        var options = new Options();
        if (!options.TryParse(args))
        {
          return 1;
        }

        var app = new App(cancellationTokenSource.Token);
        var success = await app.Run(options);
        return success ? 0 : 1;
      }
    }
  }
}
