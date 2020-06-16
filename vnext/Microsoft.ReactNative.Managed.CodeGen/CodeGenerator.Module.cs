// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Globalization;
using System.Linq;
using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;
using static Microsoft.ReactNative.Managed.CodeGen.SyntaxHelpers;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  public partial class CodeGenerator
  {
    internal MemberDeclarationSyntax CreateModules(IEnumerable<ReactModule> modules)
    {
      var addModuleStatements = new List<StatementSyntax>();
      foreach (var module in modules)
      {
        var addMemberStatements = new List<StatementSyntax>();

        // generates:
        //  MyModule module = new MyModule();
        addMemberStatements.Add(
          LocalDeclarationStatement(module.ModuleType, ReactNativeNames.Module, ObjectCreationExpression(module.ModuleType)));

        if (module.Constants.Any())
        {
          addMemberStatements.Add(AddConstants(module.Constants));
        }

        foreach (var constantProvider in module.ConstantProviders)
        {
          addMemberStatements.Add(AddConstantProvider(constantProvider));
        }

        foreach (var method in module.Methods)
        {
          addMemberStatements.Add(AddMethod(method));
        }

        foreach (var evnt in module.Events)
        {
          addMemberStatements.Add(AddEvent(evnt));
        }

        foreach (var function in module.Functions)
        {
          addMemberStatements.Add(AddFunction(function));
        }

        foreach (var initializer in module.Initializers)
        {
          addMemberStatements.Add(AddInitializer(initializer));
        }

        // generates
        //  return module;
        addMemberStatements.Add(
          ReturnStatement(IdentifierName(ReactNativeNames.Module)));

        // Generates:
        //    packageBuilder.AddModule("MyViewManager", () => new global::MyApp.ViewManager);
        addModuleStatements.Add(
          InvocationStatement(
            MemberAccessExpression(ReactNativeNames.PackageBuilderId, ReactNativeNames.AddModule),

            LiteralExpression(module.ModuleName),
            ParenthesizedLambdaExpression(
              parameterList: ParameterList(
                  Parameter(ReactNativeNames.ModuleBuilder)
                    .WithType(m_reactTypes.IReactModuleBuilder.ToTypeSyntax())),
                block: Block(
                  addMemberStatements),
                expressionBody: null)
          )
        );
      }

      // Generates:
      //    internal void CreateModules(IPackageBuilder packageBuilder)
      //    {
      //       ... registrationCalls (ses above)
      //    }
      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreateModules)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .AddParameterListParameters(
          GetPackageBuilderArgument())
        .WithBody(
          Block(
            addModuleStatements));
    }

    internal StatementSyntax AddConstants(IEnumerable<ReactConstant> constants)
    {
      // place all constants in a single constant provider
      var statements = new List<StatementSyntax>();

      // constants
      foreach (var constant in constants)
      {
        // generates:
        //   JSValueWriter.WriteObjectProperty(writer, "MyConst", module.MyConst);
        statements.Add(
          InvocationStatement(
            MemberAccessExpression(m_reactTypes.JSValueWriter, ReactNativeNames.WriteObjectPropertyMethodName),
            IdentifierName(ReactNativeNames.WriterLocalName),
            LiteralExpression(constant.Name),
            MemberAccessExpression(ReactNativeNames.Module, Identifier(constant.Symbol.Name))));
      }

      // generates:
      //  moduleBuilder.AddConstantProvider( (IJSValueWriter writer) => { ... } );
      return InvocationStatement(
          MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddConstantProvider),

          ParenthesizedLambdaExpression(
            parameterList: ParameterList(
                Parameter(ReactNativeNames.WriterLocalName)
                  .WithType(m_reactTypes.IJSValueWriter.ToTypeSyntax())),
            block: Block(
              statements),
            expressionBody: null
          )
        );
    }

    internal StatementSyntax AddConstantProvider(ReactConstantProvider constantProvider)
    {
      // generates:
      //  moduleBuilder.AddConstantProvider( (IJSValueWriter writer) => {
      //     var provider = new ReactConstantProvider(writer);
      //     module.ConstantProviderName(provider);
      //  });

      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddConstantProvider),

        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(m_reactTypes.IJSValueWriter.ToTypeSyntax())),
          block: Block(
            LocalDeclarationStatement(
              m_reactTypes.ReactConstantProvider,
              ReactNativeNames.ProviderLocalName,
              ObjectCreationExpression(
                m_reactTypes.ReactConstantProvider,
                IdentifierName(ReactNativeNames.WriterLocalName))),
            InvocationStatement(
              MemberAccessExpression(ReactNativeNames.Module, Identifier(constantProvider.Method.Name)),
              IdentifierName(ReactNativeNames.ProviderLocalName))),
          expressionBody: null
        )
      );
    }

    internal StatementSyntax AddMethod(ReactMethod method)
    {
      var statements = new List<StatementSyntax>();

      string methodReturnType = "Void";

      var parameters = method.Method.Parameters;

      var readArgsArguments = new List<ArgumentSyntax>(parameters.Length);
      readArgsArguments.Add(Argument(IdentifierName(ReactNativeNames.ReaderLocalName)));
      var args = new List<ExpressionSyntax>(parameters.Length);

      // generates:
      //   (out ArgType0 arg0, out ArgType1 arg1, ...);
      // as well as
      //   (arg0, arg1, ... )
      for (int i = 0; i < parameters.Length; i++)
      {
        var param = parameters[i];
        bool skipReadingArg = false;

        if (!method.IsSynchronous) // IsSynchronous methods don't have support for the callbacks
        {
          if (i == parameters.Length - 1)
          {
            if (IsPromiseType(parameters[i].Type, out var typeParameter))
            {
              // outArgs: Do not add an extra out parameter to be extracted from the JS side

              // generates:
              //    new ReactPromise<TResult>(writer, resolve, reject)
              // to (arg0, arg1, ...) to be passed when calling the remoted method.
              args.Add(ObjectCreationExpression(
                m_reactTypes.ReactPromise.Construct(typeParameter),
                IdentifierName(ReactNativeNames.WriterLocalName),
                IdentifierName(ReactNativeNames.ResolveLocalName),
                IdentifierName(ReactNativeNames.RejectLocalName)
              ));
              methodReturnType = "Promise";
              skipReadingArg = true;
            }

            // if the last argument is a callback, assume this is the resolve or resolve function
            if (IsSingleArgCallback(parameters[i].Type, out _))
            {
              var isReject = parameters.Length >= 2 &&
                             IsSingleArgCallback(parameters[i - 1].Type, out _);
              args.Add(GeneratePromiseInvocation(isReject));
              methodReturnType = isReject ? "TwoCallbacks" : "Callback";
              skipReadingArg = true;
            }
          }
          else if (i == parameters.Length - 2)
          {
            // if the last 2 argument is a callback, assume this is the resolve function
            if (IsSingleArgCallback(parameters[i].Type, out _) &&
                IsSingleArgCallback(parameters[i + 1].Type, out _))
            {
              args.Add(GeneratePromiseInvocation(isReject: false));
              skipReadingArg = true;
            }
          }
        }

        if (!skipReadingArg)
        {
          var variableName = "arg" + i.ToString(CultureInfo.InvariantCulture);

          readArgsArguments.Add(Argument(
            nameColon: null,
            refKindKeyword: Token(SyntaxKind.OutKeyword),
            expression: DeclarationExpression(
              param.Type.ToTypeSyntax(),
              SingleVariableDesignation(
                Identifier(variableName))
            )
          ));

          args.Add(IdentifierName(variableName));
        }
      }

      // generates:
      //  reader.ReadArgs( ... )
      statements.Add(InvocationStatement(
        MemberAccessExpression(m_reactTypes.JSValueReader, ReactNativeNames.ReadArgsMethodName),
        readArgsArguments));

      var methodCall = InvocationStatement(
        MemberAccessExpression(ReactNativeNames.Module, Identifier(method.Method.Name)),
        args.ToArray()
      );

      if (method.Method.ReturnsVoid)
      {
        // generate:
        //   module.MyMethod(arg0, arg1, ...)
        statements.Add(methodCall);
      }
      else
      {
        methodReturnType = "Callback";
        // generate:
        //   MyResult result = module.MyMethod(arg0, arg1, ...);
        statements.Add(
          LocalDeclarationStatement(
            method.Method.ReturnType,
            ReactNativeNames.ResultLocalName,
            methodCall.Expression));
        // generate:
        //   writer.WriteArgs(result);
        var writeArgs = InvocationExpression(
          MemberAccessExpression(m_reactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
          IdentifierName(ReactNativeNames.WriterLocalName),
          IdentifierName(ReactNativeNames.ResultLocalName));

        if (method.IsSynchronous)
        {
          statements.Add(ExpressionStatement(writeArgs));
        }
        else
        {
          // generate:
          //   resolve(.. writeargs ..);
          statements.Add(
            InvocationStatement(
              IdentifierName(ReactNativeNames.ResolveLocalName),
              writeArgs
            ));
        }
      }

      if (method.IsSynchronous)
      {
        // generate:
        //   moduleBuilder.AddSyncMethod(
        //    "MyMethod",
        //    (
        //    IJSValueReader reader,
        //    IJSValueWriter writer) =>
        //    {
        //       .... statements
        //    }
        return InvocationStatement(
          MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddSyncMethod),
          LiteralExpression(method.Name),
          ParenthesizedLambdaExpression(
            parameterList:
            ParameterList(
              Parameter(ReactNativeNames.ReaderLocalName)
                .WithType(m_reactTypes.IJSValueReader.ToTypeSyntax()),
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(m_reactTypes.IJSValueWriter.ToTypeSyntax())),
            block: Block(statements),
            expressionBody: null));
      }
      else
      {
        // generate:
        //   moduleBuilder.AddMethod(
        //    "MyMethod",
        //    MethodReturnType.xxx,
        //    (
        //    IJSValueReader reader,
        //    IJSValueWriter writer,
        //    MethodResultCallback resolve,
        //    MethodResultCallback reject) =>
        //    {
        //       .... statements
        //    }
        return InvocationStatement(
          MemberAccessExpression(
            ReactNativeNames.ModuleBuilder,
            method.IsSynchronous ? ReactNativeNames.AddSyncMethod : ReactNativeNames.AddMethod),
          LiteralExpression(method.Name),
          MemberAccessExpression(
            SyntaxKind.SimpleMemberAccessExpression,
            m_reactTypes.MethodReturnType.ToTypeSyntax(),
            SyntaxFactory.IdentifierName(methodReturnType)),
          ParenthesizedLambdaExpression(
            parameterList:
            ParameterList(
              Parameter(ReactNativeNames.ReaderLocalName)
                .WithType(m_reactTypes.IJSValueReader.ToTypeSyntax()),
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(m_reactTypes.IJSValueWriter.ToTypeSyntax()),
              Parameter(ReactNativeNames.ResolveLocalName)
                .WithType(m_reactTypes.MethodResultCallback.ToTypeSyntax()),
              Parameter(ReactNativeNames.RejectLocalName)
                .WithType(m_reactTypes.MethodResultCallback.ToTypeSyntax())),
            block: Block(
              statements),
            expressionBody: null));
      }
    }

    internal ExpressionSyntax GeneratePromiseInvocation(bool isReject)
    {
      // generates:
      //    value1 => resolve|reject(JSValueWriter.WriteArgs(writer, value1)
      return ParenthesizedLambdaExpression(
        parameterList: ParameterList(
          SeparatedList<ParameterSyntax>(
            new[]
            {
              Parameter(ReactNativeNames.ValueLocalName)
            })),
        block: null,
        expressionBody: InvocationExpression(
          IdentifierName(isReject ? ReactNativeNames.RejectLocalName : ReactNativeNames.ResolveLocalName),
          InvocationExpression(
            MemberAccessExpression(m_reactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
            IdentifierName(ReactNativeNames.WriterLocalName),
            IdentifierName(ReactNativeNames.ValueLocalName))
          )
        );
    }

    private bool IsPromiseType(ITypeSymbol type, [NotNullWhen(returnValue: true)] out ITypeSymbol? typeParameter)
    {
      if (type != null &&
             type is INamedTypeSymbol namedType &&
             namedType.IsGenericType &&
             namedType.ConstructUnboundGenericType()
               .Equals(m_reactTypes.IReactPromise.ConstructUnboundGenericType(), SymbolEqualityComparer.Default))
      {
        typeParameter = namedType.TypeArguments[0];
        return true;
      }

      typeParameter = null;
      return false;
    }

    private bool IsSingleArgCallback(ITypeSymbol type, [NotNullWhen(returnValue: true)] out ITypeSymbol? parameterType)
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

    internal StatementSyntax AddEvent(ReactEvent evnt)
    {
      // generates:
      //   moduleBuilder.AddInitializer((IReactContext reactContext) =>
      //      module.MyEvent = (ArgType0 arg0, ArgType1 arg1, ...) => reactContext.EmitJsEvent("eventEmitterName", "eventName", arg0, arg1, ...);
      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddInitializer),
        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(m_reactTypes.IReactContext.ToTypeSyntax())),
          block: null,
          expressionBody: GenerateCallback(evnt, ReactNativeNames.EmitJSEventFunctionName)));
    }

    internal StatementSyntax AddFunction(ReactFunction function)
    {
      // generates:
      //   moduleBuilder.AddInitializer((IReactContext reactContext) =>
      //      module.MyEvent = (ArgType0 arg0, ArgType1 arg1, ...) => reactContext.EmitJsFunction("moduleName", "eventName", arg0, arg1, ...);
      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddInitializer),
        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(m_reactTypes.IReactContext.ToTypeSyntax())),
          block: null,
          expressionBody: GenerateCallback(function, ReactNativeNames.CallJSFunctionFunctionName)));
    }

    internal StatementSyntax AddInitializer(ReactInitializer initializer)
    {
      // generates:
      //    moduleBuilder.AddInitializer((IReactContext reactContext) => module.initializerMethod(new ReactContext(reactContext)));
      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddInitializer),
        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(m_reactTypes.IReactContext.ToTypeSyntax())),
          block: null,
          expressionBody: InvocationExpression(
            MemberAccessExpression(ReactNativeNames.Module, Identifier(initializer.Method.Name)),
            ObjectCreationExpression(m_reactTypes.ReactContext, IdentifierName((ReactNativeNames.ReactContextLocalName)))
            )));
    }

    private ExpressionSyntax GenerateCallback(ReactCallback callback, SyntaxToken contextCallbackMethod)
    {

      var lambdaParams = new List<ParameterSyntax>(callback.CallbackParameters.Length);
      var arguments = new List<ArgumentSyntax>(callback.CallbackParameters.Length);
      arguments.Add(Argument(IdentifierName(ReactNativeNames.WriterLocalName)));

      for (int i = 0; i < callback.CallbackParameters.Length; i++)
      {
        var paramType = callback.CallbackParameters[i].Type;
        var identifierName = "arg" + i.ToString(CultureInfo.InvariantCulture);

        lambdaParams.Add(Parameter(Identifier(identifierName)).WithType(paramType.ToTypeSyntax()));
        arguments.Add(Argument(IdentifierName(identifierName)));
      }

      // generates:
      //  module.<callackName> = (ArgType0 arg0, ArgType1 arg0, ...) =>
      //    reactContext.<contextCallbackMethod>(
      //      eventEmitterName,
      //      eventName,
      //      writer => writer.WriteArgs(arg0, arg1, ...)
      //      )
      return
        AssignmentExpression(
          SyntaxKind.SimpleAssignmentExpression,
          MemberAccessExpression(ReactNativeNames.Module, Identifier(callback.Symbol.Name)),
          ParenthesizedLambdaExpression(
            parameterList: ParameterList(lambdaParams.ToArray()),
            block: null,
            expressionBody: InvocationExpression(
              MemberAccessExpression(ReactNativeNames.ReactContextLocalName, contextCallbackMethod),
              LiteralExpression(callback.CallbackContextName),
              LiteralExpression(callback.Name),
              ParenthesizedLambdaExpression(
                parameterList: ParameterList(Parameter(ReactNativeNames.WriterLocalName)),
                block: null,
                expressionBody: InvocationExpression(
                  MemberAccessExpression(m_reactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
                  arguments
                  )))));
    }
  }
}
