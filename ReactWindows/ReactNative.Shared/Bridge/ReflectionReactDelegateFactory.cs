using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using static System.FormattableString;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A delegate factory that uses reflection to create the native method.
    /// </summary>
    public sealed class ReflectionReactDelegateFactory : ReactDelegateFactoryBase
    {
        private ReflectionReactDelegateFactory() { }

        /// <summary>
        /// The <see cref="ReflectionReactDelegateFactory"/> instance.
        /// </summary>
        public static ReflectionReactDelegateFactory Instance { get; } 
            = new ReflectionReactDelegateFactory();

        /// <summary>
        /// Create an invocation delegate from the given method.
        /// </summary>
        /// <param name="module">The native module instance.</param>
        /// <param name="method">The method.</param>
        /// <returns>The invocation delegate.</returns>
        public override Action<IReactInstance, JArray> Create(INativeModule module, MethodInfo method)
        {
            var extractors = CreateExtractors(module, method);
            var expectedArguments = extractors.Sum(e => e.ExpectedArguments);
            var extractFunctions = extractors.Select(e => e.ExtractFunction).ToList();
            var genericDelegate = CreateGenericDelegate(module, method);

            return (reactInstance, arguments) => 
                Invoke(
                    method, 
                    expectedArguments,
                    extractFunctions,
                    genericDelegate,
                    module,
                    reactInstance,
                    arguments);
        }

        private IList<Extractor> CreateExtractors(INativeModule module, MethodInfo method)
        {
            var parameters = method.GetParameters();
            var extractors = new List<Extractor>(parameters.Length);
            foreach (var parameter in parameters)
            {
                extractors.Add(CreateExtractor(parameter.ParameterType, module.Name, method.Name));
            }

            return extractors;
        }

        private Extractor CreateExtractor(Type type, string moduleName, string methodName)
        {
            var exceptionFormat =
                Invariant($"Error extracting argument for module '{moduleName}' method '{methodName}' at index '{{0}}'.");
           
            if (type == typeof(ICallback))
            {
                return new Extractor(
                    1,
                    (reactInstance, arguments, index) =>
                    {
                        try
                        {
                            return new Result(
                                index + 1, 
                                CreateCallback(arguments[index], reactInstance));
                        }
                        catch (Exception ex)
                        {
                            throw new NativeArgumentsParseException(
                                string.Format(exceptionFormat, index),
                                "arguments",
                                ex);
                        }
                    });
            }
            else if (type == typeof(IPromise))
            {
                return new Extractor(
                    2,
                    (reactInstance, arguments, index) =>
                    {
                        var nextIndex = index + 1;
                        if (nextIndex >= arguments.Count)
                        {
                            throw new NativeArgumentsParseException(
                                string.Format(exceptionFormat, index + "' and '" + (index + 1)),
                                "arguments");
                        }

                        try
                        {
                            return new Result(
                                nextIndex + 1,
                                CreatePromise(arguments[index], arguments[nextIndex], reactInstance));
                        }
                        catch (Exception ex)
                        {
                            throw new NativeArgumentsParseException(
                                string.Format(exceptionFormat, index + "' and '" + nextIndex),
                                "arguments",
                                ex);
                        }
                    });
            }
            else
            {
                return new Extractor(
                    1,
                    (reactInstance, arguments, index) =>
                    {
                        try
                        {
                            return new Result(
                                index + 1,
                                arguments[index].ToObject(type));
                        }
                        catch (Exception ex)
                        {
                            throw new NativeArgumentsParseException(
                                string.Format(exceptionFormat, index),
                                "arguments",
                                ex.InnerException);
                        }
                    });
            }
        }

        private static void Invoke(
            MethodInfo method,
            int expectedArguments,
            IList<Func<IReactInstance, JArray, int, Result>> extractors,
            IGenericDelegate genericDelegate,
            INativeModule moduleInstance,
            IReactInstance reactInstance,
            JArray jsArguments)
        {
            if (moduleInstance == null)
                throw new ArgumentNullException(nameof(moduleInstance));
            if (reactInstance == null)
                throw new ArgumentNullException(nameof(reactInstance));
            if (jsArguments == null)
                throw new ArgumentNullException(nameof(jsArguments));

            var n = expectedArguments;
            var c = extractors.Count;
            if (jsArguments.Count != n) 
            {
                throw new NativeArgumentsParseException(
                    Invariant($"Module '{moduleInstance.Name}' method '{method.Name}' got '{jsArguments.Count}' arguments, expected '{n}'."),
                    nameof(jsArguments));
            }

            var idx = 0;
            var args = new object[extractors.Count];
            for (var j = 0; j < c; ++j)
            {
                var result = extractors[j](reactInstance, jsArguments, idx);
                args[j] = result.Value;
                idx = result.NextIndex;
            }

            if (genericDelegate != null)
            {
                genericDelegate.Invoke(args);
            }
            else
            {
                // This should only happen for React methods with greater than 16 arguments.
                method.Invoke(moduleInstance, args);
            }
        }

        static IGenericDelegate CreateGenericDelegate(object instance, MethodInfo method)
        {
            var methodParameters = method.GetParameters();
            var hasReturnValue = method.ReturnType != typeof(void);
            var maximumParameterCount = hasReturnValue ? GenericDelegateTypes.FuncTypes.Length : GenericDelegateTypes.ActionTypes.Length;

            // If the number of arguments exceeds the number supported by generic delegates,
            // return null. This will fall back to using reflection.
            var argumentCount = methodParameters.Length;
            if (argumentCount > maximumParameterCount)
            {
                return null;
            }

            // Increment the generic arity for methods with return values.
            var genericArity = hasReturnValue ? argumentCount + 1 : argumentCount;
            var typeList = new Type[genericArity];

            // Fill the generic type parameter list with method parameter types.
            for (int t = 0; t < argumentCount; t++)
            {
                typeList[t] = methodParameters[t].ParameterType;
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

            // Create the generic delegate instance.
            return (IGenericDelegate)Activator.CreateInstance(delegateType, instance, method);
        }

        private struct Result
        {
            public Result(int nextIndex, object value)
            {
                NextIndex = nextIndex;
                Value = value;
            }

            public int NextIndex { get; }

            public object Value { get; }
        }

        private struct Extractor
        {
            public Extractor(int expectedArguments, Func<IReactInstance, JArray, int, Result> extractFunction)
            {
                ExpectedArguments = expectedArguments;
                ExtractFunction = extractFunction;
            }

            public int ExpectedArguments { get; }
            public Func<IReactInstance, JArray, int, Result> ExtractFunction { get; }
        }
    }
}
