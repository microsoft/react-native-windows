// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Microsoft.ReactNative.Managed.CodeGen.Commandline
{
  public class Parser
  {
    internal static bool TryParseArgs(IEnumerable<string> args, out IList<ParsedArgument> parsedArgs)
    {
      parsedArgs = new List<ParsedArgument>();
      return ParseArgs(args, parsedArgs);
    }

    private static bool ParseArgs(IEnumerable<string> args, IList<ParsedArgument> parsedArgs)
    {
      foreach (var arg in args)
      {
        if (arg.StartsWith('@'))
        {
          var responseFilePath = ParsedArgument.StripQuotes(arg.Substring(1));
          try
          {
            var lines = File.ReadAllLines(responseFilePath);
            var filteredLines = lines
              .Select(line => line.Trim())
              .Where(line => !line.StartsWith('#') && !string.IsNullOrEmpty(line))
              .ToList();
            // Recurse to allow response files to have response files.
            if (!ParseArgs(filteredLines, parsedArgs))
            {
              return false;
            }
          }
          catch (Exception e) when (e is IOException || e is UnauthorizedAccessException)
          {
            Console.Error.WriteLine($"Error loading response file '{responseFilePath}': {e.Message}");
            return false;
          }
        }
        else
        {
          if (!ParsedArgument.TryParse(arg, out var parsedArg))
          {
            return false;
          }

          parsedArgs.Add(parsedArg);
        }
      }

      return true;
    }
  }
}
