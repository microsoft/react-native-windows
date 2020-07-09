// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative.Managed.CodeGen.Commandline;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;

namespace Microsoft.ReactNative.Managed.CodeGen
{
    /// <summary>
    /// Captures the options needed to run the Code Generators.
    /// This class is typically populated from the commandline arguments
    /// </summary>
    public class Options
    {
        private const string m_defaultName = "Microsoft.ReactNative.Managed.CodeGen";

        public List<string> SourceFiles { get; } = new List<string>();

        public List<string> References { get; } = new List<string>();

        public List<string> Defines { get; } = new List<string>();

        public string Namespace { get; set; } = m_defaultName;

        public string OutputFile { get; set; } = m_defaultName + ".g.cs";

        public bool Help { get; set; }

        public bool TryParse(string[] args)
        {
            if (!Parser.TryParseArgs(args, out var parsedArgs))
            {
                return false;
            }

            foreach (var parsedArg in parsedArgs)
            {
                if (
                  TryProcess(parsedArg, "r", "reference", References, isFile: true) ||
                  TryProcess(parsedArg, "s", "sourceFile", SourceFiles, isFile: true) ||
                  TryProcess(parsedArg, "d", "define", val => Defines.Add(val)) ||
                  TryProcess(parsedArg, "o", "outputFile", val => OutputFile = val) ||
                  TryProcess(parsedArg, "ns", "nameSpace", val => Namespace = val) ||
                  TryProcess(parsedArg, "h", "help", _ => Help = true) ||
                  TryProcess(parsedArg, "?", "???", _ => Help = true)
                )
                {
                    continue;
                }

                if (!Help)
                {
                    Console.Error.WriteLine($"Error: Unsupported argument '{parsedArg.OriginalArg}'");
                }

                ShowHelp();
                return false;
            }

            return true;
        }

        private void ShowHelp()
        {
            Console.WriteLine("-- sourceFile (s)   C# Source files to analyzer and generate code for.");
            Console.WriteLine("-- reference (r)    The references needed to load those source files.");
            Console.WriteLine("-- define (/d)      The defines used in the source files.");
            Console.WriteLine("-- outputFile (/o)  The output path to write the generated file in.");
            Console.WriteLine("-- nameSpace (/ns)  The namespace to put the generated class in.");
            Console.WriteLine("-- help (/h)        Shows this help.");
        }

        private static bool TryProcess(
          ParsedArgument parsedArg,
          string shortName,
          string longName,
          Action<string> handleValue)
        {
            if (parsedArg.Matches(shortName, longName))
            {
                if (!string.IsNullOrEmpty(parsedArg.Value))
                {
                    handleValue(parsedArg.Value!);
                    return true;
                }

                Console.Error.WriteLine($"Error: Missing required value for argument '{parsedArg.OriginalArg}'");
                return false;
            }

            return false;
        }

        private static bool TryProcess(
          ParsedArgument parsedArg,
          string shortName,
          string longName,
          ICollection<string> args,
          bool isFile)
        {
            if (parsedArg.Matches(shortName, longName))
            {
                var arg = parsedArg.Value;

                if (isFile)
                {
                    if (!TryGetFilePath(parsedArg, out var filePath))
                    {
                        return false;
                    }

                    arg = filePath;
                }

                if (arg != null)
                {
                    args.Add(arg);
                }

                return true;
            }

            return false;
        }

        private static bool TryGetFilePath(ParsedArgument parsedArg, [NotNullWhen(returnValue: true)] out string? filePath)
        {
            if (string.IsNullOrEmpty(parsedArg.Value))
            {
                Console.Error.WriteLine($"Error: Argument '{parsedArg.OriginalArg}' does not specify a value.");
                filePath = null;
                return false;
            }

            filePath = Path.GetFullPath(parsedArg.Value);
            if (!File.Exists(filePath))
            {
                Console.Error.WriteLine($"Missing file: {filePath}");
                return false;
            }

            return true;
        }


    }
}
