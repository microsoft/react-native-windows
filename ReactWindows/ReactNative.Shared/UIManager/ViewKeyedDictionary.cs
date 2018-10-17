// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System.Collections.Generic;
using static System.FormattableString;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using System.Collections.Concurrent;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    /// <summary>
    /// A dictionary that has <see cref="UIElement"/>'s as keys.
    /// If a method has a <see cref="UIElement"/> parameter it must be called
    /// on the thread the <see cref="UIElement"/> has affinity to. With this assumption
    /// the dictionary is thread safe.
    /// </summary>
    /// <remarks>
    /// This class is designed to be used as a storage of custom state for a view.
    /// This could be needed when a view is sealed but a custom state is needed to implement
    /// a React property in a <see cref="ViewManager{TFrameworkElement, TReactShadowNode}"/>.
    /// The class is thread safe if any method that has a <see cref="UIElement"/> as parameter
    /// is called on a thread it has affinity to. Thread safety is important since there may exist
    /// multiple UI dispatchers (due to multiple windows) that use the same instance of a
    /// <see cref="ViewManager{TFrameworkElement, TReactShadowNode}"/>.
    /// </remarks>
    /// <typeparam name="TKey"></typeparam>
    /// <typeparam name="TValue"></typeparam>
    public class ViewKeyedDictionary<TKey, TValue> where TKey : DependencyObject
    {
#if WINDOWS_UWP
        private ConcurrentDictionary<TKey, TValue> _dictionary = new ConcurrentDictionary<TKey, TValue>();
#else
        private Dictionary<TKey, TValue> _dictionary = new Dictionary<TKey, TValue>();
#endif

        /// <summary>Attempts to get the value associated with the specified key.</summary>
        /// <param name="key">The key of the value to get.</param>
        /// <param name="value">When this method returns, contains the object that has
        ///     the specified key, or the default value of the type if the operation failed.</param>
        /// <returns>true if the key was found otherwise, false.</returns>
        public bool TryGetValue(TKey key, out TValue value)
        {
            DispatcherHelpers.AssertOnDispatcher(key);
            return _dictionary.TryGetValue(key, out value);
        }

        /// <summary>
        /// Set the value associated with the key. Replaces existing value associated with the key, if any.
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        public void AddOrUpdate(TKey key, TValue value)
        {
            DispatcherHelpers.AssertOnDispatcher(key);
#if WINDOWS_UWP
            _dictionary.AddOrUpdate(key, value, (k, v) => value);
#else
            _dictionary.Add(key, value);
#endif
        }

        /// <summary>
        /// Removes the value with the specified key.
        /// </summary>
        /// <param name="key"></param>
        public bool Remove(TKey key)
        {
            DispatcherHelpers.AssertOnDispatcher(key);
#if WINDOWS_UWP
            return _dictionary.TryRemove(key, out _);
#else
            return _dictionary.Remove(key);
#endif
        }

        /// <summary>
        /// Indexer
        /// </summary>
        public TValue this[TKey key]
        {
            get
            {
                if (TryGetValue(key, out var value))
                {
                    return value;
                }
                else
                {
                    throw new KeyNotFoundException(Invariant($"Key '{key}' not found."));
                }
            }
            set
            {
                AddOrUpdate(key, value);
            }
        }
    }
}
