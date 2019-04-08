// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;

namespace ReactNative.Bridge
{
    static class GenericDelegate
    {
        public static IGenericDelegate Create(MethodInfo method)
        {
            return (IGenericDelegate)Activator.CreateInstance(CreateDelegateType(method, false), method);
        }

        public static IGenericDelegate Create(object instance, MethodInfo method)
        {
            return (IGenericDelegate)Activator.CreateInstance(CreateDelegateType(method, true), instance, method);
        }

        private static Type CreateDelegateType(MethodInfo method, bool withInstance)
        {
            var methodParameters = method.GetParameters();
            var hasReturnValue = method.ReturnType != typeof(void);
            var maximumParameterCount = hasReturnValue ? GenericDelegateTypes.FuncTypes.Length : GenericDelegateTypes.ActionTypes.Length;

            // If the number of arguments exceeds the number supported by generic delegates,
            // return null. This will fall back to using reflection.
            var thisArgumentCount = withInstance ? 0 : 1;
            var argumentCount = methodParameters.Length + thisArgumentCount;
            if (argumentCount > maximumParameterCount)
            {
                return null;
            }

            // Increment the generic arity for methods with return values.
            var genericArity = hasReturnValue ? argumentCount + 1 : argumentCount;
            var typeList = new Type[genericArity];

            // Set the first generic parameter to the declaring type, if necessary
            if (!withInstance)
            {
                typeList[0] = method.DeclaringType;
            }

            // Fill the generic type parameter list with method parameter types.
            for (int t = thisArgumentCount; t < argumentCount; t++)
            {
                typeList[t] = methodParameters[t - thisArgumentCount].ParameterType;
            }

            // If the function has a return value, append it to the generic type parameters.
            if (hasReturnValue)
            {
                typeList[argumentCount] = method.ReturnType;
            }

            // Get the appropriate generic delegate type.
            var genericDelegateType = hasReturnValue
                ? GenericDelegateTypes.FuncTypes[argumentCount]
                : GenericDelegateTypes.ActionTypes[argumentCount];

            // Close the generic type over the argument and return types, if necessary.
            var delegateType = genericDelegateType;
            if (genericArity > 0)
            {
                delegateType = genericDelegateType.MakeGenericType(typeList);
            }

            return delegateType;
        }
    }
}
