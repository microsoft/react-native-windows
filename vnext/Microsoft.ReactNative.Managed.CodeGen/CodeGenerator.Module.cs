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

    internal StatementSyntax AddMethod(ReactMethod method)
    {
      var statements = new List<StatementSyntax>();

      var parameters = method.Method.Parameters;

      var readArgsArguments = new List<ArgumentSyntax>(parameters.Length);
      readArgsArguments.Add(Argument(IdentifierName(ReactNativeNames.ReaderLocalName)));
      var args = new List<ExpressionSyntax>(parameters.Length);

      // generates:
      //   (out ArgType0 arg0, out ArgType1 arg1, ...);
      // as well as
      //   (arg0, arg1, ... )
      for (int i = 0; i < method.EffectiveParameters.Count; i++)
      {
        var param = method.EffectiveParameters[i];
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

      switch (method.ReturnStyle)
      {
        case ReactMethod.MethodReturnStyle.Promise:
          args.Add(ObjectCreationExpression(
            ReactTypes.ReactPromise.Construct(method.EffectiveReturnType),
            IdentifierName(ReactNativeNames.WriterLocalName),
            IdentifierName(ReactNativeNames.ResolveLocalName),
            IdentifierName(ReactNativeNames.RejectLocalName)
          ));
          break;
        case ReactMethod.MethodReturnStyle.Callback:
          args.Add(GeneratePromiseInvocation(isReject: false));
          break;
        case ReactMethod.MethodReturnStyle.TwoCallbacks:
          args.Add(GeneratePromiseInvocation(isReject: false));
          args.Add(GeneratePromiseInvocation(isReject: true));
          break;
      }

      // generates:
      //  reader.ReadArgs( ... )
      statements.Add(InvocationStatement(
        MemberAccessExpression(ReactTypes.JSValueReader, ReactNativeNames.ReadArgsMethodName),
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
          // generate:
          //   writer.WriteArgs(result);
          var writeArgs = InvocationExpression(
            MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
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
            MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
            IdentifierName(ReactNativeNames.WriterLocalName),
            IdentifierName(ReactNativeNames.ValueLocalName))
          )
        );
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
            Parameter(ReactNativeNames.ReactContextLocalName).WithType(ReactTypes.IReactContext.ToTypeSyntax())),
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
            ObjectCreationExpression(ReactTypes.ReactContext, IdentifierName((ReactNativeNames.ReactContextLocalName)))
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
                  MemberAccessExpression(ReactTypes.JSValueWriter, ReactNativeNames.WriteArgsMethodName),
                  arguments
                  )))));
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
