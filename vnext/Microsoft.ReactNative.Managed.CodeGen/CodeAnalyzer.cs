// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics.CodeAnalysis;
using System.Diagnostics.ContractsLight;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;

namespace Microsoft.ReactNative.Managed.CodeGen
{
    /// <summary>
    /// Primary class 
    /// </summary>
    public class CodeAnalyzer
    {
        public int ConcurrencyLevel { get; set; } = DataflowBlockOptions.Unbounded;

        private readonly CancellationToken m_cancellationToken;

        private readonly ConcurrentBag<SyntaxTree> m_syntaxTrees = new ConcurrentBag<SyntaxTree>();
        private readonly ConcurrentBag<MetadataReference> m_metadataReferences = new ConcurrentBag<MetadataReference>();

        private Compilation? m_compilation { get; set; }

        private ReactTypes? m_reactTypes;
        internal ReactTypes ReactTypes
        {
            get
            {
                Contract.Assert(m_reactTypes != null);
                return m_reactTypes;
            }
        }


        private readonly List<Diagnostic> m_diagnostics = new List<Diagnostic>();
        public IEnumerable<Diagnostic> Diagnostics => m_diagnostics;

        public CodeAnalyzer(CancellationToken cancellationToken)
        {
            m_cancellationToken = cancellationToken;
        }

        internal void AddSyntaxTree(SyntaxTree syntaxTree)
        {
            m_syntaxTrees.Add(syntaxTree);
        }

        public async Task ParseSourceFilesAsync(IEnumerable<string> sourceFiles, IEnumerable<string>? preprocessorSymbols)
        {
            var csharpParseOptions = new CSharpParseOptions(
              preprocessorSymbols: preprocessorSymbols,
              languageVersion: LanguageVersion.Latest);

            var block = new ActionBlock<string>(async file =>
            {
                string text = await File.ReadAllTextAsync(file, m_cancellationToken);
                AddSyntaxTree(CSharpSyntaxTree.ParseText(text, path: file, options: csharpParseOptions));
            },
            new ExecutionDataflowBlockOptions()
            {
                MaxDegreeOfParallelism = ConcurrencyLevel,
                CancellationToken = m_cancellationToken,
            });
            foreach (var sourceFile in sourceFiles)
            {
                block.Post(sourceFile);
            }

            block.Complete();
            await block.Completion;
        }

        public async Task LoadMetadataReferencesAsync(IEnumerable<string> references)
        {
            var block = new ActionBlock<string>(file =>
              {
                  m_metadataReferences.Add(MetadataReference.CreateFromFile(file));
              },
              new ExecutionDataflowBlockOptions()
              {
                  MaxDegreeOfParallelism = ConcurrencyLevel,
                  CancellationToken = m_cancellationToken,
              });
            foreach (var reference in references.Distinct())
            {
                block.Post(reference);
            }

            block.Complete();
            await block.Completion;
        }

        public bool TryCompileAndCheckForErrors()
        {
            Contract.Assert(m_syntaxTrees.Any(), "Expected to have syntax trees loaded");
            Contract.Assert(m_metadataReferences.Any(), "Expected to have references loaded");

            m_compilation = CSharpCompilation.Create(
              "temp",
              m_syntaxTrees,
              m_metadataReferences,
              new CSharpCompilationOptions(OutputKind.WindowsRuntimeMetadata)
            );

            Contract.Assert(m_compilation != null, "Expect Microsoft.Net.Compilers to always return a compilation unit.");

            m_diagnostics.AddRange(m_compilation
              .GetDiagnostics(m_cancellationToken)
              .Where(diagnostic => diagnostic.Severity == DiagnosticSeverity.Error)
              );
            if (m_diagnostics.Count > 0)
            {
                return false;
            }

            return ReactTypes.TryLoad(m_compilation, m_diagnostics, out m_reactTypes);
        }

