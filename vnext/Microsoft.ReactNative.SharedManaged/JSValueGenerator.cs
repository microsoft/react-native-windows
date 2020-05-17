// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  static class JSValueGenerator
  {
    // Compare two types by putting more specific types before more generic.
    // While we use it to compare types with the same generic type base,
    // we do more thorough comparison because the same method is called
    // recursively for the generic type arguments.
    public class GenericTypeComparer : IComparer<Type>
    {
      public static readonly GenericTypeComparer Default = new GenericTypeComparer();

      public int Compare(Type x, Type y)
      {
        var xTypeInfo = x.GetTypeInfo();
        var yTypeInfo = y.GetTypeInfo();

        // Generic parameters are less specific and must appear after other types. E.g. string before T.
        int result = Comparer<bool>.Default.Compare(x.IsGenericParameter, y.IsGenericParameter);
        if (result != 0) return result;

        // Compare generic parameters. E.g. T vs U. We use default type order.
        if (x.IsGenericParameter) return Comparer<Type>.Default.Compare(x, y);

        // We consider arrays to be more specific than non-arrays.
        // Note the minus '-' sign to reverse order.
        result = -Comparer<bool>.Default.Compare(x.IsArray, y.IsArray);
        if (result != 0) return result;

        // Compare arrays by their element types.
        if (x.IsArray) return Compare(x.GetElementType(), y.GetElementType());

        // Generic types are more specific and must appear before non-generic types.
        // E.g. IDictionary<T, U> before IDictionary. Note the minus '-' sign to reverse order.
        result = -Comparer<bool>.Default.Compare(xTypeInfo.IsGenericType, yTypeInfo.IsGenericType);
        if (result != 0) return result;

        // Compare non-generic types. E.g string vs int. We use default type order.
        if (!xTypeInfo.IsGenericType) return Comparer<Type>.Default.Compare(x, y);

        // We consider types with more generic parameters to be more specific than types with less generic parameters.
        // E.g. we want to match IDictionary<string, T> before IList<KeyValuePair<string, T>>.
        var xArgs = x.GetGenericArguments();
        var yArgs = y.GetGenericArguments();
        // Note minus sign '-' to order integers in reverse order. E.g. 7 before 5.
        result = -Comparer<int>.Default.Compare(xArgs.Length, yArgs.Length);
        if (result != 0) return result;

        // If number of generic arguments is the same, then we use the order generic type definitions.
        // E.g. List<> vs IList<>.
        result = Comparer<Type>.Default.Compare(xTypeInfo.GetGenericTypeDefinition(), yTypeInfo.GetGenericTypeDefinition());
        if (result != 0) return result;

        // We have the same generic type definitions. Recursively compare their generic arguments.
        for (int i = 0; i < xArgs.Length; ++i)
        {
          result = Compare(xArgs[i], yArgs[i]);
          if (result != 0) return result;
        }

        return 0;
      }
    }

    // Try to match type to pattern with patternArgs.
    // If successful return matchedArgs where each generic parameter T from patternArgs has a real type.
    public static bool TryMatchGenericType(Type type, Type pattern, Type[] patternArgs, out Type[] matchedArgs)
    {
      matchedArgs = null;
      var genericBindings = new Dictionary<Type, Type>(patternArgs.Length);

      // This local function is going to be called recursively for generic type arguments.
      bool MatchType(Type testType, Type patternType)
      {
        if (testType == patternType) return true;

        // Match array types
        if (testType.IsArray != patternType.IsArray) return false;
        if (testType.IsArray && patternType.IsArray)
        {
          return MatchType(testType.GetElementType(), patternType.GetElementType());
        }

        // Match testType to generic parameter type such as T.
        if (patternType.IsGenericParameter)
        {
          if (genericBindings.TryGetValue(patternType, out var existingBinding))
          {
            return testType == existingBinding;
          }
          else
          {
            genericBindings.Add(patternType, testType);
            return true;
          }
        }

        // Match generic types
        var testTypeInfo = testType.GetTypeInfo();
        var patternTypeInfo = patternType.GetTypeInfo();
        if (testTypeInfo.IsGenericType && patternTypeInfo.IsGenericType)
        {
          Type[] testGenericArgs = testType.GetGenericArguments();
          Type[] patternGenericArgs = pattern.GetGenericArguments();
          if (testGenericArgs.Length == patternGenericArgs.Length)
          {
            for (int i = 0; i < testGenericArgs.Length; ++i)
            {
              if (!MatchType(testGenericArgs[i], patternGenericArgs[i]))
              {
                return false;
              }
            }

            return true;
          }
        }

        return false;
      }

      if (!MatchType(type, pattern)) return false;

      if (patternArgs.Length != genericBindings.Count) return false;

      // Check generic constraints
      foreach (var genericArg in patternArgs)
      {
        // base class and interface constraints
        var baseTypeConstraints = genericArg.GetTypeInfo().GetGenericParameterConstraints();
        if (baseTypeConstraints.Length > 0)
        {
          var boundType = genericBindings[genericArg];
          foreach (var baseType in baseTypeConstraints)
          {
            // TODO: what if baseType is based on a generic parameter? E.g. 'where T : U'
            if (!boundType.GetTypeInfo().IsSubclassOf(baseType))
            {
              return false;
            }
          }
        }

        // TODO: Consider to add checks for generic parameter attributes: t.GenericParameterAttributes
      }

      matchedArgs = new Type[patternArgs.Length];
      for (int i = 0; i < matchedArgs.Length; ++i)
      {
        matchedArgs[i] = genericBindings[patternArgs[i]];
      }

      return true;
    }

    public class VariableWrapper
    {
      public static VariableWrapper CreateVariable(Type type, Expression init)
      {
        return new VariableWrapper
        {
          Type = type,
          AsExpression = Expression.Variable(type),
          Init = init,
          IsParameter = false
        };
      }

      public static VariableWrapper CreateVariable(Type type)
      {
        return new VariableWrapper
        {
          Type = type,
          AsExpression = Expression.Variable(type),
          Init = null,
          IsParameter = false
        };
      }

      public static VariableWrapper CreateParameter(Type type)
      {
        return new VariableWrapper
        {
          Type = type,
          AsExpression = Expression.Parameter(type),
          IsParameter = true
        };
      }

      public bool IsParameter { get; private set; }

      public Expression Init { get; private set; }

      public Type Type { get; private set; }

      public ParameterExpression AsExpression { get; private set; }

      public static implicit operator ParameterExpression(VariableWrapper v) => v.AsExpression;

      public Expression Assign(Expression value)
      {
        return Expression.Assign(AsExpression, value);
      }

      // This method allows us to expand the argument array that may use parameters that are
      // Expressions, VariableWrappers, or arrays of them.
      // The argument expressions are added to the args list.
      private void ExpandArgArray(IList<Expression> args, object[] argObjects)
      {
        foreach (var arg in argObjects)
        {
          switch (arg)
          {
            case object[] items: ExpandArgArray(args, items); break;
            case VariableWrapper variable: args.Add(variable.AsExpression); break;
            case Expression expr: args.Add(expr); break;
          }
        }
      }

      public MethodCallExpression Call(MethodInfo method, params object[] arguments)
      {
        var args = new List<Expression>();
        ExpandArgArray(args, arguments);
        return Expression.Call(AsExpression, method, args);
      }

      public MethodCallExpression CallExt(MethodInfo method, params object[] arguments)
      {
        var args = new List<Expression> { AsExpression };
        ExpandArgArray(args, arguments);
        return Expression.Call(method, args);
      }

      // It can be used only for delegate types
      public Expression Invoke(params object[] arguments)
      {
        var args = new List<Expression>();
        ExpandArgArray(args, arguments);
        return Expression.Invoke(AsExpression, args);
      }

      public Expression Property(string propertyName)
      {
        return PropertyOrField(AsExpression, propertyName);
      }

      public Expression Property(PropertyInfo propertyInfo)
      {
        return Expression.Property(propertyInfo.GetGetMethod().IsStatic ? null : AsExpression, propertyInfo);
      }

      public Expression SetProperty(string propertyName, Expression value)
      {
        return Expression.Assign(Property(propertyName), value);
      }

      public Expression SetPropertyStatement(string propertyName, Expression value)
      {
        return Block(SetProperty(propertyName, value), Default(typeof(void)));
      }

      public Expression SetProperty(PropertyInfo propertyInfo, Expression value)
      {
        return Expression.Assign(Property(propertyInfo), value);
      }

      public Expression CastTo(Type type)
      {
        return Convert(AsExpression, type);
      }
    }

    public class TypeWrapper
    {
      public TypeWrapper(Type type)
      {
        Type = type;
      }

      public Type Type { get; private set; }

      public Delegate CompileLambda(params object[] expressions)
      {
        return AutoLambda(Type, expressions).Compile();
      }

      public static implicit operator Type(TypeWrapper wrapper) => wrapper.Type;
    }

    public static VariableWrapper Variable(Type type, out VariableWrapper variable, Expression init = null)
    {
      return variable = VariableWrapper.CreateVariable(type, init);
    }

    public static VariableWrapper Parameter(Type type, out VariableWrapper parameter)
    {
      return parameter = VariableWrapper.CreateParameter(type);
    }

    public static VariableWrapper[] Parameters(Type[] types, out VariableWrapper[] parameters)
    {
      return parameters = types.Select(t => VariableWrapper.CreateParameter(t)).ToArray();
    }

    public static BlockExpression AutoBlock(params object[] expressions)
    {
      ParseExpressions(expressions, out var body, out _, out var variables);
      return Block(variables, body);
    }

    public static LambdaExpression AutoLambda(Type delegateType, params object[] expressions)
    {
      ParseExpressions(expressions, out var body, out var parameters, out var variables);

      var lambdaBody = (body.Length == 1 && variables.Length == 0)
        ? body[0]
        : Block(variables, body);

      return Lambda(delegateType, lambdaBody, parameters);
    }

    public static LambdaExpression AutoLambda<TDelegate>(params object[] expressions)
    {
      return AutoLambda(typeof(TDelegate), expressions);
    }

    private static void ParseExpressions(object[] expressions,
      out Expression[] body, out ParameterExpression[] parameters, out ParameterExpression[] variables)
    {
      var bodyList = new List<Expression>();
      var parameterList = new List<ParameterExpression>();
      var variableList = new List<ParameterExpression>();

      void ParseArray(object[] exprs)
      {
        foreach (var expr in exprs)
        {
          switch (expr)
          {
            case VariableWrapper parameter when parameter.IsParameter:
              parameterList.Add(parameter);
              break;
            case VariableWrapper variable when !variable.IsParameter:
              variableList.Add(variable);
              if (variable.Init != null)
              {
                bodyList.Add(variable.Assign(variable.Init));
              }
              break;
            case Expression expression:
              bodyList.Add(expression);
              break;
            case object[] items:
              ParseArray(items);
              break;
          }
        }
      }

      ParseArray(expressions);
      body = bodyList.ToArray();
      parameters = parameterList.ToArray();
      variables = variableList.ToArray();
    }

    public static Expression While(Expression condition, Expression body)
    {
      // A label to jump to from a loop.
      LabelTarget breakLabel = Label(typeof(void));

      // Execute loop while condition is true.
      return Loop(IfThenElse(condition, body, Break(breakLabel)), breakLabel);
    }

    public static MethodCallExpression Call(this Expression instance, MethodInfo method, params object[] arguments)
    {
      var args = new List<Expression>();

      void ParseArgs(object[] argObjects)
      {
        foreach (var arg in argObjects)
        {
          switch (arg)
          {
            case object[] items: ParseArgs(items); break;
            case VariableWrapper variable: args.Add(variable.AsExpression); break;
            case Expression expr: args.Add(expr); break;
          }
        }
      }

      ParseArgs(arguments);
      return Expression.Call(method.IsStatic ? null : instance, method, args);
    }

    public static Expression SetField(this Expression instance, FieldInfo fieldInfo, Expression value)
    {
      return Assign(Field(fieldInfo.IsStatic ? null : instance, fieldInfo), value);
    }

    public static Expression SetProperty(this Expression instance, PropertyInfo propertyInfo, Expression value)
    {
      return Assign(Property(propertyInfo.GetSetMethod().IsStatic ? null : instance, propertyInfo), value);
    }

    public static TDelegate CompileLambda<TDelegate>(params object[] expressions) /*TODO: add in C# v7.3: where TDelegate : Delegate*/
    {
      var typeWrapper = new TypeWrapper(typeof(TDelegate));
      return (TDelegate)(object)typeWrapper.CompileLambda(expressions);
    }

    public static VariableWrapper[] MethodArgs(
      ParameterInfo[] parameters,
      out Type[] argTypes,
      out VariableWrapper[] args)
    {
      argTypes = parameters.Select(p => p.ParameterType).ToArray();
      args = argTypes.Select(t => Variable(t, out _)).ToArray();
      return args;
    }

    public static VariableWrapper[] MethodArgs(
      ParameterInfo[] parameters,
      out Type[] argTypes,
      out VariableWrapper[] args,
      out Type promiseResultType)
    {
      argTypes = parameters.Take(parameters.Length - 1).Select(p => p.ParameterType).ToArray();
      args = argTypes.Select(t => Variable(t, out _)).ToArray();
      promiseResultType = parameters[parameters.Length - 1].ParameterType.GetGenericArguments()[0];
      return args;
    }

    public static VariableWrapper[] MethodArgs(
      ParameterInfo[] parameters,
      out Type[] argTypes,
      out VariableWrapper[] args,
      out Type resolveCallbackType,
      out Type[] resolveArgTypes)
    {
      argTypes = parameters.Take(parameters.Length - 1).Select(p => p.ParameterType).ToArray();
      args = argTypes.Select(t => Variable(t, out _)).ToArray();
      resolveCallbackType = parameters[parameters.Length - 1].ParameterType;
      resolveArgTypes = resolveCallbackType.GetMethod("Invoke").GetParameters().Select(p => p.ParameterType).ToArray();
      return args;
    }

    public static VariableWrapper[] MethodArgs(
      ParameterInfo[] parameters,
      out Type[] argTypes,
      out VariableWrapper[] args,
      out Type resolveCallbackType,
      out Type[] resolveArgTypes,
      out Type rejectCallbackType,
      out Type[] rejectArgTypes)
    {
      argTypes = parameters.Take(parameters.Length - 2).Select(p => p.ParameterType).ToArray();
      args = argTypes.Select(t => Variable(t, out _)).ToArray();
      resolveCallbackType = parameters[parameters.Length - 2].ParameterType;
      resolveArgTypes = resolveCallbackType.GetMethod("Invoke").GetParameters().Select(p => p.ParameterType).ToArray();
      rejectCallbackType = parameters[parameters.Length - 1].ParameterType;
      rejectArgTypes = rejectCallbackType.GetMethod("Invoke").GetParameters().Select(p => p.ParameterType).ToArray();
      return args;
    }

    public static TypeWrapper ActionOf(params Type[] argTypes)
    {
      switch (argTypes.Length)
      {
        case 1: return new TypeWrapper(typeof(Action<>).MakeGenericType(argTypes));
        default: throw new NotImplementedException($"Not supported argTypes count: {argTypes.Length}");
      }
    }

    public static TypeWrapper ActionOf<T1>() => ActionOf(typeof(T1));
  }
}
