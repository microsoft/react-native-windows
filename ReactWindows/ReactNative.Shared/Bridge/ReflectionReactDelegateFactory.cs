// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
        public override Func<INativeModule, IReactInstance, JArray, JToken> Create(INativeModule module, MethodInfo method)
        {
            ReactMethodAttribute attribute = method.GetCustomAttribute<ReactMethodAttribute>();
            if (attribute != null && attribute.IsDirectCallMethod)
            {

                return (Func<INativeModule, IReactInstance, JArray, JToken>)method.CreateDelegate(typeof(Func<INativeModule, IReactInstance, JArray, JToken>));
            }

            var extractors = CreateExtractors(module, method);
            var expectedArguments = extractors.Sum(e => e.ExpectedArguments);
            var extractFunctions = extractors.Select(e => e.ExtractFunction).ToList();
            var genericDelegate = GenericDelegate.Create(module, method);

            return (moduleInstance, reactInstance, arguments) => 
                Invoke(
                    method, 
                    expectedArguments,
                    extractFunctions,
                    genericDelegate,
                    moduleInstance,
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

        private static JToken Invoke(
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
                var extractorResult = extractors[j](reactInstance, jsArguments, idx);
                args[j] = extractorResult.Value;
                idx = extractorResult.NextIndex;
            }

            var hasDelegate = (genericDelegate != null) && ReferenceEquals(moduleInstance, genericDelegate.DelegateTarget);
            if (method.ReturnType == typeof(void))
            {
                if (hasDelegate)
                {
                    genericDelegate.Invoke(args);
                }
                else
                {
                    // This should only happen for React methods with greater than 16 arguments.
                    method.Invoke(moduleInstance, args);
                }

                return null;
            }

            var result = hasDelegate ? genericDelegate.Invoke(args) : method.Invoke(moduleInstance, args);
            if (result == null)
            {
                return new JValue((object)null);
            }

            return JToken.FromObject(result);
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
