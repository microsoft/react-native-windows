// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using Microsoft.CodeAnalysis;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests
{
    public abstract class CodeGenTestBase : AnalysisTestBase
    {
        /// <summary>
        /// This property when set to true will update all LKG files automatically.
        /// </summary>
        protected const bool AutoFixLkgs = false;

        protected void TestCodeGen<TSymbol>(string csSnippet, Func<CodeGenerator, TSymbol, SyntaxNode> generateCode, string lkgName = null)
        {
            TestMultipleCodeGen<TSymbol>(csSnippet, (codeGen, symbols) => generateCode(codeGen, symbols.FirstOrDefault()), lkgName);
        }

        protected void TestMultipleCodeGen<TSymbol>(string csSnippet, Func<CodeGenerator, IEnumerable<TSymbol>, SyntaxNode> generateCode, string lkgName = null)
        {
            var (analyzer, type) = AnalyzeModuleFile(csSnippet);
            IEnumerable<TSymbol> symbols = type.GetMembers().OfType<TSymbol>();
            Assert.IsTrue(symbols.Any());

            var codeGen = new CodeGenerator(analyzer.ReactTypes, "Test.TestNS");
            var syntax = generateCode(codeGen, symbols);

            var codeToCompare = syntax.NormalizeWhitespace().ToFullString();
#pragma warning disable CS0162 // This is here to help update all the LKG's in one go and allow for easy diffing in case a codegen change is made that affects many changes
            if (AutoFixLkgs)
            {
                var lkgSourceFile = GetLkgSourceFile(lkgName);
                File.WriteAllText(lkgSourceFile, codeToCompare);
            }
            else
            {
                var generateMessage = @"To generate the file, set 'CodeGenTestBase.AutoFixLkgs' temporarily to true in C# file 'vnext\Microsoft.ReactNative.Managed.CodeGen.UnitTests\CodeGenTestBase.cs'.";

                var lkgFile = GetLkgPath(lkgName);
                if (!File.Exists(lkgFile))
                {
                    TraceEncountered(codeToCompare, "Encountered");
                    Assert.Fail(
                      $"Could not find expected LKG file: '{lkgFile}'. {generateMessage}");
                }

                var lkgContents = File.ReadAllText(lkgFile);
                if (!String.Equals(lkgContents, codeToCompare, StringComparison.Ordinal))
                {
                    TraceEncountered(lkgContents, "Expected");
                    TraceEncountered(codeToCompare, "Encountered");
                    Assert.AreEqual(lkgContents, codeToCompare, $"Lkg file '{lkgFile}' does not match expected content. {generateMessage}");
                }
#pragma warning restore CS0162
            }
        }

        private void TraceEncountered(string contents, string title)
        {
            TestContext.WriteLine("*********************");
            TestContext.WriteLine("* " + title);
            TestContext.WriteLine("*********************");
            TestContext.WriteLine(contents);

            TestContext.WriteLine("*********************");
        }

        private string GetLkgPath(string lkgName)
        {
            return Path.Combine(TestContext.TestDeploymentDir, GetRelativeLkgPath(lkgName));
        }

        private string GetLkgSourceFile(string lkgName, [CallerFilePath] string file = "")
        {
            var sourceRoot = Path.GetDirectoryName(file);
            return Path.Combine(sourceRoot, "CodeGen", GetRelativeLkgPath(lkgName));
        }

        private string GetRelativeLkgPath(string lkgName)
        {
            var suffix = string.IsNullOrEmpty(lkgName) ? lkgName : "--" + lkgName;
            return Path.Combine("Lkg", TestContext.FullyQualifiedTestClassName + "--" + TestContext.TestName + suffix + ".lkg");
        }

        protected IPropertySymbol ParseProp(string csSnippet) => ParseMemberSymbol<IPropertySymbol>(csSnippet);

        protected TSymbol ParseMemberSymbol<TSymbol>(string csSnippet)
          where TSymbol : ISymbol
        {
            var (_, type) = AnalyzeModuleFile(csSnippet);
            return type.GetMembers().OfType<TSymbol>().First();
        }
    }
}
