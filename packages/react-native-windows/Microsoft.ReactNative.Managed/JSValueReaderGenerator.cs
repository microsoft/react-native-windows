// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.CompilerServices;
using static System.Linq.Expressions.Expression;
using static Microsoft.ReactNative.Managed.JSValueGenerator;

namespace Microsoft.ReactNative.Managed
{
  public static class JSValueReaderGenerator
  {
    // The current assembly to ensure we always register the basic type readers
    private static readonly Assembly s_currentAssembly = typeof(JSValueReaderGenerator).Assembly;

    private static readonly ConcurrentDictionary<Assembly, bool> s_registerdAssemblies = new ConcurrentDictionary<Assembly, bool>();
    private static readonly ConcurrentDictionary<Type, MethodInfo> s_codeGenerateGenericExtensionMethods = new ConcurrentDictionary<Type, MethodInfo>();

    private static readonly Lazy<KeyValuePair<Type, MethodInfo>[]> s_allMethods;
    private static readonly Lazy<IReadOnlyDictionary<Type, MethodInfo>> s_nonGenericMethods;
    private static readonly Lazy<IReadOnlyDictionary<Type, SortedList<Type, MethodInfo>>> s_genericMethods;

    public static void RegisterAssembly(Assembly assembly)
    {
      // UnitTests re-register over and over, safe to skip if already added.
      if (s_registerdAssemblies.ContainsKey(assembly))
      {
        return;
      }

      s_registerdAssemblies.GetOrAdd(assembly, true);

      // Fail programs that register after we started serializing values.
      if (s_allMethods.IsValueCreated)
      {
        throw new InvalidOperationException("Cannot register assemblies dynamically after the first value is serialized.");
      }
    }

    public static void RegisterCodeGeneratorGenericExtensionMethod(Type type, MethodInfo method)
    {
      if (!method.IsGenericMethod)
      {
        throw new InvalidOperationException("Cannot register non generic methods.");
      }
      s_codeGenerateGenericExtensionMethods.TryAdd(type, method);
    }

    static JSValueReaderGenerator()
    {
      s_registerdAssemblies.GetOrAdd(s_currentAssembly, true);

      // Get all extension ReadValue methods for IJSValueReader and JSValue.
      // The first parameter must be IJSValueReader or JSValue.
      // The second parameter must be an 'out' parameter and must not be a generic parameter T.
      // This is to avoid endless recursion because ReadValue with generic parameter T
      // is the one who calls the JSValueReaderGenerator.
      s_allMethods = new Lazy<KeyValuePair<Type, MethodInfo>[]>(() =>
      {
        var extensionMethods =
          from assembly in s_registerdAssemblies.Keys
          from type in assembly.GetTypes()
          let typeInfo = type.GetTypeInfo()
          where typeInfo.IsSealed && !typeInfo.IsGenericType && !typeInfo.IsNested
          from member in type.GetMember(nameof(JSValueReader.ReadValue), BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic)
          let method = member as MethodInfo
          where (method != null) && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
          let parameters = method.GetParameters()
          where parameters.Length == 2
            && (parameters[0].ParameterType == typeof(IJSValueReader)
             || parameters[0].ParameterType == typeof(JSValue))
            && parameters[1].IsOut
          let dataType = parameters[1].ParameterType.GetElementType()
          where !dataType.IsGenericParameter
          select new KeyValuePair<Type, MethodInfo>(dataType, method);
        return extensionMethods.ToArray();
      });

      // Get all non-generic ReadValue extension methods.
      // They are easy to match and we can put them in a dictionary.
      s_nonGenericMethods = new Lazy<IReadOnlyDictionary<Type, MethodInfo>>(() =>
        s_allMethods.Value.Where(p => !p.Value.IsGenericMethod).ToDictionary(p => p.Key, p => p.Value));

      // Get all generic ReadValue extension methods.
      // Group them by generic type definitions and sort them by having more specific methods getting first.
      // The matching for generic types is more complicated: we first match the generic type definition,
      // and then walk the sorted list and try to match types one by one.
      // Note that the parameter could be an array of generic type. The array is not a generic type. 
      s_genericMethods = new Lazy<IReadOnlyDictionary<Type, SortedList<Type, MethodInfo>>>(() =>
      {
        var genericMethods =
          from pair in s_allMethods.Value.Union(s_codeGenerateGenericExtensionMethods)
          where pair.Value.IsGenericMethod
          let type = pair.Key
          let keyType = type.GetTypeInfo().IsGenericType ? type.GetGenericTypeDefinition()
            : type.IsArray ? typeof(Array)
            : throw new InvalidOperationException($"Unsupported argument type {type}")
          group pair by keyType into g
          select new KeyValuePair<Type, SortedList<Type, MethodInfo>>(
            g.Key, new SortedList<Type, MethodInfo>(
              g.ToDictionary(p => p.Key, p => p.Value), GenericTypeComparer.Default));
        return genericMethods.ToDictionary(p => p.Key, p => p.Value);
      });
    }