        internal IEnumerable<INamedTypeSymbol> GetAllTypes()
        {
            Contract.Assert(m_compilation != null, "Expected to have a compilation. Must call CompileAndCheckForErrors first");

            var allTypes = new List<INamedTypeSymbol>();
            CollectTypes(m_compilation.Assembly.GlobalNamespace, allTypes);
            return allTypes;
        }

        public ReactAssembly AnalyzeAndConstructAssemblyModel()
        {
            Contract.Assert(m_compilation != null, "Expected to have a compilation. Must call CompileAndCheckForErrors first");

            var assembly = new ReactAssembly();
            var proceseedTypes = new HashSet<ITypeSymbol>();

            foreach (var type in GetAllTypes())
            {
                if (TryExtractModule(type, out var module))
                {
                    assembly.Modules.Add(module);
                }

                if (IsViewManager(type))
                {
                    assembly.ViewManagers.Add(type);
                }

                // Proactively register serializable types.
                switch (type.TypeKind)
                {
                    case TypeKind.Enum:
                        assembly.SerializableTypes.Add(type, type.TypeKind);
                        break;
                }

                // Register extension methods
                if (type.MightContainExtensionMethods)
                {
                    foreach (var member in type.GetMembers())
                    {
                        if (member is IMethodSymbol method)
                        {
                            if (method.IsExtensionMethod &&
                                method.Parameters.Length == 2 &&
                                !(method.Parameters[1].Type is ITypeParameterSymbol))
                            {
                                if (method.Parameters[0].Type.Equals(ReactTypes.JSValueReader, SymbolEqualityComparer.Default) &&
                                    method.Parameters[1].RefKind == RefKind.Out)
                                {
                                    assembly.JSReaderFunctions.Add(method.Parameters[1].Type, method);
                                }
                                else if (method.Parameters[0].Type.Equals(ReactTypes.JSValueWriter, SymbolEqualityComparer.Default))
                                {
                                    assembly.JSWriterFunctions.Add(method.Parameters[1].Type, method);
                                }
                            }
                        }
                    }
                }
            }

            foreach (var module in assembly.Modules)
            {
                foreach (var method in module.Methods)
                {
                    AddSerializableMethod(method.Method);
                }

                foreach (var constant in module.Constants)
                {
                    if (constant.Symbol is IFieldSymbol field)
                    {
                        AddSerializableType(field.Type);
                    }
                    else if (constant.Symbol is IPropertySymbol property)
                    {
                        AddSerializableType(property.Type);
                    }
                }

                foreach (var callback in module.Events.Union<ReactCallback>(module.Functions))
                {
                    foreach (var param in callback.CallbackParameters)
                    {
                        AddSerializableType(param.Type);
                    }
                }
            }

            void AddSerializableMethod(IMethodSymbol method)
            {
                AddSerializableType(method.ReturnType);
                foreach (var param in method.Parameters)
                {
                    AddSerializableType(param.Type);
                }
            }

            void AddSerializableType(ITypeSymbol type)
            {
                if (proceseedTypes.Contains(type))
                {
                    return;
                }

                proceseedTypes.Add(type);

                if (type is INamedTypeSymbol namedType)
                {
                    if (type.TypeKind == TypeKind.Delegate)
                    {
                        Contract.Assert(namedType.DelegateInvokeMethod != null);

                        AddSerializableMethod(namedType.DelegateInvokeMethod);
                    }
                    else if (type.ContainingAssembly.Equals(m_compilation.Assembly, SymbolEqualityComparer.Default))
                    {
                        if (namedType.TypeKind == TypeKind.Class)
                        {
                            // Check that classes have a default constructor.
                            if (!namedType
                              .GetMembers()
                              .Any(member =>
                                member is IMethodSymbol method &&
                                method.MethodKind == MethodKind.Constructor &&
                                method.Parameters.Length == 0))
                            {

                            }
                        }

                        assembly.SerializableTypes.Add(namedType, namedType.TypeKind);
                    }
                }
            }

            return assembly;
        }

        internal bool IsViewManager(INamedTypeSymbol type)
        {
            return !type.IsAbstract && type.AllInterfaces.Any(iface => iface.Equals(ReactTypes.IViewManagerType, SymbolEqualityComparer.Default));
        }

