// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using System.Collections.Generic;
using System.Diagnostics.ContractsLight;
using System.Linq;
using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;
using static Microsoft.ReactNative.Managed.CodeGen.SyntaxHelpers;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Class to generate code given the passed in models
  /// </summary>
  public partial class CodeGenerator
  {
    internal readonly ReactTypes ReactTypes;

    private readonly string m_rootNamespace;

    public CodeGenerator(ReactTypes reactTypes, string rootNamespace)
    {
      Contract.Requires(!string.IsNullOrEmpty(rootNamespace));

      ReactTypes = reactTypes;
      m_rootNamespace = rootNamespace;
    }

    public CSharpSyntaxNode Generate(ReactAssembly assembly)
    {
      var registrationInvocations = new List<StatementSyntax>();
      var providerMembers = new List<MemberDeclarationSyntax>();

      if (assembly.ViewManagers.Any())
      {
        registrationInvocations.Add(InvocationStatement(ReactNativeNames.CreateViewManagers, IdentifierName(ReactNativeNames.PackageBuilderId)));
        providerMembers.Add(CreateViewManagers(assembly.ViewManagers));
      }

      if (assembly.Modules.Any())
      {
        registrationInvocations.Add(InvocationStatement(ReactNativeNames.CreateModules, IdentifierName((ReactNativeNames.PackageBuilderId))));
        providerMembers.Add(CreateModules(assembly.Modules));
      }

      if (assembly.SerializableTypes.Any())
      {
        registrationInvocations.Add(InvocationStatement(ReactNativeNames.CreateSerializers));
        providerMembers.AddRange(CreateSerializers(assembly.SerializableTypes.Keys));
      }

      if (assembly.JSReaderFunctions.Any())
      {
        registrationInvocations.Add(InvocationStatement(ReactNativeNames.RegisterExtensionReaders));
        providerMembers.Add(RegisterExtensionReaders(assembly.JSReaderFunctions));
      }

      if (assembly.JSWriterFunctions.Any())
      {
        registrationInvocations.Add(InvocationStatement(ReactNativeNames.RegisterExtensionWriter));
        providerMembers.Add(RegisterExtensionWriter(assembly.JSWriterFunctions));
      }

      // Generates:
      //  public void CreatePackage(IPackageBuilder packageBuilder)
      //  {
      //    ... registrationInvocations
      //  }
      var createPackageMethod = MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreatePackageImplementation)
        .AddModifiers(
          Token(SyntaxKind.PartialKeyword))
        .AddParameterListParameters(
          GetPackageBuilderArgument())
        .WithBody(
          Block(
            registrationInvocations
          ));

      // Ensure the main package registration implementation is at the top of the file.
      providerMembers.Insert(0, createPackageMethod);

      // Generates:
      //    namespace "MyNS"
      //    {
      //      public sealed partial ReactPackageProvider
      //      {
      //        ... providerMembers
      //      }
      //    }
      var ns =
        NamespaceDeclaration(ParseName(m_rootNamespace))
          .AddMembers(
            ClassDeclaration("ReactPackageProvider")
              .AddModifiers(
                Token(SyntaxKind.PublicKeyword),
                Token(SyntaxKind.SealedKeyword),
                Token(SyntaxKind.PartialKeyword))
              .WithMembers(
                new SyntaxList<MemberDeclarationSyntax>(providerMembers))
          );

      return ns;
    }
  }
}
