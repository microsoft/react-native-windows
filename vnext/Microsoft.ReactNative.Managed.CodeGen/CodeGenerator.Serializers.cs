// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Collections.Generic;
using System.Diagnostics.ContractsLight;
using System.Linq;
using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;
using static Microsoft.ReactNative.Managed.CodeGen.SyntaxHelpers;
using StatementSyntax = Microsoft.CodeAnalysis.CSharp.Syntax.StatementSyntax;
using SyntaxKind = Microsoft.CodeAnalysis.CSharp.SyntaxKind;

namespace Microsoft.ReactNative.Managed.CodeGen
{
  /// <summary>
  /// Code generation for serialization
  /// </summary>
  public partial class CodeGenerator
  {
    internal IEnumerable<MemberDeclarationSyntax> CreateSerializers(IEnumerable<INamedTypeSymbol> typesToSerialize)
    {
      var classMembers = new List<MemberDeclarationSyntax>();
      var registrationCalls = new List<StatementSyntax>();

      foreach (var group in typesToSerialize.GroupBy(type => type.TypeKind))
      {
        switch (group.Key)
        {
          case TypeKind.Enum:
            classMembers.Add(CreateEnumSerializers(group));
            registrationCalls.Add(InvocationStatement(ReactNativeNames.CreateEnumSerializers));
            break;
          case TypeKind.Class:
          case TypeKind.Struct:
            classMembers.Add(CreateObjectSerializers(group));
            registrationCalls.Add(InvocationStatement(ReactNativeNames.CreateObjectSerializers));
            break;
          default:
            throw Contract.AssertFailure("Unexpected type to serialize");
        }
      }

      // Generates:
      //    internal void CreateSerializers()
      //    {
      //       ... registrationCalls (ses above)
      //    }
      classMembers.Add(MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreateSerializers)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .WithBody(
          Block(
            registrationCalls)));

      return classMembers;
    }