        internal bool TryExtractModule(INamedTypeSymbol type, [NotNullWhen(returnValue: true)] out ReactModule? module)
        {
            if (TryFindAttribute(type, ReactTypes.ReactModuleAttribute, out var attr))
            {
                string? moduleName = null;
                string? eventEmitterName = null;

                if (attr.ConstructorArguments.Length > 0)
                {
                    moduleName = attr.ConstructorArguments[0].Value as string;
                }

                foreach (var namedArgument in attr.NamedArguments)
                {
                    switch (namedArgument.Key)
                    {
                        case nameof(ReactModuleAttribute.ModuleName):
                            moduleName = namedArgument.Value.Value as string;
                            break;
                        case nameof(ReactModuleAttribute.EventEmitterName):
                            eventEmitterName = namedArgument.Value.Value as string;
                            break;
                        default:
                            var location = attr.ApplicationSyntaxReference?.SyntaxTree.GetLocation(attr.ApplicationSyntaxReference.Span);
                            m_diagnostics.Add(Diagnostic.Create(DiagnosticDescriptors.UnexpectedPropertyInAttribute, location ?? Location.None, namedArgument.Key, attr.AttributeClass?.Name));
                            module = null;
                            return false;
                    }
                }

                moduleName ??= type.Name;
                eventEmitterName ??= ReactNativeNames.DefaultEventEmitterName;

                module = new ReactModule(type, moduleName, eventEmitterName);

                foreach (var member in type.GetMembers())
                {
                    if (member is IMethodSymbol method)
                    {
                        if (TryExtractMethod(method, out var asyncMethod))
                        {
                            module.Methods.Add(asyncMethod);
                        }
                        else if (TryExtractSyncMethod(method, out var syncMethod))
                        {
                            module.Methods.Add(syncMethod);
                        }
                        else if (TryExtractInitializer(method, out var initializer))
                        {
                            module.Initializers.Add(initializer);
                        }
                        else if (TryExtractConstantProvider(method, out var constantProvider))
                        {
                            module.ConstantProviders.Add(constantProvider);
                        }
                    }
                    else if (member is IPropertySymbol || member is IFieldSymbol)
                    {
                        if (TryExtractConstant(member, out var constant))
                        {
                            module.Constants.Add(constant);
                        }
                        else if (TryExtractEvent(member, eventEmitterName, out var evnt))
                        {
                            module.Events.Add(evnt);
                        }
                        else if (TryExtractFunction(member, moduleName, out var function))
                        {
                            module.Functions.Add(function);
                        }
                    }
                }

                return true;
            }

            module = null;
            return false;
        }

        private bool TryExtractMethod(IMethodSymbol method, [NotNullWhen(returnValue: true)] out ReactMethod? reactMethod)
        {
            return TryExtractMethodFromAttributeData(method, ReactTypes.ReactMethodAttribute, synchronous: false, out reactMethod);
        }

        private bool TryExtractSyncMethod(IMethodSymbol method, [NotNullWhen(returnValue: true)] out ReactMethod? reactMethod)
        {
            return TryExtractMethodFromAttributeData(method, ReactTypes.ReactSyncMethodAttribute, synchronous: true, out reactMethod);
        }

