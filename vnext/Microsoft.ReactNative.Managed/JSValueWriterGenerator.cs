// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Xml.Schema;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  public static class JSValueWriterGenerator
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

    static JSValueWriterGenerator()
    {
      s_registerdAssemblies.GetOrAdd(s_currentAssembly, true);

      // Get all extension WriteValue methods for IJSValueWriter.
      // The first parameter must be IJSValueWriter.
      // The second parameter must be not a generic parameter T.
      // This is to avoid endless recursion because WriteValue with generic parameter T
      // is the one who calls the JSValueWriterGenerator.
      s_allMethods = new Lazy<KeyValuePair<Type, MethodInfo>[]>(() =>
      {
        var extensionMethods =
          from assembly in s_registerdAssemblies.Keys
          from type in assembly.GetTypes()
          let typeInfo = type.GetTypeInfo()
          where typeInfo.IsSealed && !typeInfo.IsGenericType && !typeInfo.IsNested
          from member in type.GetMember(nameof(JSValueWriter.WriteValue), BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic)
          let method = member as MethodInfo
          where (method != null) && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
          let parameters = method.GetParameters()
          where parameters.Length == 2
            && parameters[0].ParameterType == typeof(IJSValueWriter)
          let dataType = parameters[1].ParameterType
          where !dataType.IsByRef && !dataType.IsGenericParameter
          select new KeyValuePair<Type, MethodInfo>(dataType, method);
        return extensionMethods.ToArray();
      });

      // Get all non-generic WriteValue extension methods.
      // They are easy to match and we can put them in a dictionary.
      s_nonGenericMethods = new Lazy<IReadOnlyDictionary<Type, MethodInfo>>(() =>
        s_allMethods.Value.Where(p => !p.Value.IsGenericMethod).ToDictionary(p => p.Key, p => p.Value));

      // Get all generic WriteValue extension methods.
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

    public static MethodInfo WriteValueOf(Type typeArg)
    {
      var readValueNoParamMethod =
        from member in typeof(JSValueWriter).GetMember(
          nameof(JSValueWriter.WriteValue), BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        where method != null
          && method.IsGenericMethod
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == 2
          && parameters[0].ParameterType == typeof(IJSValueWriter)
          && parameters[1].ParameterType.IsGenericParameter
        select method;
      return readValueNoParamMethod.First().MakeGenericMethod(typeArg);
    }

    public static MethodInfo WriteArgsOf(params Type[] typeArgs)
    {
      var writeArgsMethod =
        from member in typeof(JSValueWriter).GetMember(
          nameof(JSValueWriter.WriteArgs), BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        let isGeneric = method.IsGenericMethod
        where method != null
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == typeArgs.Length + 1
          && parameters[0].ParameterType == typeof(IJSValueWriter)
        select isGeneric ? method.MakeGenericMethod(typeArgs) : method;
      return writeArgsMethod.First();
    }

    private static MethodInfo MethodOf(string methodName, params Type[] typeArgs) =>
      typeof(JSValueWriterGenerator).GetMethod(methodName).MakeGenericMethod(typeArgs);

    static TypeWrapper WriteValueDelegateOf(Type typeArg) =>
      new TypeWrapper(typeof(WriteValueDelegate<>).MakeGenericType(typeArg));

    static MethodInfo WriteObjectPropertyOf(Type typeArg)
    {
      var writeObjectProperty =
        from member in typeof(JSValueWriter).GetMember(
          nameof(JSValueWriter.WriteObjectProperty), BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        where method != null
          && method.IsGenericMethod
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == 3
        && parameters[0].ParameterType == typeof(IJSValueWriter)
        && parameters[1].ParameterType == typeof(string)
        select method;
      return writeObjectProperty.First().MakeGenericMethod(typeArg);
    }

    static MethodInfo WriteObjectBegin => typeof(IJSValueWriter).GetMethod(nameof(IJSValueWriter.WriteObjectBegin));
    static MethodInfo WriteObjectEnd => typeof(IJSValueWriter).GetMethod(nameof(IJSValueWriter.WriteObjectEnd));
    static MethodInfo WriteNull => typeof(IJSValueWriter).GetMethod(nameof(IJSValueWriter.WriteNull));

    public static Delegate GenerateWriteValueDelegate(Type valueType)
    {
      return GenerateWriteValueFromExtension(valueType)
        ?? GenerateWriteValueFromGenericExtension(valueType)
        ?? GenerateWriteValueForEnum(valueType)
        ?? GenerateWriteValueForClass(valueType)
        ?? throw new Exception($"Cannot generate WriteValue delegate for type {valueType}");
    }

    // Creates a delegate from the WriteValue non-generic extension method
    private static Delegate GenerateWriteValueFromExtension(Type valueType)
    {
      if (s_nonGenericMethods.Value.TryGetValue(valueType, out MethodInfo methodInfo))
      {
        return methodInfo.CreateDelegate(typeof(WriteValueDelegate<>).MakeGenericType(valueType));
      }

      return null;
    }

    private static Delegate GenerateWriteValueFromGenericExtension(Type valueType)
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
            return genericMethod.CreateDelegate(typeof(WriteValueDelegate<>).MakeGenericType(valueType));
          }
        }
      }

      return null;
    }

    // It cannot be an extension method because it would conflict with the generic
    // extension method that uses T value type.
    public static void WriteEnum<TEnum>(IJSValueWriter writer, TEnum value) where TEnum : Enum
    {
      writer.WriteValue((int)(object)value);
    }

    private static Delegate GenerateWriteValueForEnum(Type valueType)
    {
      // Creates a delegate from the WriteEnum method
      return valueType.GetTypeInfo().IsEnum
        ? MethodOf(nameof(WriteEnum), valueType).CreateDelegate(typeof(WriteValueDelegate<>).MakeGenericType(valueType))
        : null;
    }

    private static Delegate GenerateWriteValueForClass(Type valueType)
    {
      // Generate code that looks like:
      //
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

      var valueTypeInfo = valueType.GetTypeInfo();
      bool isStruct = valueTypeInfo.IsValueType && !valueTypeInfo.IsEnum;
      bool isClass = valueTypeInfo.IsClass;
      if (isStruct || isClass)
      {
        var fields =
          from field in valueType.GetFields(BindingFlags.Public | BindingFlags.Instance)
          where !field.IsInitOnly
          select new { field.Name, Type = field.FieldType };
        var properties =
          from property in valueType.GetProperties(BindingFlags.Public | BindingFlags.Instance)
          let propertySetter = property.SetMethod
          where propertySetter != null && propertySetter.IsPublic
          select new { property.Name, Type = property.PropertyType };
        var members = fields.Concat(properties).ToArray();

        return WriteValueDelegateOf(valueType).CompileLambda(
          Parameter(typeof(IJSValueWriter), out var writer),
          Parameter(valueType, out var value),
          isStruct
            ? Block(
                writer.Call(WriteObjectBegin),
                Block(members.Select(member =>
                  writer.CallExt(WriteObjectPropertyOf(member.Type), Constant(member.Name), value.Property(member.Name)))),
                writer.Call(WriteObjectEnd)) as Expression
            : IfThenElse(NotEqual(value, Constant(null)),
                ifTrue: Block(
                  writer.Call(WriteObjectBegin),
                  Block(members.Select(member =>
                    writer.CallExt(WriteObjectPropertyOf(member.Type), Constant(member.Name), value.Property(member.Name)))),
                  writer.Call(WriteObjectEnd)),
                ifFalse: writer.Call(WriteNull)));
      }

      return null;
    }
  }
}
