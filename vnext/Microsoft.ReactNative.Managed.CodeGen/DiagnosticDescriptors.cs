// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using static System.FormattableString;

#pragma warning disable RS1017 // DiagnosticId for analyzers must be a non-null constant.

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Error descriptors to be used when reporting errors.
  /// This is a pattern that the Roslyn diagnostics system uses. All reported errors use this infrastructure
  /// to leverage their standardized error formats and location handling.
  /// </summary>
  internal class DiagnosticDescriptors
  {
    public static readonly DiagnosticDescriptor CantFindReferenceAssembly = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1000"),
      category: ReactNativeNames.ErrorCategory,
      title: "Can't find assembly",
      messageFormat: "Can't find reference assembly '{0}'. Please ensure you have a reference to it.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor CantFindTypeInAssembly = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1001"),
      category: ReactNativeNames.ErrorCategory,
      title: "Can't find type in assembly",
      messageFormat: "Can't find type '{0}' in assembly '{1}'. Please ensure you have a reference to the correct version.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor UnexpectedPropertyInAttribute = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1002"),
      category: ReactNativeNames.ErrorCategory,
      title: "Unexpected attribute arguments",
      messageFormat: "Unexpected named attribute '{0}' encountered on attribute '{1}'. Did you mix versions of React Native Windows and the Sdk?",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor CallbackTypeMustBeDelegate = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1003"),
      category: ReactNativeNames.ErrorCategory,
      title: "Unexpected event type",
      messageFormat: "React event types must be a delegate. Encountered type '{0}' for event '{1}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor SerializableTypesMustHaveEmptyConstructor = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1003"),
      category: ReactNativeNames.ErrorCategory,
      title: "Cannot serialie type",
      messageFormat: "Types that are serialized for React bust have a default parameter-less constructor.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor InitializersMustReturnVoid = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1004"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Initializers must return void",
      messageFormat: "Initializers of ReactModules must return void. Encountered type '{0}' as return type of initializer '{1}' of module '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor InitializersMustHaveOneParameterOfTypeReactContext = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1005"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Initializers must have one parameter of type ReactContext",
      messageFormat: "Initializers of ReactModules must have one parameter of type '{0}'. Encountered '{1}' arguments on initializer '{2}' of module '{3}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor InitializersMustBePublicOrInternal = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1006"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Initializers must be public or internal",
      messageFormat: "Initializers of ReactModules must be public or internal. Visibility of initializer '{0}' of module '{1}' is '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor ConstantsMustBePublicOrInternal = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1007"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Constants must be public or internal",
      messageFormat: "Constants of ReactModules must be public or internal. Visibility of constant '{0}' of module '{1}' is '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor CallbackMustBePublicOrInternal = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1008"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Events and Functions must be public or internal",
      messageFormat: "Events and Functions of ReactModules must be public or internal. Visibility of constant '{0}' of module '{1}' is '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor CallbacksMustBeSettable = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1009"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Events and Functions must be settable",
      messageFormat: "Events and Functions of ReactModules must be settable. Ensure event '{0}' of module '{1}' has an accessible setter..",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor ConstantsMustBeGettable = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1010"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Constants must be gettable",
      messageFormat: "Constants of ReactModules must be gettable. Ensure event '{0}' of module '{1}' has an accessible getter..",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor ConstantProviderMustReturnVoid = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1011"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule ConstantProviders must return void",
      messageFormat: "ConstantProviders of ReactModules must return void. Encountered type '{0}' as return type of constantProvider '{1}' of module '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor ConstantProviderMustHaveOneParameterOfTypeReactContext = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1012"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule ConstantProviders must have one parameter of type ReactContext",
      messageFormat: "ConstantProviders of ReactModules must have one parameter of type '{0}'. Encountered '{1}' arguments on constantProvider '{2}' of module '{3}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor ConstantProviderMustBePublicOrInternal = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1013"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule ConstantProviders must be public or internal",
      messageFormat: "ConstantProviders of ReactModules must be public or internal. Visibility of constantProviders '{0}' of module '{1}' is '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor CallbackDelegateMustReturnVoid = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1014"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Events and Functions must have a void delegate",
      messageFormat: "Events and Functions of ReactModules must be a delegate that returns void. Return type of delegate for callback '{0}' of module '{1}' is '{2}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);

    public static readonly DiagnosticDescriptor MethodShouldNotReturnTaskWhenSynchronous = new DiagnosticDescriptor(
      id: Invariant($"{ReactNativeNames.ErrorCodePrefix}1015"),
      category: ReactNativeNames.ErrorCategory,
      title: "ReactModule Methods should not return ",
      messageFormat: "Methods of ReactModules should not return a Task when they are synchronous. Change the attribute from '[ReactSyncMethod]' to '[ReactMethod]' for method '{0}' of module '{1}'.",
      defaultSeverity: DiagnosticSeverity.Error,
      isEnabledByDefault: true);
  };
}
