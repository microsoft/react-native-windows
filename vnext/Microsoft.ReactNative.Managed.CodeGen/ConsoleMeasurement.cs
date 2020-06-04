// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Humanizer;
using System;
using System.Diagnostics;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Little helper to measure and print durations of phases of execution
  /// </summary>
  public readonly struct ConsoleMeasurement : IDisposable
  {
    private readonly Stopwatch m_stopWatch;

    public ConsoleMeasurement(string message)
    {
      Console.Write(message);
      m_stopWatch = Stopwatch.StartNew();
    }

    public void Dispose()
    {
      Console.WriteLine($" [{m_stopWatch.Elapsed.Humanize(2)}]");
    }

  }
}
