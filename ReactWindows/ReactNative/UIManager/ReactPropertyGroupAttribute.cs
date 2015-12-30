using System;

namespace ReactNative.UIManager
{
    [AttributeUsage(AttributeTargets.Method)]
    public class ReactPropertyGroupAttribute : Attribute
    {
        public ReactPropertyGroupAttribute(params string[] names)
        {
            Names = names;
        }

        public string[] Names { get; }
    }
}
