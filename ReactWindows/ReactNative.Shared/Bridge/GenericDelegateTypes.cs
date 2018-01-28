using System;
using System.Reflection;

namespace ReactNative.Bridge
{
    internal class SignatureAction0 : IGenericDelegate
    {
        Action _instancedDelegate;

        public SignatureAction0(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action)method.CreateDelegate(typeof(Action), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate();
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction1<T0> : IGenericDelegate
    {
        Action<T0> _instancedDelegate;

        public SignatureAction1(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0>)method.CreateDelegate(typeof(Action<T0>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction2<T0, T1> : IGenericDelegate
    {
        Action<T0, T1> _instancedDelegate;

        public SignatureAction2(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1>)method.CreateDelegate(typeof(Action<T0, T1>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction3<T0, T1, T2> : IGenericDelegate
    {
        Action<T0, T1, T2> _instancedDelegate;

        public SignatureAction3(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2>)method.CreateDelegate(typeof(Action<T0, T1, T2>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction4<T0, T1, T2, T3> : IGenericDelegate
    {
        Action<T0, T1, T2, T3> _instancedDelegate;

        public SignatureAction4(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction5<T0, T1, T2, T3, T4> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4> _instancedDelegate;

        public SignatureAction5(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction6<T0, T1, T2, T3, T4, T5> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4, T5> _instancedDelegate;

        public SignatureAction6(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4, T5>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4, T5>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction7<T0, T1, T2, T3, T4, T5, T6> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4, T5, T6> _instancedDelegate;

        public SignatureAction7(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4, T5, T6>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4, T5, T6>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction8<T0, T1, T2, T3, T4, T5, T6, T7> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4, T5, T6, T7> _instancedDelegate;

        public SignatureAction8(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4, T5, T6, T7>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4, T5, T6, T7>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6], (T7)args[7]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction9<T0, T1, T2, T3, T4, T5, T6, T7, T8> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4, T5, T6, T7, T8> _instancedDelegate;

        public SignatureAction9(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4, T5, T6, T7, T8>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4, T5, T6, T7, T8>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6], (T7)args[7], (T8)args[8]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureAction10<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> : IGenericDelegate
    {
        Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> _instancedDelegate;

        public SignatureAction10(object instance, MethodInfo method)
        {
            _instancedDelegate = (Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>)method.CreateDelegate(typeof(Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>), instance);
        }

        public object Invoke(object[] args)
        {
            _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6], (T7)args[7], (T8)args[8], (T9)args[9]);
            return null;
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc1<T0> : IGenericDelegate
    {
        Func<T0> _instancedDelegate;

        public SignatureFunc1(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0>)method.CreateDelegate(typeof(Func<T0>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate();
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc2<T0, T1> : IGenericDelegate
    {
        Func<T0, T1> _instancedDelegate;

        public SignatureFunc2(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1>)method.CreateDelegate(typeof(Func<T0, T1>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc3<T0, T1, T2> : IGenericDelegate
    {
        Func<T0, T1, T2> _instancedDelegate;

        public SignatureFunc3(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2>)method.CreateDelegate(typeof(Func<T0, T1, T2>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc4<T0, T1, T2, T3> : IGenericDelegate
    {
        Func<T0, T1, T2, T3> _instancedDelegate;

        public SignatureFunc4(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc5<T0, T1, T2, T3, T4> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4> _instancedDelegate;

        public SignatureFunc5(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc6<T0, T1, T2, T3, T4, T5> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4, T5> _instancedDelegate;

        public SignatureFunc6(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4, T5>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4, T5>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc7<T0, T1, T2, T3, T4, T5, T6> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4, T5, T6> _instancedDelegate;

        public SignatureFunc7(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4, T5, T6>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4, T5, T6>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc8<T0, T1, T2, T3, T4, T5, T6, T7> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4, T5, T6, T7> _instancedDelegate;

        public SignatureFunc8(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4, T5, T6, T7>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4, T5, T6, T7>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc9<T0, T1, T2, T3, T4, T5, T6, T7, T8> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4, T5, T6, T7, T8> _instancedDelegate;

        public SignatureFunc9(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4, T5, T6, T7, T8>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4, T5, T6, T7, T8>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6], (T7)args[7]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    internal class SignatureFunc10<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> : IGenericDelegate
    {
        Func<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> _instancedDelegate;

        public SignatureFunc10(object instance, MethodInfo method)
        {
            _instancedDelegate = (Func<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>)method.CreateDelegate(typeof(Func<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>), instance);
        }

        public object Invoke(object[] args)
        {
            return _instancedDelegate((T0)args[0], (T1)args[1], (T2)args[2], (T3)args[3], (T4)args[4], (T5)args[5], (T6)args[6], (T7)args[7], (T8)args[8]);
        }

        public object DelegateTarget
        {
            get
            {
                return _instancedDelegate.Target;
            }
        }
    }

    public static class TypesTemplateAction
    {
        public static Type[] TemplateType =
        {
            typeof(SignatureAction0),
            typeof(SignatureAction1<>),
            typeof(SignatureAction2<,>),
            typeof(SignatureAction3<,,>),
            typeof(SignatureAction4<,,,>),
            typeof(SignatureAction5<,,,,>),
            typeof(SignatureAction6<,,,,,>),
            typeof(SignatureAction7<,,,,,,>),
            typeof(SignatureAction8<,,,,,,,>),
            typeof(SignatureAction9<,,,,,,,,>),
            typeof(SignatureAction10<,,,,,,,,,>),
        };
    }

    public static class TypesTemplateFunc
    {
        public static Type[] TemplateType =
        {
            typeof(SignatureFunc1<>),
            typeof(SignatureFunc2<,>),
            typeof(SignatureFunc3<,,>),
            typeof(SignatureFunc4<,,,>),
            typeof(SignatureFunc5<,,,,>),
            typeof(SignatureFunc6<,,,,,>),
            typeof(SignatureFunc7<,,,,,,>),
            typeof(SignatureFunc8<,,,,,,,>),
            typeof(SignatureFunc9<,,,,,,,,>),
            typeof(SignatureFunc10<,,,,,,,,,>),
        };
    }
}
