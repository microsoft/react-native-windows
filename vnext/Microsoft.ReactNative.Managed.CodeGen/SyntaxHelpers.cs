// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Roslyn has a set of static and extension methods to help create syntax Trees
  /// This class adds some extra convenience helpers. The method names typically follow the
  /// naming convention used in Roslyn but sets common defaults.
  /// </summary>
  public static class SyntaxHelpers
  {
    private static readonly SymbolDisplayFormat s_symbolFormat = SymbolDisplayFormat.FullyQualifiedFormat;

    public static TypeSyntax ToTypeSyntax(this ISymbol symbol)
    {
      return SyntaxFactory.ParseTypeName(symbol.ToDisplayString(s_symbolFormat));
    }

    internal static ExpressionSyntax LiteralExpression(string value)
    {
      // generates:
      //  "value"
      return SyntaxFactory.LiteralExpression(
        SyntaxKind.StringLiteralExpression,
        SyntaxFactory.Literal(value));
    }

    internal static ExpressionStatementSyntax InvocationStatement(SyntaxToken methodName, params ExpressionSyntax[] arguments)
    {
      return InvocationStatement(SyntaxFactory.IdentifierName(methodName), arguments);
    }

    internal static ExpressionSyntax MemberAccessExpression(SyntaxToken instance, SyntaxToken method)
    {
      return SyntaxFactory.MemberAccessExpression(
        SyntaxKind.SimpleMemberAccessExpression,
        SyntaxFactory.IdentifierName(instance),
        SyntaxFactory.IdentifierName(method));
    }

    internal static ExpressionSyntax MemberAccessExpression(ISymbol type, SyntaxToken method)
    {
      return SyntaxFactory.MemberAccessExpression(
        SyntaxKind.SimpleMemberAccessExpression,
        type.ToTypeSyntax(),
        SyntaxFactory.IdentifierName(method));
    }

    internal static StatementSyntax LocalDeclarationStatement(ISymbol type, SyntaxToken variableName, ExpressionSyntax initializer)
    {
      return SyntaxFactory.LocalDeclarationStatement(
        SyntaxFactory.VariableDeclaration(
           type.ToTypeSyntax())
          .AddVariables(
            SyntaxFactory.VariableDeclarator(variableName)
              .WithInitializer(
                SyntaxFactory.EqualsValueClause(
                  initializer))));
    }

    internal static StatementSyntax LocalDeclarationStatement(SyntaxToken variableName, ExpressionSyntax initializer)
    {
      return SyntaxFactory.LocalDeclarationStatement(
        SyntaxFactory.VariableDeclaration(
            SyntaxFactory.IdentifierName(SyntaxFactory.Identifier("var")))
          .AddVariables(
            SyntaxFactory.VariableDeclarator(variableName)
              .WithInitializer(
                SyntaxFactory.EqualsValueClause(
                  initializer))));
    }

    internal static ExpressionStatementSyntax InvocationStatement(ExpressionSyntax method, params ExpressionSyntax[] arguments)
    {
      return InvocationStatement(method, arguments.Select(arg => SyntaxFactory.Argument(arg)));
    }

    internal static ExpressionStatementSyntax InvocationStatement(ExpressionSyntax method, IEnumerable<ArgumentSyntax> arguments)
    {
      // generates
      //  method(arg1, arg2);
      return SyntaxFactory.ExpressionStatement(InvocationExpression(method, arguments));
    }

    internal static ExpressionSyntax InvocationExpression(ExpressionSyntax method, params ExpressionSyntax[] arguments)
    {
      return InvocationExpression(method, arguments.Select(arg => SyntaxFactory.Argument(arg)));
    }

    internal static ExpressionSyntax InvocationExpression(ExpressionSyntax method, IEnumerable<ArgumentSyntax> arguments)
    {
      // generates
      //  method(arg1, arg2);
      return SyntaxFactory.InvocationExpression(method)
        .WithArgumentList(
          SyntaxFactory.ArgumentList(
            SyntaxFactory.SeparatedList<ArgumentSyntax>(
              arguments)));
    }

    internal static ExpressionSyntax ObjectCreationExpression(ITypeSymbol symbol, params ExpressionSyntax[] arguments)
    {
      return SyntaxFactory
        .ObjectCreationExpression(symbol.ToTypeSyntax())
        .WithArgumentList(
          SyntaxFactory.ArgumentList(
            SyntaxFactory.SeparatedList<ArgumentSyntax>(
              arguments.Select(arg => SyntaxFactory.Argument(arg)))));
    }

    internal static ParameterListSyntax ParameterList(params ParameterSyntax[] parameters)
    {
      return SyntaxFactory.ParameterList(
        SyntaxFactory.SeparatedList<ParameterSyntax>(
          parameters));
    }

    internal static GenericNameSyntax GenericName(SyntaxToken identifier, params TypeSyntax[] types)
    {
      return SyntaxFactory
        .GenericName(identifier)
        .WithTypeArgumentList(
          SyntaxFactory.TypeArgumentList(
            SyntaxFactory.SeparatedList<TypeSyntax>(
              types)));
    }

    internal static Location GetLocation(this ISymbol symbol)
    {
      return symbol.DeclaringSyntaxReferences.FirstOrDefault()?.GetSyntax(CancellationToken.None).GetLocation() ?? Location.None;
    }
  }
}
