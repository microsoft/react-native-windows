// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Json;
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
    abstract class PropSetter : IPropSetter
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
        private readonly string _propType;

        protected PropSetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
        {
            Method = method;
            Name = name;
            PropType = GetPropType(method);

            _genericDelegate = GenericDelegate.Create(method);

            _propType = attribute.CustomType == ReactPropBaseAttribute.UseDefaultType
                ? GetPropType(PropType)
                : attribute.CustomType;

            _attribute = attribute;
        }

        public string Name { get; }

        protected MethodInfo Method { get; }

        protected Type PropType { get; }

        string IPropSetter.PropType
        {
            get
            {
                return _propType;
            }
        }

        public void UpdateShadowNodeProp(ReactShadowNode shadowNode, JObject props)
        {
            if (shadowNode == null)
                throw new ArgumentNullException(nameof(shadowNode));
            if (props == null)
                throw new ArgumentNullException(nameof(props));

            Invoke(GetShadowNodeArgs(shadowNode, props));
        }

        public void UpdateViewManagerProp(IViewManager viewManager, object view, JObject props)
        {
            if (viewManager == null)
                throw new ArgumentNullException(nameof(viewManager));
            if (props == null)
                throw new ArgumentNullException(nameof(props));

            Invoke(GetViewManagerArgs(viewManager, view, props));
        }

        protected virtual object[] GetShadowNodeArgs(ReactShadowNode shadowNode, JObject props)
        {
            throw new NotSupportedException(Invariant($"'{nameof(ReactShadowNode)}' props cannot be changed with this setter."));
        }

        protected virtual object[] GetViewManagerArgs(IViewManager viewManager, object view, JObject props)
        {
            throw new NotSupportedException($"'{nameof(IViewManager)}' props cannot be changed with this setter.");
        }

        protected abstract Type GetPropType(MethodInfo method);

        protected virtual void OnInvoked()
        {
        }

        protected object ExtractProp(JObject props)
        {
            if (props.IsNull(Name) && s_defaultValues.TryGetValue(PropType, out var defaultFunc))
            {
                return defaultFunc(_attribute);
            }

            var prop = props.GetProperty(Name)?
                .ToObject(PropType);

            if (prop == null && PropType.GetTypeInfo().IsValueType)
            {
                return Activator.CreateInstance(PropType);
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

        private string GetPropType(Type propType)
        {
            if (s_numberTypes.Contains(propType))
            {
                return "number";
            }

            if (propType == typeof(bool))
            {
                return "boolean";
            }

            if (propType == typeof(string))
            {
                return "String";
            }

            if (propType.IsArray)
            {
                return "Array";
            }

            var nullableType = Nullable.GetUnderlyingType(propType);
            if (nullableType != null)
            {
                return GetPropType(nullableType);
            }

            return "Map";
        }

        public static IEnumerable<IPropSetter> CreateShadowNodeSetters(MethodInfo method)
        {
            if (method == null)
                throw new ArgumentNullException(nameof(method));

            var reactProp = method.GetCustomAttribute<ReactPropAttribute>();
            var reactPropGroup = default(ReactPropGroupAttribute);
            if (reactProp != null)
            {
                yield return new ShadowNodePropSetter(method, reactProp.Name, reactProp);
            }
            else if ((reactPropGroup = method.GetCustomAttribute<ReactPropGroupAttribute>()) != null)
            {
                for (var i = 0; i < reactPropGroup.Names.Length; ++i)
                {
                    var name = reactPropGroup.Names[i];
                    yield return new ShadowNodeGroupPropSetter(method, i, name, reactPropGroup);
                }
            }
        }

        public static IEnumerable<IPropSetter> CreateViewManagerSetters<TView>(MethodInfo method)
        {
            if (method == null)
                throw new ArgumentNullException(nameof(method));

            var reactProp = method.GetCustomAttribute<ReactPropAttribute>();
            var reactPropGroup = default(ReactPropGroupAttribute);
            if (reactProp != null)
            {
                yield return new ViewManagerPropSetter<TView>(
                    method,
                    reactProp.Name,
                    reactProp);
            }
            else if ((reactPropGroup = method.GetCustomAttribute<ReactPropGroupAttribute>()) != null)
            {
                for (var i = 0; i < reactPropGroup.Names.Length; ++i)
                {
                    var name = reactPropGroup.Names[i];
                    yield return new ViewManagerGroupPropSetter<TView>(
                        method,
                        i,
                        name,
                        reactPropGroup);
                }
            }
        }

        class ViewManagerPropSetter<TView> : PropSetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[3]);

            public ViewManagerPropSetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
            }

            protected override Type GetPropType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 2)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (!typeof(TView).IsAssignableFrom(parameters[0].ParameterType))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be of type '{typeof(TView)}' for prop setter '{method.DeclaringType.Name}.{Name}'."));
                }

                return parameters[1].ParameterType;
            }

            protected override object[] GetViewManagerArgs(IViewManager viewManager, object view, JObject props)
            {
                var args = s_args.Value;
                args[0] = viewManager;
                args[1] = view;
                args[2] = ExtractProp(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 3);
            }
        }

        class ViewManagerGroupPropSetter<TView> : PropSetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[4]);

            private readonly int _index;

            public ViewManagerGroupPropSetter(MethodInfo method, int index, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
                _index = index;
            }

            protected override Type GetPropType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 3)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for group prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (!typeof(TView).IsAssignableFrom(parameters[0].ParameterType))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be of type '{typeof(TView)}' for group prop setter '{method.DeclaringType.Name}.{Name}'."));
                }

                if (parameters[1].ParameterType != typeof(int))
                {
                    throw new InvalidOperationException(
                        Invariant($"Second parameter must be a prop index for group prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }


                return parameters[2].ParameterType;
            }

            protected override object[] GetViewManagerArgs(IViewManager viewManager, object view, JObject props)
            {
                var args = s_args.Value;
                args[0] = viewManager;
                args[1] = view;
                args[2] = _index;
                args[3] = ExtractProp(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 4);
            }
        }

        class ShadowNodePropSetter : PropSetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[2]);

            public ShadowNodePropSetter(MethodInfo method, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
            }

            protected override Type GetPropType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 1)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                return  parameters[0].ParameterType;
            }

            protected override object[] GetShadowNodeArgs(ReactShadowNode shadowNode, JObject props)
            {
                var args = s_args.Value;
                args[0] = shadowNode;
                args[1] = ExtractProp(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 2);
            }
        }

        class ShadowNodeGroupPropSetter : PropSetter
        {
            private static ThreadLocal<object[]> s_args = new ThreadLocal<object[]>(() => new object[3]);

            private readonly int _index;

            public ShadowNodeGroupPropSetter(MethodInfo method, int index, string name, ReactPropBaseAttribute attribute)
                : base(method, name, attribute)
            {
                _index = index;
            }

            protected override Type GetPropType(MethodInfo method)
            {
                var parameters = method.GetParameters();
                if (parameters.Length != 2)
                {
                    throw new InvalidOperationException(
                        Invariant($"Wrong number of arguments for group prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                if (parameters[0].ParameterType != typeof(int))
                {
                    throw new InvalidOperationException(
                        Invariant($"First parameter must be a prop index for group prop setter '{method.DeclaringType.Name}.{method.Name}'."));
                }

                return parameters[1].ParameterType;
            }

            protected override object[] GetShadowNodeArgs(ReactShadowNode shadowNode, JObject props)
            {
                var args = s_args.Value;
                args[0] = shadowNode;
                args[1] = _index;
                args[2] = ExtractProp(props);
                return args;
            }

            protected override void OnInvoked()
            {
                Array.Clear(s_args.Value, 0, 3);
            }
        }
    }
}