        private bool TryExtractMethodFromAttributeData(
          IMethodSymbol method,
          INamedTypeSymbol attributeType,
          bool synchronous,
          [NotNullWhen(returnValue: true)] out ReactMethod? reactMethod)
        {
            Contract.Requires(m_reactTypes != null);

            if (TryFindAttribute(method, attributeType, out var attr))
            {
                string? name = null;
                if (attr.ConstructorArguments.Length > 0)
                {
                    name = attr.ConstructorArguments[0].Value as string;
                }

                foreach (var namedArgument in attr.NamedArguments)
                {
                    switch (namedArgument.Key)
                    {
                        case nameof(ReactMethodAttribute.MethodName):
                            name = namedArgument.Value.Value as string;
                            break;
                        default:
                            var location = attr.ApplicationSyntaxReference?.SyntaxTree.GetLocation(attr.ApplicationSyntaxReference.Span);
                            m_diagnostics.Add(Diagnostic.Create(
                              DiagnosticDescriptors.UnexpectedPropertyInAttribute,
                              location ?? Location.None,
                              namedArgument.Key,
                              attr.AttributeClass?.Name));
                            reactMethod = null;
                            return false;
                    }
                }

                ReactMethod.MethodReturnStyle? returnStyle = null;

                // Compute the effective parameters
                var effectiveParameters = new List<IParameterSymbol>();
                ITypeSymbol effectiveReturnType = m_reactTypes.SystemVoid;
                var parameters = method.Parameters;
                for (int i = 0; i < parameters.Length; i++)
                {
                    var param = parameters[i];
                    bool skipReadingArg = false;

                    if (!synchronous) // IsSynchronous methods don't have support for the callbacks
                    {
                        if (i == parameters.Length - 1)
                        {
                            if (IsPromiseType(parameters[i].Type, out var promiseGenericType))
                            {
                                returnStyle = ReactMethod.MethodReturnStyle.Promise;
                                effectiveReturnType = promiseGenericType;
                                skipReadingArg = true;
                            }

                            // if the last argument is a callback, assume this is the resolve or resolve function
                            if (IsSingleArgCallback(parameters[i].Type, out var currentCallBackType))
                            {
                                ITypeSymbol? previousCallBackType = null;
                                var isReject = parameters.Length >= 2 &&
                                               IsSingleArgCallback(parameters[i - 1].Type, out previousCallBackType);
                                effectiveReturnType = isReject
                                  ? previousCallBackType!
                                  : currentCallBackType!;
                                returnStyle = isReject
                                  ? ReactMethod.MethodReturnStyle.TwoCallbacks
                                  : ReactMethod.MethodReturnStyle.Callback;

                                skipReadingArg = true;
                            }
                        }
                        else if (i == parameters.Length - 2)
                        {
                            // if the last 2 argument are callbacks, assume this is the resolve function
                            if (IsSingleArgCallback(parameters[i].Type, out var resolveCallBackType) &&
                                IsSingleArgCallback(parameters[i + 1].Type, out _))
                            {
                                effectiveReturnType = resolveCallBackType;
                                returnStyle = ReactMethod.MethodReturnStyle.TwoCallbacks;
                                skipReadingArg = true;
                            }
                        }
                    }

                    if (!skipReadingArg)
                    {
                        effectiveParameters.Add(param);
                    }
                }

                if (returnStyle == null)
                {
                    if (IsTaskType(method.ReturnType, out var taskReturnType))
                    {
                        if (synchronous)
                        {
                            m_diagnostics.Add(Diagnostic.Create(
                              DiagnosticDescriptors.MethodShouldNotReturnTaskWhenSynchronous,
                              method.GetLocation() ?? Location.None,
                              method.Name,
                              attr.AttributeClass?.Name));
                            reactMethod = null;
                            return false;
                        }

                        effectiveReturnType = taskReturnType;
                        returnStyle = ReactMethod.MethodReturnStyle.Task;
                    }
                    else if (method.ReturnsVoid)
                    {
                        returnStyle = ReactMethod.MethodReturnStyle.Void;
                    }
                    else
                    {
                        returnStyle = ReactMethod.MethodReturnStyle.ReturnValue;
                    }
                }

                name ??= method.Name;

                reactMethod = new ReactMethod(method, name, returnStyle.Value, effectiveReturnType, effectiveParameters, synchronous);
                return true;
            }

            reactMethod = null;
            return false;
        }

        private bool TryExtractConstantProvider(IMethodSymbol method, [NotNullWhen(returnValue: true)] out ReactConstantProvider? constantProvider)
        {
            constantProvider = null;
            if (TryFindAttribute(method, ReactTypes.ReactConstantProviderAttribute, out _))
            {
                if (!IsAccessible(method))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.ConstantProviderMustBePublicOrInternal,
                        method.GetLocation(),
                        method.ReturnType.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.DeclaredAccessibility
                      ));
                    return false;
                }

