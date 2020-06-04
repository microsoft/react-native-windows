// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed.CodeGen.Commandline
{
  internal readonly struct ParsedArgument
  {
    public string OriginalArg { get; }

    public string? ShortName { get; }

    public string? LongName { get; }

    public string? Value { get; }

    private ParsedArgument(string originalArg, string? shortName, string? longName, string? value)
    {
      OriginalArg = originalArg;
      ShortName = shortName;
      LongName = longName;
      Value = value;
    }

    public static bool TryParse(string arg, out ParsedArgument parsedArgument)
    {
      parsedArgument = default;

      var splitIndex = arg.IndexOf(':');
      string key;
      string? value = null;
      if (splitIndex > 0)
      {
        key = arg.Substring(0, splitIndex);
        value = StripQuotes(arg.Substring(splitIndex + 1));

        if (value.Length == 0)
        {
          Console.Error.WriteLine($"Error: Unsupported commandline argument '{key}': Missing value.");
          return false;
        }

      }
      else
      {
        key = arg;
      }

      if (TryParseKey(key, out var shortName, out var longName))
      {
        parsedArgument = new ParsedArgument(arg, shortName, longName, value);
        return true;
      }

      return false;
    }

    internal static string StripQuotes(string value)
    {
      if (value.Length >= 2 && value.StartsWith("\"") && value.EndsWith("\""))
      {
        return value.Substring(1, value.Length - 2);
      }
      else
      {
        return value;
      }
    }

    public bool Matches(
      string shortName,
      string longName)
    {
      return string.Equals(shortName, ShortName, StringComparison.Ordinal) ||
             string.Equals(longName, LongName, StringComparison.Ordinal);
    }

    private static bool TryParseKey(string key, out string? shortKey, out string? longKey)
    {
      if (string.IsNullOrEmpty(key))
      {
        Console.Error.WriteLine($"Error: Unsupported commandline argument '{key}': Empty is not a valid argument.");
        shortKey = null;
        longKey = null;
        return false;
      }

      switch (key[0])
      {
        case '-':
          if (key.Length >= 2 && key[1] == '-')
          {
            if (key.Length == 2)
            {
              Console.Error.WriteLine($"Error: Unsupported commandline argument '{key}': Missing name of the argument");
              shortKey = null;
              longKey = null;
              return false;
            }

            shortKey = null;
            longKey = key.Substring(2);
          }
          else
          {
            if (key.Length <= 1)
            {
              Console.Error.WriteLine($"Error: Unsupported commandline argument '{key}': Missing name of the argument");
              shortKey = null;
              longKey = null;
              return false;
            }

            shortKey = key.Substring(1);
            longKey = null;
          }

          return true;

        case '/':
          if (key.Length <= 1)
          {
            Console.Error.WriteLine($"Error: Unsupported commandline argument '{key}': Missing name of the argument");
            shortKey = null;
            longKey = null;
            return false;
          }

          shortKey = key.Substring(1);
          longKey = shortKey;
          return true;
        default:
          Console.Error.WriteLine($"Error: Unsupported commandline argument: {key}");
          shortKey = null;
          longKey = null;
          return false;
      }
    }
  }
}
