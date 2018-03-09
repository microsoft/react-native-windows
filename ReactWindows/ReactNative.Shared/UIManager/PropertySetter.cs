// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Threading;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif
using static System.FormattableString;

namespace ReactNative.UIManager
{
    abstract class PropertySetter : IPropertySetter
    {
        private static HashSet<Type> s_numberTypes =
            new HashSet<Type>
            {
                typeof(sbyte),
                typeof(byte),
                typeof(short),
                typeof(ushort),
                typeof(int),
                typeof(uint),
                typeof(long),
                typeof(ulong),
                typeof(float),
                typeof(double),
                typeof(decimal),
            };

        private static readonly IDictionary<Type, Func<ReactPropBaseAttribute, object>> s_defaultValues =
            new Dictionary<Type, Func<ReactPropBaseAttribute, object>>
            {
                { typeof(sbyte), a => a.DefaultSByte },
                { typeof(byte), a => a.DefaultByte },
                { typeof(short), a => a.DefaultInt16 },
                { typeof(ushort), a => a.DefaultUInt16 },
                { typeof(int), a => a.DefaultInt32 },
                { typeof(uint), a => a.DefaultUInt32 },
                { typeof(long), a => a.DefaultInt64 },
                { typeof(ulong), a => a.DefaultUInt64 },
                { typeof(float), a => a.DefaultSingle },
                { typeof(double), a => a.DefaultDouble },
                { typeof(bool), a => a.DefaultBoolean },
            };

        private readonly IGenericDelegate _genericDelegate;
        private readonly ReactPropBaseAttribute _attribute;
        private readonly string _propertyType;

        protected PropertySetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
        {
            Method = method;
            Name = name;
            PropertyType = GetPropertyType(method);

            _genericDelegate = GenericDelegate.Create(method);

            _propertyType = attribute.CustomType == ReactPropBaseAttribute.UseDefaultType
                ? GetPropertyType(PropertyType)
                : attribute.CustomType;

            _attribute = attribute;
        }

        public string Name { get; }

        protected MethodInfo Method { get; }

        protected Type PropertyType { get; }

        string IPropertySetter.PropertyType
        {
            get
            {
                return _propertyType;
            }
        }

        public void UpdateShadowNodeProperty(ReactShadowNode shadowNode, ReactStylesDiffMap props)
        {
            if (shadowNode == null)
                throw new ArgumentNullException(nameof(shadowNode));
            if (props == null)
                throw new ArgumentNullException(nameof(props));

            Invoke(GetShadowNodeArgs(shadowNode, props));
        }

        public void UpdateViewManagerProperty(IViewManager viewManager, DependencyObject view, ReactStylesDiffMap props)
        {
            if (viewManager == null)
                throw new ArgumentNullException(nameof(viewManager));
            if (props == null)
                throw new ArgumentNullException(nameof(props));

            Invoke(GetViewManagerArgs(viewManager, view, props));
        }

        protected virtual object[] GetShadowNodeArgs(ReactShadowNode shadowNode, ReactStylesDiffMap props)
        {
            throw new NotSupportedException(Invariant($"'{nameof(ReactShadowNode)}' properties cannot be changed with this setter."));
        }

        protected virtual object[] GetViewManagerArgs(IViewManager viewManager, DependencyObject view, ReactStylesDiffMap props)
        {
            throw new NotSupportedException($"'{nameof(IViewManager)}' properties cannot be changed with this setter.");
        }

        protected abstract Type GetPropertyType(MethodInfo method);

        protected virtual void OnInvoked()
        {
        }

        protected object ExtractProperty(ReactStylesDiffMap props)
        {
            if (props.IsNull(Name) && s_defaultValues.TryGetValue(PropertyType, out var defaultFunc))
            {
                return defaultFunc(_attribute);
            }

            var prop = props.GetProperty(Name)?
                .ToObject(PropertyType);

            if (prop == null && PropertyType.GetTypeInfo().IsValueType)
            {
                return Activator.CreateInstance(PropertyType);
            }

            return prop;
        }

        private void Invoke(object[] args)
        {
            try
            {
                _genericDelegate.Invoke(args);
            }
            finally
            {
                OnInvoked();
            }
        }

        private string GetPropertyType(Type propertyType)
        {
            if (s_numberTypes.Contains(propertyType))
            {
                return "number";
            }

            if (propertyType == typeof(bool))
            {
                return "boolean";
            }

            if (propertyType == typeof(string))
            {
                return "String";
            }

            if (propertyType.IsArray)
            {
                return "Array";
            }

            var nullableType = Nullable.GetUnderlyingType(propertyType);
            if (nullableType != null)
            {
                return GetPropertyType(nullableType);
            }

            return "Map";
        }

