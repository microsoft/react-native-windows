using System;
using System.Reflection;

namespace ReactNative.Bridge
{
    public interface IGenericDelegate
    {
        object Invoke(object[] args);

        object DelegateTarget
        {
            get;
        }
    }

    public static class GenericDelegateFactory
    {
        static IGenericDelegate CreateNewTypeWrapper(Type[] typeList, object instance, MethodInfo method, bool isFunc)
        {
            var numArgs = typeList.Length;
            if (numArgs == 0)
            {
                return new SignatureAction0(instance, method);
            }

            Type actionT;
            if (isFunc)
            {
                actionT = TypesTemplateFunc.TemplateType[numArgs - 1].MakeGenericType(typeList);
            }
            else
            {
                actionT = TypesTemplateAction.TemplateType[numArgs].MakeGenericType(typeList);
            }

            return (IGenericDelegate)Activator.CreateInstance(actionT, instance, method);
        }

        public static IGenericDelegate CreateGenericDelegate(object instance, MethodInfo method)
        {
            var methodParameters = method.GetParameters();
            var hasReturnValue = method.ReturnType != typeof(void);
            var maximumParameterCount = hasReturnValue ? TypesTemplateFunc.TemplateType.Length : TypesTemplateAction.TemplateType.Length;
            if (methodParameters.Length > maximumParameterCount - 1)
            {
                return null;
            }

            var parameterCount = hasReturnValue ? methodParameters.Length + 1 : methodParameters.Length;
            var typeList = new Type[parameterCount];
            for (int t = 0; t < methodParameters.Length; t++)
            {
                typeList[t] = methodParameters[t].ParameterType;
            }

            if (hasReturnValue)
            {
                typeList[methodParameters.Length] = method.ReturnType;
            }

            return CreateNewTypeWrapper(typeList, instance, method, hasReturnValue);
        }
    }
}