    public static MethodInfo ReadArgsOf(Type[] typeArgs)
    {
      var readArgsMethod =
        from member in typeof(JSValueReader).GetMember(
          nameof(JSValueReader.ReadArgs), BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        let isGeneric = method.IsGenericMethod
        where method != null
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == typeArgs.Length + 1
          && parameters[0].ParameterType == typeof(IJSValueReader)
        select isGeneric ? method.MakeGenericMethod(typeArgs) : method;
      return readArgsMethod.First();
    }

    static MethodInfo GetNextObjectProperty =>
      typeof(IJSValueReader).GetMethod(nameof(IJSValueReader.GetNextObjectProperty));

    private static MethodInfo MethodOf(string methodName, params Type[] typeArgs) =>
      typeof(JSValueReaderGenerator).GetMethod(methodName).MakeGenericMethod(typeArgs);

    static string ValueType => nameof(IJSValueReader.ValueType);

    static TypeWrapper ReadValueDelegateOf(Type typeArg) =>
      new TypeWrapper(typeof(ReadValueDelegate<>).MakeGenericType(typeArg));

    static TypeWrapper ReadClassDelegateOf(Type typeArg) =>
      new TypeWrapper(typeof(ReadClassDelegate<>).MakeGenericType(typeArg));

    static MethodInfo ReadValueOf(Type typeArg)
    {
      var readValueNoParamMethod =
        from member in typeof(JSValueReader).GetMember(
          nameof(JSValueReader.ReadValue), BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        where method != null
          && method.IsGenericMethod
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == 1
          && parameters[0].ParameterType == typeof(IJSValueReader)
        select method;
      return readValueNoParamMethod.First().MakeGenericMethod(typeArg);
    }

    static MethodInfo JSValueReadFrom => typeof(JSValue).GetMethod(nameof(JSValue.ReadFrom));

    public static Delegate GenerateReadValueDelegate(Type valueType)
    {
      return GenerateFromExtension(valueType)
        ?? GenerateReadValueFromGenericExtension(valueType)
        ?? GenerateReadValueForEnum(valueType)
        ?? GenerateReadValueForClass(valueType)
        ?? throw new Exception($"Cannot generate ReadValue delegate for type {valueType}");
    }

    // Creates a delegate from the ReadValue non-generic extension method
    private static Delegate GenerateFromExtension(Type valueType)
    {
      if (s_nonGenericMethods.Value.TryGetValue(valueType, out MethodInfo methodInfo))
      {
        var extendedType = methodInfo.GetParameters()[0].ParameterType;
        if (extendedType == typeof(IJSValueReader))
        {
          return methodInfo.CreateDelegate(typeof(ReadValueDelegate<>).MakeGenericType(valueType));
        }
        else if (extendedType == typeof(JSValue))
        {
          return GenerateReadValueFromJSValueExtension(valueType, methodInfo);
        }
      }

      return null;
    }

    private static Delegate GenerateReadValueFromGenericExtension(Type valueType)
    {
      var keyType = valueType.GetTypeInfo().IsGenericType ? valueType.GetGenericTypeDefinition()
        : valueType.IsArray ? typeof(Array)
        : null;

      if (keyType != null
        && s_genericMethods.Value.TryGetValue(keyType, out SortedList<Type, MethodInfo> candidateMethods))
      {
        foreach (var candidateMethod in candidateMethods)
        {
          var genericType = candidateMethod.Key;
          var methodInfo = candidateMethod.Value;
          var genericArgs = methodInfo.GetGenericArguments();
          if (TryMatchGenericType(valueType, genericType, genericArgs, out Type[] typeArgs))
          {
            var genericMethod = methodInfo.MakeGenericMethod(typeArgs);
            var extendedType = methodInfo.GetParameters()[0].ParameterType;
            if (extendedType == typeof(IJSValueReader))
            {
              return genericMethod.CreateDelegate(typeof(ReadValueDelegate<>).MakeGenericType(valueType));
            }
            else if (extendedType == typeof(JSValue))
            {
              return GenerateReadValueFromJSValueExtension(valueType, genericMethod);
            }
          }
        }
      }

      return null;
    }

    // Creates a delegate from the JSValue ReadValue non-generic extension method
    private static Delegate GenerateReadValueFromJSValueExtension(Type valueType, MethodInfo methodInfo)
    {
      // Generate code that looks like:
      //
      // (IJSValueReader reader, out Type value) =>
      // {
      //   var jsValue = JSValue.ReadFrom(reader);
      //   jsValue.ReadValue(out value);
      // }

      return ReadValueDelegateOf(valueType).CompileLambda(
        Parameter(typeof(IJSValueReader), out var reader),
        Parameter(valueType.MakeByRefType(), out var value),
        Variable(typeof(JSValue), out var jsValue, Call(JSValueReadFrom, reader)),
        jsValue.CallExt(methodInfo, value));
    }

    // It cannot be an extension method because it would conflict with the generic
    // extension method that uses T value type.
    public static void ReadEnum<TEnum>(IJSValueReader reader, out TEnum value) where TEnum : Enum
    {
      value = (TEnum)(object)reader.ReadValue<int>();
    }

    private static Delegate GenerateReadValueForEnum(Type valueType)
    {
      // Creates a delegate from the ReadEnum method
      return valueType.GetTypeInfo().IsEnum
        ? MethodOf(nameof(ReadEnum), valueType).CreateDelegate(typeof(ReadValueDelegate<>).MakeGenericType(valueType))
        : null;
    }

    public static void ReadClass<TClass>(IJSValueReader reader, out TClass value) where TClass : new()
    {
      // .Net Native seems to have a bug that does not allow to have 'out' or 'ref' parameters.
      // We use a return value to work around this issue.
      value = JSValueReaderReadClassOf<TClass>.ReadClass(reader);
    }

    private static Delegate GenerateReadValueForClass(Type valueType)
    {
      var valueTypeInfo = valueType.GetTypeInfo();
      bool isStruct = valueTypeInfo.IsValueType && !valueTypeInfo.IsEnum;
      bool isClass = valueTypeInfo.IsClass;
      if (isStruct || (isClass && valueType.GetConstructor(Type.EmptyTypes) != null))
      {
        return MethodOf(nameof(ReadClass), valueType).CreateDelegate(typeof(ReadValueDelegate<>).MakeGenericType(valueType));
      }

      return null;
    }

    internal static Delegate GenerateReadClassDelegate(Type classType)
    {
      // Generate code that looks like:
      //
      // (IJSValueReader reader) =>
      // {
      //   Type value = new Type();
      //   if (reader.ValueType == JSValueType.Object)
      //   {
      //     while (reader.GetNextObjectProperty(out string propertyName))
      //     {
      //       // For each object field or property
      //       switch(propertyName)
      //       {
      //         case "Field1": value.Field1 = reader.ReadValue<Field1Type>(); break;
      //         case "Field2": value.Field2 = reader.ReadValue<Field2Type>(); break;
      //         case "Prop1": value.Prop1 = reader.ReadValue<Prop1Type>(); break;
      //         case "Prop2": value.Prop2 = reader.ReadValue<Prop2Type>(); break;
      //       }
      //     }
      //   }
      //
      //   return value;
      // }

      var fields =
        from field in classType.GetFields(BindingFlags.Public | BindingFlags.Instance)
        where !field.IsInitOnly
        select new { field.Name, Type = field.FieldType };
      var properties =
        from property in classType.GetProperties(BindingFlags.Public | BindingFlags.Instance)
        let propertySetter = property.SetMethod
        where propertySetter != null && propertySetter.IsPublic
        select new { property.Name, Type = property.PropertyType };
      var members = fields.Concat(properties).ToArray();

      return ReadClassDelegateOf(classType).CompileLambda(
        Parameter(typeof(IJSValueReader), out var reader),
        Variable(classType, out var value, New(classType)),
        IfThen(Equal(reader.Property(ValueType), Constant(JSValueType.Object)),
          ifTrue: AutoBlock(
            Variable(typeof(string), out var propertyName),
            While(reader.Call(GetNextObjectProperty, propertyName),
            (members.Length != 0)
            ? Switch(propertyName,
                members.Select(member => SwitchCase(
                  value.SetPropertyStatement(member.Name, reader.CallExt(ReadValueOf(member.Type))),
                  Constant(member.Name))).ToArray()) as Expression
            : Default(typeof(void))))),
        value.AsExpression);
    }
  }

  //============================================================================
  // .Net Native has a bug that it fails with a 'ref' parameters in generated
  // lambdas. To work around this issue we use a return value for a ReadValue
  // generated against a class or struct.
  //============================================================================

  delegate T ReadClassDelegate<T>(IJSValueReader reader);

  // This class provides constant time access to the ReadClass delegate.
  static class JSValueReaderReadClassOf<T>
  {
    public static ReadClassDelegate<T> ReadClass =
      (ReadClassDelegate<T>)JSValueReaderGenerator.GenerateReadClassDelegate(typeof(T));
  }
}
