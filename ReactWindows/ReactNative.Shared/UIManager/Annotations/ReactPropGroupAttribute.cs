// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

namespace ReactNative.UIManager.Annotations
{
    /// <summary>
    /// Annotates a group of props of native views that should be exposed
    /// to JavaScript. It is a batched version of the 
    /// <see cref="ReactPropAttribute"/> annotation.
    /// </summary>
    /// <remarks>
    /// This annotation is meant to be used of similar props. That is why
    /// it only supports a set of props of the same type. A good example
    /// is supporting "border", where there are many variations of that
    /// prop and each has very similar handling.
    /// 
    /// Each annotated method should return <see cref="void"/>.
    /// 
    /// In cases when the prop has been removed from the corresponding 
    /// React component, the annotated setter will be called and a default
    /// value will be provided as a value parameter. Default values can be
    /// customized using, e.g., <see cref="ReactPropBaseAttribute.DefaultInt32"/>.
    /// In all other cases, <code>null</code> will be provided as a default.
    /// </remarks>
    [AttributeUsage(AttributeTargets.Method)]
    public class ReactPropGroupAttribute : ReactPropBaseAttribute
    {        
        /// <summary>
        /// Instantiates the <see cref="ReactPropGroupAttribute"/>.
        /// </summary>
        /// <param name="names">The prop group names.</param>
        public ReactPropGroupAttribute(params string[] names)
        {
            Names = names;
        }

        /// <summary>
        /// The set of prop group names.
        /// </summary>
        public string[] Names { get; }
    }
}
