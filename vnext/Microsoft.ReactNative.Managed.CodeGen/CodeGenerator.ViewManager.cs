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

    private ParameterSyntax GetPackageBuilderArgument()
    {
      // generates:
      //  IReactPackageBuilder ReactNativeNames.PackageBuilder
      return Parameter(ReactNativeNames.PackageBuilderId)
        .WithType(ReactTypes.IReactPackageBuilder.ToTypeSyntax());
    }
  }
}