    internal MemberDeclarationSyntax CreateEnumSerializers(IEnumerable<INamedTypeSymbol> symbols)
    {
      var registrationCalls = new List<StatementSyntax>();

      foreach (var symbol in symbols)
      {
        Contract.Assert(symbol.TypeKind == TypeKind.Enum);
        Contract.Assert(symbol.EnumUnderlyingType != null);

        // Generates:
        //   (EnumType)reader.ReadValue<EnumBaseType>();
        // TODO: Should we add support for strings, today the strings must be integers
        registrationCalls.Add(
          CodeGenRead(
            symbol,
            expression: CastExpression(
              symbol.ToTypeSyntax(),
              InvocationExpression(
                MemberAccessExpression(
                  SyntaxKind.SimpleMemberAccessExpression,
                  ReactTypes.JSValueReader.ToTypeSyntax(),
                  GenericName(ReactNativeNames.ReadValueMethodName, symbol.EnumUnderlyingType.ToTypeSyntax())
                ),
                new[]
                {
                  Argument(IdentifierName(ReactNativeNames.ReaderLocalName)),
                }
              )
            )
          )
        );

        // Generates:
        //   writer.WriteValue((EnumBaseType)value);
        // TODO: Should we add support for strings, today the strings must be integers
        registrationCalls.Add(
          CodeGenWrite(
            symbol,
            expression: InvocationExpression(
              MemberAccessExpression(
                SyntaxKind.SimpleMemberAccessExpression,
                ReactTypes.JSValueWriter.ToTypeSyntax(),
                GenericName(ReactNativeNames.WriteValueMethodName, symbol.EnumUnderlyingType.ToTypeSyntax())),
              IdentifierName(ReactNativeNames.WriterLocalName),
              CastExpression(
                symbol.EnumUnderlyingType.ToTypeSyntax(),
                IdentifierName(ReactNativeNames.ValueLocalName)
              )
            )
          )
        );
      }

      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreateEnumSerializers)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .WithBody(
          Block(
            registrationCalls));
    }

    internal MemberDeclarationSyntax CreateObjectSerializers(IEnumerable<INamedTypeSymbol> symbols)
    {

      var registrationCalls = new List<StatementSyntax>();

      foreach (var symbol in symbols)
      {
        Contract.Assert(symbol.TypeKind == TypeKind.Enum);
        Contract.Assert(symbol.EnumUnderlyingType != null);

        var readOperations = new List<SwitchSectionSyntax>();
        var writeOperations = new List<StatementSyntax>();

        writeOperations.Add(
          InvocationStatement(
            MemberAccessExpression(ReactNativeNames.WriterLocalName, ReactNativeNames.WriteObjectBeginMethodName))
        );

        // Generates either:
        //    case "Field1": value.Field1 = reader.ReadValue<Field1Type>(); break;
        // and/or
        //     writer.WriteObjectProperty("Field2", value.Field2);
        // for each member
        foreach (var member in symbol.GetMembers())
        {
          if (!IsAccessible(member))
          {
            continue;
          }

          string name;
          ISymbol type;
          bool emitRead;
          bool emitWrite;
          if (!member.IsStatic && !member.IsExtern)
          {
            if (member is IFieldSymbol field)
            {
              name = field.Name;
              type = field.Type;
              emitRead = !field.IsConst && !field.IsReadOnly;
              emitWrite = true;
            }
            else if
              (member is IPropertySymbol property)
            {
              name = property.Name;
              type = property.Type;
              emitRead = !property.IsReadOnly;
              emitWrite = !property.IsWriteOnly;
            }
            else
            {
              continue;
            }

            if (emitRead)
            {
              readOperations.Add(ReadSwitch(name, type));
            }

            if (emitWrite)
            {
              writeOperations.Add(
                ExpressionStatement(
                  InvocationExpression(
                    MemberAccessExpression(
                      SyntaxKind.SimpleMemberAccessExpression,
                      ReactTypes.JSValueWriter.ToTypeSyntax(),
                      GenericName(ReactNativeNames.WriteObjectPropertyMethodName, type.ToTypeSyntax())),
                    IdentifierName(ReactNativeNames.WriterLocalName),
                    LiteralExpression(name),
                    MemberAccessExpression(ReactNativeNames.ValueLocalName, Identifier(name)))
                )
              );
            }
          }
        }

        writeOperations.Add(
          InvocationStatement(
            MemberAccessExpression(ReactNativeNames.WriterLocalName, ReactNativeNames.WriteObjectEndMethodName))
        );

        // Generates:
        // (IJSValueReader reader, out MyType value) =>
        // {
        //   value = new MyType();
        //   if (reader.ValueType == JSValueType.Object)
        //   {
        //     while (reader.GetNextObjectProperty(out string propertyName))
        //     {
        //       // For each object field or property
        //       switch(propertyName)
        //       {
        //         ... readOperations ...
        //       }
        //     }
        //   }
        // }
        registrationCalls.Add(
          CodeGenRead(
            symbol,
            statements: new StatementSyntax[]
            {
              ExpressionStatement(
                AssignmentExpression(
                  SyntaxKind.SimpleAssignmentExpression,
                  IdentifierName(ReactNativeNames.ValueLocalName),
                  ObjectCreationExpression(symbol))
              ),
              IfStatement(
                BinaryExpression(
                  SyntaxKind.EqualsExpression,
                  MemberAccessExpression(ReactNativeNames.ReaderLocalName, ReactNativeNames.ValueTypePropertyName),
                  MemberAccessExpression(ReactTypes.JSValueType, ReactNativeNames.ObjectEnumMemberName)),
                Block(
                  WhileStatement(
                    InvocationExpression(
                      MemberAccessExpression(ReactNativeNames.ReaderLocalName,
                        ReactNativeNames.GetNextObjectPropertyMethodName),
                      new[]
                      {
                        Argument(
                          DeclarationExpression(
                            PredefinedType(
                              Token(SyntaxKind.StringKeyword)),
                            SingleVariableDesignation(
                              ReactNativeNames.PropertyNameLocalName)))
                          .WithRefOrOutKeyword(Token(SyntaxKind.OutKeyword))
                      }),
                    Block(
                      SwitchStatement(
                        IdentifierName(ReactNativeNames.PropertyNameLocalName),
                        new SyntaxList<SwitchSectionSyntax>(
                          readOperations
                        )))
                  )
                )
              )
            })
        );

        // Generates:
        // (IJSValueWriter writer, Type value) =>
        // {
        //   if (value != null) // we do not check it for structs
        //   {
        //     writer.WriteObjectBegin();
        //     writer.WriteObjectProperty("Field1", value.Field1);
        //     writer.WriteObjectProperty("Field2", value.Field2);
        //     writer.WriteObjectProperty("Prop1", value.Prop1);
        //     writer.WriteObjectProperty("Prop2", value.Prop2);
        //     writer.WriteObjectEnd();
        //   }
        //   else
        //   {
        //     writer.WriteNull();
        //   }
        // }
        registrationCalls.Add(
          CodeGenWrite(
            symbol,
            statements: new[]
            {
              IfStatement(
                BinaryExpression(
                  SyntaxKind.NotEqualsExpression,
                  IdentifierName(ReactNativeNames.ValueLocalName),
                  LiteralExpression(SyntaxKind.NullLiteralExpression)),
                Block(
                  writeOperations
                ),
                ElseClause(
                  Block(
                    InvocationStatement(
                      MemberAccessExpression(ReactNativeNames.WriterLocalName, ReactNativeNames.WriteNullMethodName)
                    )
                  )
                )
              )
            })
        );
      }

      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.CreateObjectSerializers)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .WithBody(
          Block(
            registrationCalls));
    }

    private bool IsAccessible(ISymbol symbol)
    {
      return symbol.DeclaredAccessibility == Accessibility.Public ||
             symbol.DeclaredAccessibility == Accessibility.Internal;
    }

    private SwitchSectionSyntax ReadSwitch(string fieldName, ISymbol fieldType)
    {
      return SwitchSection(
        new SyntaxList<SwitchLabelSyntax>(
          CaseSwitchLabel(LiteralExpression(fieldName))
        ),
        new SyntaxList<StatementSyntax>(
          new StatementSyntax[]
          {
            ExpressionStatement(
              AssignmentExpression(
                SyntaxKind.SimpleAssignmentExpression,
                MemberAccessExpression(
                  SyntaxKind.SimpleMemberAccessExpression,
                  IdentifierName(ReactNativeNames.ValueLocalName),
                  IdentifierName(fieldName)),
                InvocationExpression(
                  MemberAccessExpression(
                    SyntaxKind.SimpleMemberAccessExpression,
                    ReactTypes.JSValueReader.ToTypeSyntax(),
                    GenericName(ReactNativeNames.ReadValueMethodName, fieldType.ToTypeSyntax())
                  ),
                  new[]
                  {
                    Argument(IdentifierName(ReactNativeNames.ReaderLocalName)),
                  }
                )
              )),
            BreakStatement()
          })
      );
    }

    private StatementSyntax CodeGenRead(ITypeSymbol symbol, ExpressionSyntax? expression = null,
      IEnumerable<StatementSyntax>? statements = null)
    {
      Contract.Requires(expression != null ^ statements != null, "Only one of the args can be null.");
      // Generates:
      //   JSValueReaderCodeGen<SymbolType>.ReadValue = (reader, out value) => value = ...readExpression...;
      return
        ExpressionStatement(
          AssignmentExpression(
            SyntaxKind.SimpleAssignmentExpression,
            MemberAccessExpression(
              ReactTypes.JSValueReaderCodeGen.Construct(symbol),
              ReactNativeNames.ReadValueMethodName),
            ParenthesizedLambdaExpression(
              parameterList: ParameterList(
                Parameter(ReactNativeNames.ReaderLocalName)
                  .WithType(ReactTypes.IJSValueReader.ToTypeSyntax())
                ,
                Parameter(ReactNativeNames.ValueLocalName)
                  .WithModifiers(SyntaxTokenList.Create(Token(SyntaxKind.OutKeyword)))
                  .WithType(symbol.ToTypeSyntax())
              ),
              block: statements == null
                ? null
                : Block(statements),
              expressionBody: expression == null
                ? null
                : AssignmentExpression(
                  SyntaxKind.SimpleAssignmentExpression,
                  IdentifierName(ReactNativeNames.ValueLocalName),
                  expression)
            )
          )
        );
    }

    private StatementSyntax CodeGenWrite(
      ITypeSymbol symbol,
      ExpressionSyntax? expression = null,
      IEnumerable<StatementSyntax>? statements = null)
    {
      Contract.Requires(expression != null ^ statements != null, "Only one of the args can be null.");

      // Generates:
      //   JSValueWriterCodeGen<SymbolType>.WriteValue = (writer, value) =>  ...writeExpression...;
      return
        ExpressionStatement(
          AssignmentExpression(
            SyntaxKind.SimpleAssignmentExpression,
            MemberAccessExpression(
              ReactTypes.JSValueWriterCodeGen.Construct(symbol),
              ReactNativeNames.WriteValueMethodName),
            ParenthesizedLambdaExpression(
              parameterList: ParameterList(
                Parameter(ReactNativeNames.WriterLocalName),
                Parameter(ReactNativeNames.ValueLocalName)
              ),
              block: statements == null
                ? null
                : Block(statements),
              expressionBody: expression
            )
          )
        );
    }

    internal MemberDeclarationSyntax RegisterExtensionReaders(IDictionary<ITypeSymbol, IMethodSymbol> jsReaderFunctions)
    {
      var nonGeneric = new List<StatementSyntax>();
      var generic = new List<StatementSyntax>();

      foreach ((var type, var method) in jsReaderFunctions)
      {
        if (method.IsGenericMethod)
        {
          generic.Add(InvocationStatement(
            MemberAccessExpression(
              ReactTypes.JSValueReaderGenerator,
              ReactNativeNames.RegisterCodeGeneratorGenericExtensionMethod),
            TypeOfExpression(type.ToTypeSyntax())
            ));
        }
        else
        {
          nonGeneric.Add(ExpressionStatement(
            AssignmentExpression(
              SyntaxKind.SimpleAssignmentExpression,
              MemberAccessExpression(
                ReactTypes.JSValueReaderCodeGen.Construct(type),
                ReactNativeNames.ReadValueMethodName),
              MemberAccessExpression(
                method.ContainingType,
                Identifier(method.Name)
                ))));
        }
      }

      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.RegisterExtensionReaders)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .WithBody(
          Block(
            Block(nonGeneric),
            Block(generic)));
    }

    internal MemberDeclarationSyntax RegisterExtensionWriter(IDictionary<ITypeSymbol, IMethodSymbol> jsWriterFunctions)
    {
      var nonGeneric = new List<StatementSyntax>();
      var generic = new List<StatementSyntax>();

      foreach ((var type, var method) in jsWriterFunctions)
      {
        if (method.IsGenericMethod)
        {
          generic.Add(InvocationStatement(
            MemberAccessExpression(
              ReactTypes.JSValueWriterGenerator,
              ReactNativeNames.RegisterCodeGeneratorGenericExtensionMethod),
            TypeOfExpression(type.ToTypeSyntax())
          ));
        }
        else
        {
          nonGeneric.Add(ExpressionStatement(
            AssignmentExpression(
              SyntaxKind.SimpleAssignmentExpression,
              MemberAccessExpression(
                ReactTypes.JSValueWriterCodeGen.Construct(type),
                ReactNativeNames.WriteValueMethodName),
              MemberAccessExpression(
                method.ContainingType,
                Identifier(method.Name)
              ))));
        }
      }

      return MethodDeclaration(
          PredefinedType(Token(SyntaxKind.VoidKeyword)),
          ReactNativeNames.RegisterExtensionWriter)
        .AddModifiers(
          Token(SyntaxKind.InternalKeyword))
        .WithBody(
          Block(
            Block(nonGeneric),
            Block(generic)));
    }

    public void CreateSerializationRegistration(
      ICollection<StatementSyntax> separateRegistrationCalls,
      ICollection<MemberDeclarationSyntax> classMembers)
    {
      //// Generates
      //var assembly = typeof(T).Assembly;
      //JSValueReaderGenerator.RegisterAssembly(assembly);
      //JSValueWriterGenerator.RegisterAssembly(assembly);

      // To ensure we haven't missed any types, still register things for reflection lookup
      // When we have confidence we have 100% we can remove the following lines.

      separateRegistrationCalls.Add(
        LocalDeclarationStatement(
          ReactNativeNames.AssemblyLocalName,
          MemberAccessExpression(
            SyntaxKind.SimpleMemberAccessExpression,
            InvocationExpression(
              MemberAccessExpression(
                SyntaxKind.SimpleMemberAccessExpression,
                ThisExpression(),
                IdentifierName(ReactNativeNames.GetTypeMethodName))),
            IdentifierName(ReactNativeNames.AssemblyPropertyName))));
      separateRegistrationCalls.Add(
        InvocationStatement(
          MemberAccessExpression(ReactTypes.JSValueReaderGenerator, ReactNativeNames.RegisterAssemblyMethodName),
          IdentifierName(ReactNativeNames.AssemblyLocalName)));
      separateRegistrationCalls.Add(
        InvocationStatement(
          MemberAccessExpression(ReactTypes.JSValueWriterGenerator, ReactNativeNames.RegisterAssemblyMethodName),
          IdentifierName(ReactNativeNames.AssemblyLocalName)));
    }
  }
}