                if (!method.ReturnsVoid)
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.ConstantProviderMustReturnVoid,
                        method.GetLocation(),
                        method.ReturnType.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat)
                      ));
                    return false;
                }

                if (method.Parameters.Length != 1 || !method.Parameters[0].Type.Equals(ReactTypes.ReactConstantProvider, SymbolEqualityComparer.Default))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.ConstantProviderMustHaveOneParameterOfTypeReactContext,
                        method.GetLocation(),
                        ReactTypes.ReactConstantProvider.ToDisplayParts(SymbolDisplayFormat.FullyQualifiedFormat),
                        method.Parameters.Length.ToString(CultureInfo.InvariantCulture),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat)
                      ));
                    return false;
                }

                constantProvider = new ReactConstantProvider(method);
                return true;
            }

            constantProvider = null;
            return false;
        }

        private bool TryExtractInitializer(IMethodSymbol method, [NotNullWhen(returnValue: true)] out ReactInitializer? initializer)
        {
            initializer = null;
            if (TryFindAttribute(method, ReactTypes.ReactInitializerAttribute, out _))
            {
                if (!IsAccessible(method))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.InitializersMustBePublicOrInternal,
                        method.GetLocation(),
                        method.ReturnType.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.DeclaredAccessibility
                      ));
                    return false;
                }

                if (!method.ReturnsVoid)
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.InitializersMustReturnVoid,
                        method.GetLocation(),
                        method.ReturnType.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat)
                        ));
                    return false;
                }

                if (method.Parameters.Length != 1 || !method.Parameters[0].Type.Equals(ReactTypes.ReactContext, SymbolEqualityComparer.Default))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.InitializersMustHaveOneParameterOfTypeReactContext,
                        method.GetLocation(),
                        ReactTypes.ReactContext.ToDisplayParts(SymbolDisplayFormat.FullyQualifiedFormat),
                        method.Parameters.Length.ToString(CultureInfo.InvariantCulture),
                        method.Name,
                        method.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat)
                      ));
                    return false;
                }

                initializer = new ReactInitializer(method);
                return true;
            }

            return false;
        }

        private bool TryExtractConstant(ISymbol symbol, [NotNullWhen(returnValue: true)] out ReactConstant? constant)
        {
            if (TryFindAttribute(symbol, ReactTypes.ReactConstantAttribute, out var attr))
            {
                string? name = null;
                if (attr.ConstructorArguments.Length > 0)
                {
                    name = attr.ConstructorArguments[0].Value as string;
                }

                foreach (var namedArgument in attr.NamedArguments)
                {
                    switch (namedArgument.Key)
                    {
                        case nameof(ReactConstantAttribute.ConstantName):
                            name = namedArgument.Value.Value as string;
                            break;
                        default:
                            var location = attr.ApplicationSyntaxReference?.SyntaxTree.GetLocation(attr.ApplicationSyntaxReference.Span);
                            m_diagnostics.Add(Diagnostic.Create(
                              DiagnosticDescriptors.UnexpectedPropertyInAttribute,
                              location ?? Location.None,
                              namedArgument.Key,
                              attr.AttributeClass?.Name));
                            constant = null;
                            return false;
                    }
                }

                name ??= symbol.Name;

                if (!IsAccessible(symbol))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.ConstantsMustBePublicOrInternal,
                        symbol.GetLocation(),
                        symbol.Name,
                        symbol.ContainingSymbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        symbol.DeclaredAccessibility
                      ));
                    constant = null;
                    return false;
                }

                if (symbol is IPropertySymbol property && (property.GetMethod == null || !IsAccessible(property.GetMethod)))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.ConstantsMustBeGettable,
                        symbol.GetLocation(),
                        property.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        symbol.ContainingType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat)));
                    constant = null;
                    return false;
                }

                constant = new ReactConstant(symbol, name);
                return true;
            }

            constant = null;
            return false;
        }

        private bool TryExtractEvent(ISymbol symbol, string defaultEventEmitterName, [NotNullWhen(returnValue: true)] out ReactEvent? reactEvent)
        {
            if (TryExtractCallback(symbol, defaultEventEmitterName, ReactTypes.ReactEventAttribute, out var callbackParameters, out var name, out var declaredModuleName))
            {
                reactEvent = new ReactEvent(symbol, callbackParameters, name, declaredModuleName);
                return true;
            }

            reactEvent = null;
            return false;
        }

        private bool TryExtractFunction(ISymbol symbol, string moduleName, [NotNullWhen(returnValue: true)] out ReactFunction? function)
        {
            if (TryExtractCallback(symbol, moduleName, ReactTypes.ReactFunctionAttribute, out var callbackParameters, out var name, out var declaredModuleName))
            {
                function = new ReactFunction(symbol, callbackParameters, name, declaredModuleName);
                return true;
            }

            function = null;
            return false;
        }

        private bool TryExtractCallback(
          ISymbol symbol,
          string defaultCallbackContextName,
          INamedTypeSymbol attribute,
          out ImmutableArray<IParameterSymbol> callbackParameters,
          [NotNullWhen(returnValue: true)] out string? name,
          [NotNullWhen(returnValue: true)] out string? callbackContextName)
        {
            callbackParameters = default;
            name = null;
            callbackContextName = null;

            if (TryFindAttribute(symbol, attribute, out var attr))
            {
                name = null;
                callbackContextName = null;
                if (attr.ConstructorArguments.Length > 0)
                {
                    name = attr.ConstructorArguments[0].Value as string;
                }

                foreach (var namedArgument in attr.NamedArguments)
                {
                    switch (namedArgument.Key)
                    {
                        case nameof(ReactFunctionAttribute.FunctionName):
                        case nameof(ReactEventAttribute.EventName):
                            name = namedArgument.Value.Value as string;
                            break;
                        case nameof(ReactFunctionAttribute.ModuleName):
                        case nameof(ReactEventAttribute.EventEmitterName):
                            callbackContextName = namedArgument.Value.Value as string;
                            break;
                        default:
                            var location = attr.ApplicationSyntaxReference?.SyntaxTree.GetLocation(attr.ApplicationSyntaxReference.Span);
                            m_diagnostics.Add(Diagnostic.Create(
                              DiagnosticDescriptors.UnexpectedPropertyInAttribute,
                              location ?? Location.None,
                              namedArgument.Key,
                              attr.AttributeClass?.Name));
                            return false;
                    }
                }

                ITypeSymbol? type = null;
                switch (symbol)
                {
                    case IPropertySymbol property:
                        type = property.Type;

                        if (property.SetMethod == null || !IsAccessible(property.SetMethod))
                        {
                            m_diagnostics.Add(
                              Diagnostic.Create(DiagnosticDescriptors.CallbacksMustBeSettable,
                                symbol.GetLocation(),
                                property.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                                symbol.ContainingType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat)));
                            return false;
                        }

                        break;
                    case IFieldSymbol field:
                        type = field.Type;
                        break;
                    default:
                        throw Contract.AssertFailure("Unsupported symbol");
                }

                if (!IsAccessible(symbol))
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.CallbackMustBePublicOrInternal,
                        symbol.GetLocation(),
                        symbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        symbol.ContainingType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat),
                        symbol.DeclaredAccessibility));
                    return false;
                }

                if (!(type is INamedTypeSymbol namedType) || namedType.DelegateInvokeMethod == null)
                {
                    m_diagnostics.Add(
                      Diagnostic.Create(DiagnosticDescriptors.CallbackTypeMustBeDelegate,
                        symbol.GetLocation(),
                        type.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                        symbol.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat)));
                    return false;
                }

                if (!namedType.DelegateInvokeMethod.ReturnsVoid)
                {
                    var location = symbol.Locations.FirstOrDefault();
                    m_diagnostics.Add(Diagnostic.Create(DiagnosticDescriptors.CallbackDelegateMustReturnVoid, location,
                      symbol.ToDisplayString(SymbolDisplayFormat.CSharpErrorMessageFormat),
                      symbol.ContainingType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat),
                      namedType.DelegateInvokeMethod.ReturnType.ToDisplayString(SymbolDisplayFormat.CSharpShortErrorMessageFormat)));
                    return false;
                }

                name ??= symbol.Name;
                callbackContextName ??= defaultCallbackContextName;
                callbackParameters = namedType.DelegateInvokeMethod.Parameters;

                return true;
            }

            return false;
        }

        private static void CollectTypes(INamespaceSymbol namespaceSymbol, ICollection<INamedTypeSymbol> result)
        {
            foreach (var type in namespaceSymbol.GetTypeMembers())
            {
                CollectTypes(type, result);
            }

            foreach (var childNs in namespaceSymbol.GetNamespaceMembers())
            {
                CollectTypes(childNs, result);
            }
        }

        private static void CollectTypes(INamedTypeSymbol type, ICollection<INamedTypeSymbol> result)
        {
            result.Add(type);

            foreach (var nested in type.GetTypeMembers())
            {
                CollectTypes(nested, result);
            }
        }

        private bool TryFindAttribute(ISymbol symbol, INamedTypeSymbol attributeType, out AttributeData attr)
        {
            attr = symbol.GetAttributes().FirstOrDefault(potentialMatch => potentialMatch.AttributeClass != null && potentialMatch.AttributeClass.Equals(attributeType, SymbolEqualityComparer.Default));
            return attr != null;
        }

        private bool IsAccessible(ISymbol symbol)
        {
            return symbol.DeclaredAccessibility == Accessibility.Public ||
                   symbol.DeclaredAccessibility == Accessibility.Internal;
        }

        private bool IsPromiseType(ITypeSymbol type, [NotNullWhen(returnValue: true)] out ITypeSymbol? typeParameter)
        {
            Contract.Requires(m_reactTypes != null);

            if (type != null &&
                type is INamedTypeSymbol namedType &&
                namedType.IsGenericType &&
                (
                   namedType.ConstructUnboundGenericType().Equals(
                       m_reactTypes.IReactPromise.ConstructUnboundGenericType(), SymbolEqualityComparer.Default) ||
                    namedType.ConstructUnboundGenericType().Equals(
                       m_reactTypes.ReactPromise.ConstructUnboundGenericType(), SymbolEqualityComparer.Default)
                ))
            {
                typeParameter = namedType.TypeArguments[0];
                return true;
            }

            typeParameter = null;
            return false;
        }

        private bool IsTaskType(ITypeSymbol type, [NotNullWhen(returnValue: true)] out ITypeSymbol? taskReturnType)
        {
            Contract.Requires(m_reactTypes != null);

            if (type.Equals(m_reactTypes.Task, SymbolEqualityComparer.Default))
            {
                taskReturnType = m_reactTypes.SystemVoid;
                return true;
            }

            if (type != null &&
                type is INamedTypeSymbol namedType &&
                namedType.IsGenericType &&
                namedType.ConstructUnboundGenericType()
                  .Equals(m_reactTypes.TaskOfT, SymbolEqualityComparer.Default))
            {
                taskReturnType = namedType.TypeArguments[0];
                return true;
            }

            taskReturnType = null;
            return false;
        }

        private static bool IsSingleArgCallback(ITypeSymbol type, [NotNullWhen(returnValue: true)] out ITypeSymbol? parameterType)
        {
            if (type is INamedTypeSymbol namedType &&
                namedType.DelegateInvokeMethod != null &&
                namedType.DelegateInvokeMethod.Parameters.Length == 1
            )
            {
                parameterType = namedType.DelegateInvokeMethod.Parameters[0].Type;
                return true;
            }

            parameterType = null;
            return false;
        }
    }
}
