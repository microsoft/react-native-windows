using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;

namespace ReactNative.UIManager
{
    static class ViewManagersPropertyCache
    {
        private static readonly IReadOnlyDictionary<string, IPropertySetter> s_shadowEmpty = new Dictionary<string, IPropertySetter>();

        private static readonly ConcurrentDictionary<Type, IReadOnlyDictionary<string, IPropertySetter>> s_settersCache =
            new ConcurrentDictionary<Type, IReadOnlyDictionary<string, IPropertySetter>>();

        public static IReadOnlyDictionary<string, IPropertySetter> GetNativePropertySettersForViewManagerType(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            if (s_settersCache.TryGetValue(type, out var setters))
            {
                return setters;
            }

            var settersImpl = new Dictionary<string, IPropertySetter>();
            var methods = type.GetMethods();
            foreach (var method in methods)
            {
                foreach (var setter in PropertySetter.CreateViewManagerSetters(method))
                {
                    settersImpl.Add(setter.Name, setter);
                }
            }

            // Make sure it works on concurrent accesses that both created a value for same key
            s_settersCache.AddOrUpdate(type, settersImpl, (key, value) => settersImpl);
            return settersImpl;
        }

        public static IReadOnlyDictionary<string, IPropertySetter> GetNativePropertySettersForShadowNodeType(Type type)
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

            var settersImpl = new Dictionary<string, IPropertySetter>();
            var methods = type.GetMethods();
            foreach (var method in methods)
            {
                foreach (var setter in PropertySetter.CreateShadowNodeSetters(method))
                {
                    settersImpl.Add(setter.Name, setter);
                }
            }

            // Make sure it works on concurrent accesses that both created a value for same key
            s_settersCache.AddOrUpdate(type, settersImpl, (key, value) => settersImpl);
            return settersImpl;
        }

        public static IReadOnlyDictionary<string, string> GetNativePropertiesForView(Type viewManagerType, Type shadowNodeType)
        {
            if (viewManagerType == null)
                throw new ArgumentNullException(nameof(viewManagerType));
            if (shadowNodeType == null)
                throw new ArgumentNullException(nameof(shadowNodeType));

            var result = new Dictionary<string, string>();
            var viewManagerProperties = GetNativePropertySettersForViewManagerType(viewManagerType);
            foreach (var pair in viewManagerProperties)
            {
                result[pair.Key] = pair.Value.PropertyType;
            }

            var shadowNodeProperties = GetNativePropertySettersForShadowNodeType(shadowNodeType);
            foreach (var pair in shadowNodeProperties)
            {
                // TODO: Do we want overwrite behavior here?
                // What if the property types do not match?
                result[pair.Key] = pair.Value.PropertyType;
            }

            return result;
        }
    }
}
