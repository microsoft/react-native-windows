// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// This project is unfortunately restricted to older versions of dotnet core due to consumers of our libraries not able ot upgrade
// This file contains all the types that would be available in later versions and can be removed when we upgrade.

#region Available In netcore30 and up.

namespace System.Diagnostics.CodeAnalysis
{
  /// <summary>Specifies that when a method returns <see cref="ReturnValue"/>, the parameter will not be null even if the corresponding type allows it.</summary>
  [System.AttributeUsage(System.AttributeTargets.Parameter, Inherited = false)]
  internal sealed class NotNullWhenAttribute : Attribute
  {
    /// <summary>
    /// Initializes a new instance of the <see cref="NotNullWhenAttribute"/> class.
    /// </summary>
    /// <param name="returnValue">
    /// The return value condition. If the method returns this value, the associated parameter will not be null.
    /// </param>
    public NotNullWhenAttribute(bool returnValue) => ReturnValue = returnValue;

    /// <summary>Gets the return value condition.</summary>
    public bool ReturnValue { get; }
  }
}

#endregion