        public static IEnumerable<IPropertySetter> CreateShadowNodeSetters(MethodInfo method)
        {
            if (method == null)
                throw new ArgumentNullException(nameof(method));

            var reactProp = method.GetCustomAttribute<ReactPropAttribute>();
            var reactPropGroup = default(ReactPropGroupAttribute);
            if (reactProp != null)
            {
                yield return new ShadowNodePropertySetter(method, reactProp.Name, reactProp);
            }
            else if ((reactPropGroup = method.GetCustomAttribute<ReactPropGroupAttribute>()) != null)
            {
                for (var i = 0; i < reactPropGroup.Names.Length; ++i)
                {
                    var name = reactPropGroup.Names[i];
                    yield return new ShadowNodeGroupPropertySetter(method, i, name, reactPropGroup);
                }
            }
        }

        public static IEnumerable<IPropertySetter> CreateViewManagerSetters(MethodInfo method)
        {
            if (method == null)
                throw new ArgumentNullException(nameof(method));

            var reactProp = method.GetCustomAttribute<ReactPropAttribute>();
            var reactPropGroup = default(ReactPropGroupAttribute);
            if (reactProp != null)
            {
                yield return new ViewManagerPropertySetter(method, reactProp.Name, reactProp);
            }
            else if ((reactPropGroup = method.GetCustomAttribute<ReactPropGroupAttribute>()) != null)
            {
                for (var i = 0; i < reactPropGroup.Names.Length; ++i)
                {
                    var name = reactPropGroup.Names[i];
                    yield return new ViewManagerGroupPropertySetter(method, i, name, reactPropGroup);
                }
            }
        }

        class ViewManagerPropertySetter : PropertySetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[3]);

            public ViewManagerPropertySetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
            }

            protected override Type GetPropertyType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 2)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (!typeof(DependencyObject).IsAssignableFrom(parameters[0].ParameterType))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be a dependency object for property setter '{method.DeclaringType.Name}.{Name}'."));
                }

                return parameters[1].ParameterType;
            }

            protected override object[] GetViewManagerArgs(IViewManager viewManager, DependencyObject view, ReactStylesDiffMap props)
            {
                var args = s_args.Value;
                args[0] = viewManager;
                args[1] = view;
                args[2] = ExtractProperty(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 3);
            }
        }

        class ViewManagerGroupPropertySetter : PropertySetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[4]);

            private readonly int _index;

            public ViewManagerGroupPropertySetter(MethodInfo method, int index, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
                _index = index;
            }

            protected override Type GetPropertyType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 3)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for group property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (!typeof(FrameworkElement).IsAssignableFrom(parameters[0].ParameterType))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be a framework element for group property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (parameters[1].ParameterType != typeof(int))
                {
                    throw new InvalidOperationException(
                        Invariant($"Second parameter must be a property index for group property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }


                return parameters[2].ParameterType;
            }

            protected override object[] GetViewManagerArgs(IViewManager viewManager, DependencyObject view, ReactStylesDiffMap props)
            {
                var args = s_args.Value;
                args[0] = viewManager;
                args[1] = view;
                args[2] = _index;
                args[3] = ExtractProperty(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 4);
            }
        }

        class ShadowNodePropertySetter : PropertySetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[2]);

            public ShadowNodePropertySetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
            }

            protected override Type GetPropertyType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 1)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                return  parameters[0].ParameterType;
            }

            protected override object[] GetShadowNodeArgs(ReactShadowNode shadowNode, ReactStylesDiffMap props)
            {
                var args = s_args.Value;
                args[0] = shadowNode;
                args[1] = ExtractProperty(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 2);
            }
        }

        class ShadowNodeGroupPropertySetter : PropertySetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[3]);

            private readonly int _index;

            public ShadowNodeGroupPropertySetter(MethodInfo method, int index, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
                _index = index;
            }

            protected override Type GetPropertyType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 2)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for group property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (parameters[0].ParameterType != typeof(int))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be a property index for group property setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                return parameters[1].ParameterType;
            }

            protected override object[] GetShadowNodeArgs(ReactShadowNode shadowNode, ReactStylesDiffMap props)
            {
                var args = s_args.Value;
                args[0] = shadowNode;
                args[1] = _index;
                args[2] = ExtractProperty(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 3);
            }
        }
    }
}
