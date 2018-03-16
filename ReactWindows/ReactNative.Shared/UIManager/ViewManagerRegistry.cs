// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using static System.FormattableString;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Class that stores the mapping between the native view name used in
    /// JavaScript and the instance of <see cref="IViewManager"/>.
    /// </summary>
    public class ViewManagerRegistry
    {
        /// <summary>
        /// Map of view manager name to instance.
        /// </summary>
        /// <remarks>
        /// This dictionary is accessed from both the main UI thread and the
        /// JavaScript thread. We don't need to make it thread-safe because
        /// only the UI thread writes to the dictionary.
        /// </remarks>
        private readonly IReadOnlyDictionary<string, IViewManager> _registry;

        /// <summary>
        /// Instantiates the <see cref="ViewManagerRegistry"/>.
        /// </summary>
        /// <param name="viewManagers">
        /// The view managers to include in the registry.
        /// </param>
        public ViewManagerRegistry(IReadOnlyList<IViewManager> viewManagers)
        {
            if (viewManagers == null)
                throw new ArgumentNullException(nameof(viewManagers));

            var registry = new Dictionary<string, IViewManager>(viewManagers.Count);
            _registry = registry;

            foreach (var viewManager in viewManagers)
            {
                registry.Add(viewManager.Name, viewManager);
            }
        }

        /// <summary>
        /// Gets the view manager for the given class name.
        /// </summary>
        /// <param name="className">The view manager class name.</param>
        /// <returns>The view manager.</returns>
        public IViewManager Get(string className)
        {
            if (className == null)
                throw new ArgumentNullException(nameof(className));

            if (_registry.TryGetValue(className, out var viewManager))
            {
                return viewManager;
            }

            throw new ArgumentException(
                Invariant($"No view manager defined for class '{className}'."),
                nameof(className));
        }
    }
}
