// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.ReactNative.Managed.CodeGen.Model;
using System.Collections.Generic;
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

        foreach (var getConstants in module.GetConstants)
        {
          addMemberStatements.Add(AddGetConstants(getConstants));
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
                    .WithType(ReactTypes.IReactModuleBuilder.ToTypeSyntax())),
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
            MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteObjectPropertyMethodName),
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
                  .WithType(ReactTypes.IJSValueWriter.ToTypeSyntax())),
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
                .WithType(ReactTypes.IJSValueWriter.ToTypeSyntax())),
          block: Block(
            LocalDeclarationStatement(
              ReactTypes.ReactConstantProvider,
              ReactNativeNames.ProviderLocalName,
              ObjectCreationExpression(
                ReactTypes.ReactConstantProvider,
                IdentifierName(ReactNativeNames.WriterLocalName))),
            InvocationStatement(
              MemberAccessExpression(ReactNativeNames.Module, Identifier(constantProvider.Method.Name)),
              IdentifierName(ReactNativeNames.ProviderLocalName))),
          expressionBody: null
        )
      );
    }

    internal StatementSyntax AddGetConstants(ReactGetConstants getConstants)
    {
      // generates:
      //  moduleBuilder.AddConstantProvider( (IJSValueWriter writer) => {
      //     var provider = new ReactConstantProvider(writer);
      //     var constants = module.GetConstantsName();
      //     provider.WriteProperties(constants);
      //  });

      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddConstantProvider),

        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(ReactTypes.IJSValueWriter.ToTypeSyntax())),
          block: Block(
            LocalDeclarationStatement(
              ReactTypes.ReactConstantProvider,
              ReactNativeNames.ProviderLocalName,
              ObjectCreationExpression(
                ReactTypes.ReactConstantProvider,
                IdentifierName(ReactNativeNames.WriterLocalName))),
            LocalDeclarationStatement(
              ReactNativeNames.ConstantsLocalName,
              InvocationExpression(
                MemberAccessExpression(ReactNativeNames.Module, Identifier(getConstants.Method.Name)))),
            InvocationStatement(
              MemberAccessExpression(ReactNativeNames.ProviderLocalName, ReactNativeNames.WritePropertiesMethodName),
              IdentifierName(ReactNativeNames.ConstantsLocalName))),
          expressionBody: null
        )
      );
    }

    internal StatementSyntax AddMethod(ReactMethod method)
    {
      var statements = new List<StatementSyntax>();
      var parameters = method.Method.Parameters;

      var args = new List<ExpressionSyntax>(parameters.Length);
      List<ArgumentSyntax>? readArgsArguments = null;
      if (method.EffectiveParameters.Count > 0)
      {
        readArgsArguments = new List<ArgumentSyntax>(method.EffectiveParameters.Count + 1) {
          Argument(IdentifierName(ReactNativeNames.ReaderLocalName))
        };

        // generates output arguments:
        //   (..., out ArgType0 arg0, out ArgType1 arg1, ...);
        readArgsArguments.AddRange(method.EffectiveParameters.Select((param, i) => Argument(
            nameColon: null,
            refKindKeyword: Token(SyntaxKind.OutKeyword),
            expression: DeclarationExpression(
              param.Type.ToTypeSyntax(),
              SingleVariableDesignation(ReactNativeNames.ArgLocalNames[i])))));

        // generates:
        //   (arg0, arg1, ... )
        args.AddRange(method.EffectiveParameters.Select((_, i) => IdentifierName(ReactNativeNames.ArgLocalNames[i])));
      }

      switch (method.ReturnStyle)
      {
        case ReactMethod.MethodReturnStyle.Promise:
          args.Add(ObjectCreationExpression(
            SymbolEqualityComparer.Default.Equals(method.EffectiveReturnType, ReactTypes.SystemVoid)
              ? ReactTypes.ReactPromiseOfVoid
              : ReactTypes.ReactPromise.Construct(method.EffectiveReturnType),
            IdentifierName(ReactNativeNames.WriterLocalName),
            IdentifierName(ReactNativeNames.ResolveLocalName),
            IdentifierName(ReactNativeNames.RejectLocalName)
          ));
          break;
        case ReactMethod.MethodReturnStyle.Callback:
          args.Add(GenerateCallbackInvocation(ReactNativeNames.ResolveLocalName, method.ResolveParameterCount));
          break;
        case ReactMethod.MethodReturnStyle.TwoCallbacks:
          args.Add(GenerateCallbackInvocation(ReactNativeNames.ResolveLocalName, method.ResolveParameterCount));
          args.Add(GenerateCallbackInvocation(ReactNativeNames.RejectLocalName, method.RejectParameterCount));
          break;
      }

      // Generate reader call only if we have argumentes to read.
      if (readArgsArguments != null)
      {
        // generates:
        //  reader.ReadArgs( ... )
        statements.Add(InvocationStatement(
          MemberAccessExpression(ReactTypes.JSValueReader, ReactNativeNames.ReadArgsMethodName),
          readArgsArguments));
      }

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
        // generate:
        //   MyResult result = module.MyMethod(arg0, arg1, ...);
        statements.Add(
          LocalDeclarationStatement(
            method.Method.ReturnType,
            ReactNativeNames.ResultLocalName,
            methodCall.Expression));

        if (method.ReturnStyle == ReactMethod.MethodReturnStyle.Task)
        {
          // generate:
          //  ReactTaskExtension.ContinueWith(result, writer, resolve, reject);
          statements.Add(InvocationStatement(
            MemberAccessExpression(ReactTypes.ReactTaskExtensions, ReactNativeNames.ContinueWith),
            IdentifierName(ReactNativeNames.ResultLocalName),
            IdentifierName(ReactNativeNames.WriterLocalName),
            IdentifierName(ReactNativeNames.ResolveLocalName),
            IdentifierName(ReactNativeNames.RejectLocalName)
            ));
        }
        else
        {
          if (method.IsSynchronous)
          {
            // generate:
            //   writer.WriteValue(result);
            var writeValue = InvocationExpression(
              MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteValueMethodName),
              IdentifierName(ReactNativeNames.WriterLocalName),
              IdentifierName(ReactNativeNames.ResultLocalName));

            statements.Add(ExpressionStatement(writeValue));
          }
          else
          {
            // generate:
            //   writer.WriteArgs(result);
            var writeArgs = InvocationExpression(
              MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
              IdentifierName(ReactNativeNames.WriterLocalName),
              IdentifierName(ReactNativeNames.ResultLocalName));

            // generate:
            //   resolve(.. writeargs ..);
            statements.Add(
              InvocationStatement(
                IdentifierName(ReactNativeNames.ResolveLocalName),
                writeArgs
              ));
          }
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
                .WithType(ReactTypes.IJSValueReader.ToTypeSyntax()),
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(ReactTypes.IJSValueWriter.ToTypeSyntax())),
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
            ReactTypes.MethodReturnType.ToTypeSyntax(),
            SyntaxFactory.IdentifierName(GetMethodReturnTypeFromStyle(method.ReturnStyle))),
          ParenthesizedLambdaExpression(
            parameterList:
            ParameterList(
              Parameter(ReactNativeNames.ReaderLocalName)
                .WithType(ReactTypes.IJSValueReader.ToTypeSyntax()),
              Parameter(ReactNativeNames.WriterLocalName)
                .WithType(ReactTypes.IJSValueWriter.ToTypeSyntax()),
              Parameter(ReactNativeNames.ResolveLocalName)
                .WithType(ReactTypes.MethodResultCallback.ToTypeSyntax()),
              Parameter(ReactNativeNames.RejectLocalName)
                .WithType(ReactTypes.MethodResultCallback.ToTypeSyntax())),
            block: Block(
              statements),
            expressionBody: null));
      }
    }

    internal ExpressionSyntax GenerateCallbackInvocation(SyntaxToken callbackName, int parameterCount)
    {
      // generates:
      //    (value0, value1, ...) => resolve|reject(JSValueWriter.WriteArgs(writer, value0, value1))
      return ParenthesizedLambdaExpression(
        parameterList: ParameterList(
          SeparatedList(
            ReactNativeNames.ValueLocalNames.Take(parameterCount).Select(name => Parameter(name)).ToArray())),
        block: null,
        expressionBody: InvocationExpression(
          IdentifierName(callbackName),
          InvocationExpression(
            MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
            Enumerable.Concat(
              Enumerable.Repeat(IdentifierName(ReactNativeNames.WriterLocalName), 1),
              ReactNativeNames.ValueLocalNames.Take(parameterCount).Select(name => IdentifierName(name))))));
    }

    internal StatementSyntax AddEvent(ReactEvent evnt)
    {
      // generates:
      //   moduleBuilder.AddInitializer((IReactContext reactContext) =>
      //      module.MyEvent = (ArgType0 arg0, ArgType1 arg1, ...) => new ReactContext(reactContext).EmitJsEvent("eventEmitterName", "eventName", arg0, arg1, ...);
      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddInitializer),
        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(ReactTypes.IReactContext.ToTypeSyntax())),
          block: null,
          expressionBody: GenerateCallback(evnt, ReactNativeNames.EmitJSEventFunctionName)));
    }

    internal StatementSyntax AddFunction(ReactFunction function)
    {
      // generates:
      //   moduleBuilder.AddInitializer((IReactContext reactContext) =>
      //      module.MyEvent = (ArgType0 arg0, ArgType1 arg1, ...) => new ReactContext(reactContext).EmitJsFunction("moduleName", "eventName", arg0, arg1, ...);
      return InvocationStatement(
        MemberAccessExpression(ReactNativeNames.ModuleBuilder, ReactNativeNames.AddInitializer),
        ParenthesizedLambdaExpression(
          parameterList: ParameterList(
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(ReactTypes.IReactContext.ToTypeSyntax())),
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
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(ReactTypes.IReactContext.ToTypeSyntax())),
          block: null,
          expressionBody: InvocationExpression(
            MemberAccessExpression(ReactNativeNames.Module, Identifier(initializer.Method.Name)),
            ObjectCreationExpression(ReactTypes.ReactContext, IdentifierName(ReactNativeNames.ReactContextLocalName))
            )));
    }

    private ExpressionSyntax GenerateCallback(ReactCallback callback, SyntaxToken contextCallbackMethod)
    {

      var lambdaParams = new List<ParameterSyntax>(callback.CallbackParameters.Length);
      var arguments = new List<ArgumentSyntax>(callback.CallbackParameters.Length);
      arguments.Add(Argument(LiteralExpression(callback.CallbackContextName)));
      arguments.Add(Argument(LiteralExpression(callback.Name)));

      for (int i = 0; i < callback.CallbackParameters.Length; i++)
      {
        var paramType = callback.CallbackParameters[i].Type;
        var identifierName = "arg" + i.ToString(CultureInfo.InvariantCulture);

        lambdaParams.Add(Parameter(Identifier(identifierName)).WithType(paramType.ToTypeSyntax()));
        arguments.Add(Argument(IdentifierName(identifierName)));
      }

      // generates:
      //  module.<callackName> = (ArgType0 arg0, ArgType1 arg0, ...) =>
      //    new ReactContext(reactContext).<contextCallbackMethod>(
      //      eventEmitterName,
      //      eventName,
      //      arg0, arg1, ...
      //      )
      return
        AssignmentExpression(
          SyntaxKind.SimpleAssignmentExpression,
          MemberAccessExpression(ReactNativeNames.Module, Identifier(callback.Symbol.Name)),
          ParenthesizedLambdaExpression(
            parameterList: ParameterList(lambdaParams.ToArray()),
            block: null,
            expressionBody: InvocationExpression(
              MemberAccessExpression(
                ObjectCreationExpression(ReactTypes.ReactContext, IdentifierName(ReactNativeNames.ReactContextLocalName)),
                contextCallbackMethod),
              arguments
              )));
    }

    private string GetMethodReturnTypeFromStyle(ReactMethod.MethodReturnStyle returnStyle)
    {
      switch (returnStyle)
      {
        case ReactMethod.MethodReturnStyle.Void:
          return "Void";
        case ReactMethod.MethodReturnStyle.Task:
        case ReactMethod.MethodReturnStyle.Promise:
          return "Promise";
        case ReactMethod.MethodReturnStyle.Callback:
        case ReactMethod.MethodReturnStyle.ReturnValue:
          return "Callback";
        case ReactMethod.MethodReturnStyle.TwoCallbacks:
          return "TwoCallbacks";
        default:
          throw new InvalidOperationException("Unexpected ReturnStyle");
      }
    }
  }
}
