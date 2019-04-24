// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;

namespace ReactNative.UIManager
{
    static class ViewManagersPropCache
    {
        private static readonly IReadOnlyDictionary<string, IPropSetter> s_shadowEmpty = new Dictionary<string, IPropSetter>();

        private static readonly ConcurrentDictionary<Type, IReadOnlyDictionary<string, IPropSetter>> s_settersCache =
            new ConcurrentDictionary<Type, IReadOnlyDictionary<string, IPropSetter>>();

        public static IReadOnlyDictionary<string, IPropSetter> GetNativePropSettersForViewManagerType<TView>(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            if (s_settersCache.TryGetValue(type, out var setters))
            {
                return setters;
            }

            var settersImpl = new Dictionary<string, IPropSetter>();
            var methods = type.GetMethods();
            foreach (var method in methods)
            {
                foreach (var setter in PropSetter.CreateViewManagerSetters<TView>(method))
                {
                    settersImpl.Add(setter.Name, setter);
                }
            }

            // Make sure it works on concurrent accesses that both created a value for same key
            s_settersCache.AddOrUpdate(type, settersImpl, (key, value) => settersImpl);
            return settersImpl;
        }

        public static IReadOnlyDictionary<string, IPropSetter> GetNativePropSettersForShadowNodeType(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            if (type == typeof(ReactShadowNode))
            {
                return s_shadowEmpty;
            }

            if (s_settersCache.TryGetValue(type, out var setters))
            {
                return setters;
            }

            var settersImpl = new Dictionary<string, IPropSetter>();
            var methods = type.GetMethods();
            foreach (var method in methods)
            {
                foreach (var setter in PropSetter.CreateShadowNodeSetters(method))
                {
                    settersImpl.Add(setter.Name, setter);
                }
            }

            // Make sure it works on concurrent accesses that both created a value for same key
            s_settersCache.AddOrUpdate(type, settersImpl, (key, value) => settersImpl);
            return settersImpl;
        }

        public static JObject GetNativePropsForView<TView>(Type viewManagerType, Type shadowNodeType)
        {
            if (viewManagerType == null)
                throw new ArgumentNullException(nameof(viewManagerType));
            if (shadowNodeType == null)
                throw new ArgumentNullException(nameof(shadowNodeType));

            var result = new JObject();
            var viewManagerProps = GetNativePropSettersForViewManagerType<TView>(viewManagerType);
            foreach (var pair in viewManagerProps)
            {
                result[pair.Key] = pair.Value.PropType;
            }

            var shadowNodeProps = GetNativePropSettersForShadowNodeType(shadowNodeType);
            foreach (var pair in shadowNodeProps)
            {
                // TODO: Do we want overwrite behavior here?
                // What if the prop types do not match?
                result[pair.Key] = pair.Value.PropType;
            }

            return result;
        }
    }
}
