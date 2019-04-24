// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using System.Diagnostics;

namespace ReactNative.Pooling
{
    /// <summary>
    /// Simple object pool.
    /// </summary>
    /// <typeparam name="T">Type of objects being pooled.</typeparam>
    /// <remarks>
    /// This object pool is not thread-safe. Please use for single-threaded access only.
    /// </remarks>
    class ObjectPool<T> where T : class
    {
        private readonly T[] _pool;
        private int _size;

        private readonly Func<T> _factory;

        /// <summary>
        /// Instantiates the <see cref="ObjectPool{T}"/>.
        /// </summary>
        /// <param name="factory">The instance factory.</param>
        /// <param name="size">The maximum size.</param>
        public ObjectPool(Func<T> factory, int size)
        {
            Debug.Assert(size >= 1);
            _factory = factory;
            _pool = new T[size];
        }

        /// <summary>
        /// Get a new instance from the pool. Create one using the factory if not available.
        /// </summary>
        /// <returns>The instance.</returns>
        public T Allocate()
        {
            if (_size == 0)
            {
                return CreateInstance();
            }

            _size--;
            var lastIndex = _size;
            var toReturn = _pool[lastIndex];
            _pool[lastIndex] = null;
            return toReturn;
        }

        /// <summary>
        /// Return an instance to the pool. No-op if pool is full.
        /// </summary>
        /// <param name="obj">The instance to return to the pool.</param>
        public void Free(T obj)
        {
            if (_size == _pool.Length)
            {
                return;
            }

            _pool[_size] = obj;
            _size++;
        }

        private T CreateInstance()
        {
            var inst = _factory();
            return inst;
        }
    }
}