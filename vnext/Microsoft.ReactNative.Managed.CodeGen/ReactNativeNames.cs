// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Naming constants used for analyzing and generating code.
  /// </summary>
  internal class ReactNativeNames
  {
    /// <summary>
    /// The prefix when reporting errors to the user. i.e. RN100
    /// </summary>
    public const string ErrorCodePrefix = "RN";

    /// <summary>
    /// The category used when reporting errors for code generation
    /// </summary>
    public const string ErrorCategory = "CodeGen";

    /// <nodoc />
    public const string DefaultEventEmitterName = "RCTDeviceEventEmitter";

    public static readonly SyntaxToken PackageBuilderId = Identifier("packageBuilder");
    public static readonly SyntaxToken ModuleBuilder = Identifier("moduleBuilder");
    public static readonly SyntaxToken Module = Identifier("module");
    public static readonly SyntaxToken CreatePackage = Identifier("CreatePackage");
    public static readonly SyntaxToken CreatePackageImplementation = Identifier("CreatePackageImplementation");
    public static readonly SyntaxToken AddViewManager = Identifier("AddViewManager");
    public static readonly SyntaxToken AddModule = Identifier("AddModule");

    public static readonly SyntaxToken AddInitializer = Identifier("AddInitializer");
    public static readonly SyntaxToken AddConstantProvider = Identifier("AddConstantProvider");
    public static readonly SyntaxToken AddMethod = Identifier("AddMethod");
    public static readonly SyntaxToken AddSyncMethod = Identifier("AddSyncMethod");

    public static readonly SyntaxToken CreateViewManagers = Identifier("CreateViewManagers");
    public static readonly SyntaxToken CreateModules = Identifier("CreateModules");
    public static readonly SyntaxToken CreateSerializers = Identifier("CreateSerializers");
    public static readonly SyntaxToken CreateEnumSerializers = Identifier("CreateEnumSerializers");
    public static readonly SyntaxToken CreateObjectSerializers = Identifier("CreateObjectSerializers");
    public static readonly SyntaxToken RegisterExtensionReaders = Identifier("RegisterExtensionReaders");
    public static readonly SyntaxToken RegisterExtensionWriter = Identifier("RegisterExtensionWriter");

    public static readonly SyntaxToken WriteObjectBeginMethodName = Identifier("WriteObjectBegin");
    public static readonly SyntaxToken WriteObjectPropertyMethodName = Identifier("WriteObjectProperty");
    public static readonly SyntaxToken WriteObjectEndMethodName = Identifier("WriteObjectEnd");
    public static readonly SyntaxToken WriteNullMethodName = Identifier("WriteNull");
    public static readonly SyntaxToken CallJSFunctionFunctionName = Identifier("CallJSFunction");
    public static readonly SyntaxToken EmitJSEventFunctionName = Identifier("EmitJSEvent");
    public static readonly SyntaxToken WriteArgsMethodName = Identifier("WriteArgs");
    public static readonly SyntaxToken ReadArgsMethodName = Identifier("ReadArgs");
    public static readonly SyntaxToken RegisterAssemblyMethodName = Identifier("RegisterAssembly");
    public static readonly SyntaxToken GetTypeMethodName = Identifier("GetType");
    public static readonly SyntaxToken ReadValueMethodName = Identifier("ReadValue");
    public static readonly SyntaxToken WriteValueMethodName = Identifier("WriteValue");
    public static readonly SyntaxToken ValueTypePropertyName = Identifier("ValueType");
    public static readonly SyntaxToken GetNextObjectPropertyMethodName = Identifier("GetNextObjectProperty");
    public static readonly SyntaxToken ObjectEnumMemberName = Identifier("Object");
    public static readonly SyntaxToken RegisterCodeGeneratorGenericExtensionMethod = Identifier("RegisterCodeGeneratorGenericExtensionMethod");
    public static readonly SyntaxToken WritePropertiesMethodName = Identifier("WriteProperties");

    public static readonly SyntaxToken ConstantsLocalName = Identifier("constants");
    public static readonly SyntaxToken ProviderLocalName = Identifier("provider");
    public static readonly SyntaxToken WriterLocalName = Identifier("writer");
    public static readonly SyntaxToken ReaderLocalName = Identifier("reader");
    public static readonly SyntaxToken ResolveLocalName = Identifier("resolve");
    public static readonly SyntaxToken RejectLocalName = Identifier("reject");
    public static readonly SyntaxToken ReactContextLocalName = Identifier("reactContext");
    public static readonly SyntaxToken PropertyNameLocalName = Identifier("propertyName");
    public static readonly SyntaxToken ResultLocalName = Identifier("result");
    public static readonly SyntaxToken ValueLocalName = Identifier("value");
    public static readonly SyntaxToken AssemblyLocalName = Identifier("assembly");
    public static readonly SyntaxToken AssemblyPropertyName = Identifier("Assembly");

    public static readonly SyntaxToken ContinueWith = Identifier("ContinueWith");
    public static readonly SyntaxToken TaskLocalName = Identifier("task");

    public static readonly SyntaxToken Arg0LocalName = Identifier("arg0");
    public static readonly SyntaxToken Arg1LocalName = Identifier("arg1");
    public static readonly SyntaxToken Arg2LocalName = Identifier("arg2");
    public static readonly SyntaxToken Arg3LocalName = Identifier("arg3");
    public static readonly SyntaxToken Arg4LocalName = Identifier("arg4");
    public static readonly SyntaxToken Arg5LocalName = Identifier("arg5");
    public static readonly SyntaxToken Arg6LocalName = Identifier("arg6");

    public static readonly SyntaxToken[] ArgLocalNames = new SyntaxToken[] {
      Arg0LocalName, Arg1LocalName, Arg2LocalName, Arg3LocalName,
      Arg4LocalName, Arg5LocalName, Arg6LocalName,
    };

    public static readonly SyntaxToken Value0LocalName = Identifier("value0");
    public static readonly SyntaxToken Value1LocalName = Identifier("value1");
    public static readonly SyntaxToken Value2LocalName = Identifier("value2");
    public static readonly SyntaxToken Value3LocalName = Identifier("value3");
    public static readonly SyntaxToken Value4LocalName = Identifier("value4");
    public static readonly SyntaxToken Value5LocalName = Identifier("value5");
    public static readonly SyntaxToken Value6LocalName = Identifier("value6");

    public static readonly SyntaxToken[] ValueLocalNames = new SyntaxToken[] {
      Value0LocalName, Value1LocalName, Value2LocalName, Value3LocalName,
      Value4LocalName, Value5LocalName, Value6LocalName,
    };
  }
}
