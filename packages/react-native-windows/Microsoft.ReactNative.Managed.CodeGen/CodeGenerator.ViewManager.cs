// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Collections.Generic;
using Microsoft.CodeAnalysis;
using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;
using static Microsoft.ReactNative.Managed.CodeGen.SyntaxHelpers;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Code generation for View Managers
  /// </summary>
  public partial class CodeGenerator
  {
    internal MemberDeclarationSyntax CreateViewManagers(IEnumerable<INamedTypeSymbol> viewManagers)
    {
      var registrationCalls = new List<StatementSyntax>();
      foreach (var viewManager in viewManagers)
      {
        // Given "MyApp.MyViewManager"
        // Generates:
        //    packageBuilder.AddViewManager("MyViewManager", () => new global::MyApp.ViewManager);
        registrationCalls.Add(
          InvocationStatement(
            MemberAccessExpression(ReactNativeNames.PackageBuilderId, ReactNativeNames.AddViewManager),
            LiteralExpression(viewManager.Name),
            ParenthesizedLambdaExpression(
              parameterList: ParameterList(),
              block: null,
              expressionBody: ObjectCreationExpression(viewManager))
          )
        );
      }

      // Generates:
      //    internal void CreateViewManager(IPackageBuilder packageBuilder)
      //    {
      //       ... registrationCalls (ses above)
      //    }
      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreateViewManagers)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .AddParameterListParameters(
          GetPackageBuilderArgument())
        .WithBody(
          Block(
            registrationCalls));
    }

    private ParameterSyntax GetPackageBuilderArgument()
    {
      // generates:
      //  IReactPackaBuilder ReactNativeNames.PackageBuilder
      return Parameter(ReactNativeNames.PackageBuilderId)
        .WithType(ReactTypes.IReactPackageBuilder.ToTypeSyntax());
    }
  }
}
