// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed.CodeGen
{
    public class App
    {
        private readonly CancellationToken m_cancellationToken;

        public App(CancellationToken cancellationToken)
        {
            m_cancellationToken = cancellationToken;
        }

        /// <summary>
        /// The main entry point to run the code generation based on the given options.
        ///
        /// Main phases of execution are:
        /// 1. Load the code into roslyn
        ///    1. Parse Source Files into syntax trees
        ///    2. Load the references
        ///    3. Run the compiler to create the Linked semantic model.
        /// 2. Analyze the roslyn Compilation unit
        ///    1. Detect react types in the code
        ///    2. Build a Model for the assembly
        /// 3. Generate code
        ///    1. Use the Model from step 2 to emit a C# Syntax Tree
        ///    2. Write the SyntaxTree to a file.
        /// </summary>
        public async Task<bool> Run(Options options)
        {
            var codeAnalyzer = new CodeAnalyzer(m_cancellationToken);

            // Load the code into roslyn
            using (new ConsoleMeasurement("Parsing source files"))
            {
                await codeAnalyzer.ParseSourceFilesAsync(
                  GetDistinctExistingFiles(options.SourceFiles),
                  options.Defines.Select(def => def.Trim()));
            }

            using (new ConsoleMeasurement("Loading references"))
            {
                await codeAnalyzer.LoadMetadataReferencesAsync(
                  GetDistinctExistingFiles(options.References));
            }

            using (new ConsoleMeasurement("Compiling"))
            {
                if (!codeAnalyzer.TryCompileAndCheckForErrors())
                {
                    CheckForErrors(codeAnalyzer);
                    return false;
                }
            }

            // Extract the model form the assembly
            ReactAssembly assembly;
            using (new ConsoleMeasurement("Finding types"))
            {
                assembly = codeAnalyzer.AnalyzeAndConstructAssemblyModel();
            }

            // Check if any errors have been logged and display them.
            if (!CheckForErrors(codeAnalyzer))
            {
                return false;
            }

            var codeGenerator = new CodeGenerator(codeAnalyzer.ReactTypes, options.Namespace);
            CSharpSyntaxNode node;
            using (new ConsoleMeasurement("Generating code"))
            {
                node = codeGenerator.Generate(assembly);
            }

            var code = node.NormalizeWhitespace(indentation: "  ", elasticTrivia: false).ToFullString();

            if (!await TryWriteFileIfDifferent(options.OutputFile, code))
            {
                return false;
            }

            return true;
        }

        private async Task<bool> TryWriteFileIfDifferent(string path, string contents)
        {
            try
            {
                if (File.Exists(path))
                {
                    var existing = await File.ReadAllTextAsync(path, m_cancellationToken);
                    if (string.Equals(contents, existing))
                    {
                        // File already has the same contents, do not write to disk
                        // so we can keep the timestamp of the file so MSBuild will not
                        // run dependent tasks.
                        return true;
                    }
                }

                // Ensure output directory
                Directory.CreateDirectory(Path.GetDirectoryName(path));

                await File.WriteAllTextAsync(path, contents, m_cancellationToken);

                return true;
            }
            catch (Exception e)
              when (e is IOException || e is UnauthorizedAccessException)
            {
                Console.Error.WriteLine($"Error writing file {path}: {e.Message}");
                return false;
            }
        }

        private bool CheckForErrors(CodeAnalyzer codeAnalyzer)
        {
            bool hasErrors = false;
            if (codeAnalyzer.Diagnostics.Any(diag => diag.Severity == DiagnosticSeverity.Error || diag.Severity == DiagnosticSeverity.Warning))
            {
                var oldColor = Console.ForegroundColor;
                foreach (var diagnostic in codeAnalyzer.Diagnostics)
                {
                    switch (diagnostic.Severity)
                    {
                        case DiagnosticSeverity.Error:
                            Console.ForegroundColor = ConsoleColor.Red;
                            hasErrors = true;
                            break;
                        case DiagnosticSeverity.Warning:
                            Console.ForegroundColor = ConsoleColor.Yellow;
                            break;
                        default:
                            Console.ForegroundColor = oldColor;
                            break;
                    }

                    Console.WriteLine(diagnostic.ToString());
                }

                Console.ForegroundColor = oldColor;
            }

            return !hasErrors;
        }

        /// <summary>
        /// Helper to dedupe and normalize file paths for some of the options.
        /// </summary>
        private static IEnumerable<string> GetDistinctExistingFiles(IEnumerable<string> files)
        {
            if (files == null)
            {
                return Enumerable.Empty<string>();
            }

            return files
              .Select(file => Path.GetFullPath(file.Trim()))
              .Where(filePath => File.Exists(filePath))
              .Distinct(StringComparer.OrdinalIgnoreCase);
        }
    }
}
