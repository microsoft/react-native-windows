// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

namespace ReactNative.UIManager.Annotations
{
    /// <summary>
    /// An annotation for props of native views that should be exposed to
    /// JavaScript.
    /// </summary>
    /// <remarks>
    /// Each annotated method should return <see cref="void"/>.
    /// 
    /// In cases when the prop has been removed from the corresponding 
    /// React component, the annotated setter will be called and a default
    /// value will be provided as a value parameter. Default values can be
    /// customized using, e.g., <see cref="ReactPropBaseAttribute.DefaultInt32"/>. 
    /// In all other cases where the type is not a primitive, 
    /// <code>null</code> will be provided as a default.
    /// </remarks>
    [AttributeUsage(AttributeTargets.Method)]
    public class ReactPropAttribute : ReactPropBaseAttribute
    {
        /// <summary>
        /// Instantiates the <see cref="ReactPropAttribute"/>.
        /// </summary>
        /// <param name="name">The prop name.</param>
        public ReactPropAttribute(string name)
        {
            Name = name;
        }

        /// <summary>
        /// Name of the prop exposed to JavaScript.
        /// </summary>
        public string Name { get; }
    }
}
