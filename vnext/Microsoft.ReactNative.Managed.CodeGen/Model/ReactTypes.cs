// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
    /// <summary>
    /// Helper class that stores all types needed for analysis and code generation
    /// from Microsoft.ReactNative and Microsoft.ReactNative.Managed
    /// </summary>
    public class ReactTypes
    {
        private const string m_ReactNativeAssemblyName = "Microsoft.ReactNative";
        private const string m_ReactNativeNamespace = "Microsoft.ReactNative";
        private const string m_ReactNativeManagedAssemblyName = "Microsoft.ReactNative.Managed";
        private const string m_ReactNativeManagedNamespace = "Microsoft.ReactNative.Managed";

        public INamedTypeSymbol SystemVoid { get; }
        public INamedTypeSymbol Task { get; }
        public INamedTypeSymbol TaskOfT { get; }

        public INamedTypeSymbol IViewManagerType { get; }
        public INamedTypeSymbol IReactPackageProvider { get; }
        public INamedTypeSymbol IReactPackageBuilder { get; }
        public INamedTypeSymbol IReactModuleBuilder { get; }
        public INamedTypeSymbol IReactContext { get; }
        public INamedTypeSymbol ReactContext { get; }
        public INamedTypeSymbol IJSValueWriter { get; }
        public INamedTypeSymbol IJSValueReader { get; }
        public INamedTypeSymbol JSValueType { get; }
        public INamedTypeSymbol JSValue { get; }
        public INamedTypeSymbol JSValueWriter { get; }
        public INamedTypeSymbol JSValueReader { get; }
        public INamedTypeSymbol JSValueWriterGenerator { get; }
        public INamedTypeSymbol JSValueReaderGenerator { get; }
        public INamedTypeSymbol JSValueWriterCodeGen { get; }
        public INamedTypeSymbol JSValueReaderCodeGen { get; }
        public INamedTypeSymbol IReactPromise { get; }
        public INamedTypeSymbol ReactPromise { get; }
        public INamedTypeSymbol MethodReturnType { get; }
        public INamedTypeSymbol MethodResultCallback { get; }
        public INamedTypeSymbol ReactConstantProvider { get; }
        public INamedTypeSymbol ReactModuleAttribute { get; }
        public INamedTypeSymbol ReactInitializerAttribute { get; }
        public INamedTypeSymbol ReactConstantAttribute { get; }
        public INamedTypeSymbol ReactConstantProviderAttribute { get; }
        public INamedTypeSymbol ReactMethodAttribute { get; }
        public INamedTypeSymbol ReactSyncMethodAttribute { get; }
        public INamedTypeSymbol ReactEventAttribute { get; }
        public INamedTypeSymbol ReactFunctionAttribute { get; }
        public INamedTypeSymbol ReactTaskExtensions { get; }

        private readonly ICollection<Diagnostic> m_diagnostics = new List<Diagnostic>();

        private ReactTypes(Compilation compilation)
        {
            SystemVoid = compilation.GetSpecialType(SpecialType.System_Void);
            Task = FindBclType(compilation, "System.Threading.Tasks.Task");
            TaskOfT = FindBclType(compilation, "System.Threading.Tasks.Task`1")!.ConstructUnboundGenericType();

            IViewManagerType = FindReactNativeType(compilation, "IViewManager");
            IReactPackageProvider = FindReactNativeType(compilation, "IReactPackageProvider");
            IReactPackageBuilder = FindReactNativeType(compilation, "IReactPackageBuilder");
            IReactModuleBuilder = FindReactNativeType(compilation, "IReactModuleBuilder");
            IReactContext = FindReactNativeType(compilation, "IReactContext");
            IJSValueWriter = FindReactNativeType(compilation, "IJSValueWriter");
            IJSValueReader = FindReactNativeType(compilation, "IJSValueReader");
            JSValueType = FindReactNativeType(compilation, "JSValueType");
            MethodReturnType = FindReactNativeType(compilation, "MethodReturnType");
            MethodResultCallback = FindReactNativeType(compilation, "MethodResultCallback");

            ReactContext = FindReactNativeManagedType(compilation, "ReactContext");
            ReactConstantProvider = FindReactNativeManagedType(compilation, "ReactConstantProvider");
            IReactPromise = FindReactNativeManagedType(compilation, "IReactPromise`1");
            ReactPromise = FindReactNativeManagedType(compilation, "ReactPromise`1");
            JSValue = FindReactNativeManagedType(compilation, "JSValue");
            JSValueWriter = FindReactNativeManagedType(compilation, "JSValueWriter");
            JSValueReader = FindReactNativeManagedType(compilation, "JSValueReader");
            JSValueWriterGenerator = FindReactNativeManagedType(compilation, "JSValueWriterGenerator");
            JSValueReaderGenerator = FindReactNativeManagedType(compilation, "JSValueReaderGenerator");
            JSValueWriterCodeGen = FindReactNativeManagedType(compilation, "JSValueWriterCodeGen`1");
            JSValueReaderCodeGen = FindReactNativeManagedType(compilation, "JSValueReaderCodeGen`1");

            ReactModuleAttribute = FindReactNativeManagedType(compilation, "ReactModuleAttribute");
            ReactInitializerAttribute = FindReactNativeManagedType(compilation, "ReactInitializerAttribute");
            ReactConstantAttribute = FindReactNativeManagedType(compilation, "ReactConstantAttribute");
            ReactConstantProviderAttribute = FindReactNativeManagedType(compilation, "ReactConstantProviderAttribute");
            ReactMethodAttribute = FindReactNativeManagedType(compilation, "ReactMethodAttribute");
            ReactSyncMethodAttribute = FindReactNativeManagedType(compilation, "ReactSyncMethodAttribute");
            ReactEventAttribute = FindReactNativeManagedType(compilation, "ReactEventAttribute");
            ReactFunctionAttribute = FindReactNativeManagedType(compilation, "ReactFunctionAttribute");
            ReactTaskExtensions = FindReactNativeManagedType(compilation, "ReactTaskExtensions");
        }

        public static bool TryLoad(Compilation compilation, ICollection<Diagnostic> diagnostics, [NotNullWhen(returnValue: true)] out ReactTypes types)
        {
            types = new ReactTypes(compilation);
            if (types.m_diagnostics.Count > 0)
            {
                foreach (var diagnostic in types.m_diagnostics)
                {
                    diagnostics.Add(diagnostic);
                }

                return false;
            }

            return true;
        }

        private INamedTypeSymbol FindReactNativeType(Compilation compilation, string typeName)
        {
            return FindType(compilation, m_ReactNativeAssemblyName, m_ReactNativeNamespace + "." + typeName);
        }

        private INamedTypeSymbol FindReactNativeManagedType(Compilation compilation, string typeName)
        {
            return FindType(compilation, m_ReactNativeManagedAssemblyName, m_ReactNativeManagedNamespace + "." + typeName);
        }

        private INamedTypeSymbol FindBclType(Compilation compilation, string typeName)
        {
            var type = compilation.GetTypeByMetadataName(typeName);
            if (type == null)
            {
                m_diagnostics.Add(Diagnostic.Create(DiagnosticDescriptors.CantFindReferenceAssembly, Location.None, typeName, "BCL"));
                // Return object to avoid null, the reported error tracks failure.
                return compilation.ObjectType;
            }

            return type;
        }

        private INamedTypeSymbol FindType(Compilation compilation, string assemblyName, string typeName)
        {
            var assemblySymbol = compilation
              .References
              .Select(compilation.GetAssemblyOrModuleSymbol)
              .OfType<IAssemblySymbol>()
              .FirstOrDefault(asm =>
                string.Equals(asm.Name, assemblyName, StringComparison.Ordinal));

            if (assemblySymbol == null)
            {
                m_diagnostics.Add(Diagnostic.Create(DiagnosticDescriptors.CantFindReferenceAssembly, Location.None, assemblyName));
                // Return object to avoid null, the reported error tracks failure.
                return compilation.ObjectType;
            }

            var type = assemblySymbol.GetTypeByMetadataName(typeName);
            if (type == null)
            {
                m_diagnostics.Add(Diagnostic.Create(DiagnosticDescriptors.CantFindReferenceAssembly, Location.None, typeName, assemblySymbol.Identity.GetDisplayName(true)));
                // Return object to avoid null, the reported error tracks failure.
                return compilation.ObjectType;
            }

            return type;
        }
    }
}
