
using System;

namespace ReactNative.UIManager
{
    [AttributeUsage(AttributeTargets.Method)]
    public class ReactPropertyAttribute : Attribute
    {
        public const string UseDefaultType = "__default_type__";

        /// <summary>
        /// Instantiates the <see cref="ReactPropertyAttribute"/>.
        /// </summary>
        /// <param name="name">The property name.</param>
        public ReactPropertyAttribute(string name)
        {
            Name = name;
        }

        /// <summary>
        /// Name of the property exposed to JavaScript.
        /// </summary>
        public string Name { get; }

        /// <summary>
        /// The custom type name that should be sent to JavaScript.
        /// </summary>
        public virtual string CustomType
        {
            get
            {
                return UseDefaultType;
            }
        }
    }
}
