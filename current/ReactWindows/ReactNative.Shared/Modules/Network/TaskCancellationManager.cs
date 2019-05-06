// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive.Disposables;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Modules.Network
{
    /// <summary>
    /// Task cancellation manager.
    /// </summary>
    /// <typeparam name="TKey">Type of key used to identify tasks.</typeparam>
    public class TaskCancellationManager<TKey>
    {
        private readonly object _gate = new object();
        private readonly IDictionary<TKey, IDisposable> _tokens;

        /// <summary>
        /// Instantiates a <see cref="TaskCancellationManager{TKey}"/>.
        /// </summary>
        public TaskCancellationManager()
            : this(EqualityComparer<TKey>.Default)
        {
        }

        /// <summary>
        /// Instantiates a <see cref="TaskCancellationManager{TKey}"/>.
        /// </summary>
        /// <param name="keyComparer">The key comparer.</param>
        public TaskCancellationManager(IEqualityComparer<TKey> keyComparer)
        {
            if (keyComparer == null)
                throw new ArgumentNullException(nameof(keyComparer));

            _tokens = new Dictionary<TKey, IDisposable>(keyComparer);
        }

        /// <summary>
        /// Number of outstanding operations being managed.
        /// </summary>
        internal int PendingOperationCount
        {
            get
            {
                return _tokens.Count;
            }
        }

        /// <summary>
        /// Adds a task to the manager.
        /// </summary>
        /// <param name="key">The task key.</param>
        /// <param name="taskFactory">The task factory.</param>
        /// <remarks>
        /// The task factory is invoked during this method call.
        /// </remarks>
        public Task AddAndInvokeAsync(TKey key, Func<CancellationToken, Task> taskFactory)
        {
            var disposable = new CancellationDisposable();
            lock (_gate)
            {
                _tokens.Add(key, disposable);
            }

            return taskFactory(disposable.Token).ContinueWith(
                task =>
                {
                    lock (_gate)
                    {
                        _tokens.Remove(key);
                    }

                    disposable.Dispose();
                    return task;
                },
                TaskContinuationOptions.ExecuteSynchronously).Unwrap();
        }

        /// <summary>
        /// Cancels the task with the given key.
        /// </summary>
        /// <param name="key">The task key.</param>
        public void Cancel(TKey key)
        {
            IDisposable disposable;
            lock (_gate)
            {
                _tokens.TryGetValue(key, out disposable);
            }

            disposable?.Dispose();
        }

        /// <summary>
        /// Cancels all pending tasks.
        /// </summary>
        public void CancelAllTasks()
        {
            IList<IDisposable> tokens;
            lock (_gate)
            {
                // Clone the list of disposables
                tokens = _tokens.Values.ToList();
            }

            foreach (var token in tokens)
            {
                // Dispose on CancellationDisposable is idempotent
                token.Dispose();
            }
        }
    }
}
