// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive.Disposables;
using System.Threading.Tasks;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// A queue for executing actions in order.
    /// </summary>
    /// <remarks>
    /// This action queue ensures that async continuations are performed on the same thread.
    /// </remarks>
    public class LimitedConcurrencyActionQueue : IActionQueue
    {
        private readonly object _dispatchGate = new object();
        private readonly object _actionGate = new object();
        private readonly Action<Exception> _onError;
        private readonly CancellationDisposable _cancellationDisposable;
        private readonly ConcurrentExclusiveSchedulerPair _schedulerPair;
        private readonly TaskFactory _taskFactory;

        /// <summary>
        /// Creates an action queue where the actions are performed on the
        /// <see cref="ConcurrentExclusiveSchedulerPair.ExclusiveScheduler"/>.
        /// </summary>
        /// <param name="onError">The error handler.</param>
        public LimitedConcurrencyActionQueue(Action<Exception> onError)
        {
            if (onError == null)
                throw new ArgumentNullException(nameof(onError));

            _onError = onError;
            _cancellationDisposable = new CancellationDisposable();
            _schedulerPair = new ConcurrentExclusiveSchedulerPair();
            _taskFactory = new TaskFactory(
                _cancellationDisposable.Token,
                TaskCreationOptions.None,
                TaskContinuationOptions.None,
                _schedulerPair.ExclusiveScheduler);
        }

        /// <summary>
        /// Dispatch an action to the queue.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <remarks>
        /// Returns immediately.
        /// </remarks>
        public void Dispatch(Action action)
        {
            lock (_dispatchGate)
            {
                if (!_cancellationDisposable.IsDisposed)
                {
                    _taskFactory.StartNew(() =>
                    {
                        try
                        {
                            lock (_actionGate)
                            {
                                if (!_cancellationDisposable.IsDisposed)
                                {
                                    action();
                                }
                            }
                        }
                        catch (Exception ex)
                        {
                            _onError(ex);
                        }
                    });
                }
            }
        }

        /// <summary>
        /// Checks if the current thread is running in the context of this
        /// action queue.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the current thread is running an action
        /// dispatched by this action queue, otherwise <code>false</code>.
        /// </returns>
        public bool IsOnThread()
        {
            return TaskScheduler.Current == _schedulerPair.ExclusiveScheduler;
        }

        /// <summary>
        /// Disposes the action queue.
        /// </summary>
        public void Dispose()
        {
            lock (_dispatchGate)
            lock (_actionGate)
            {
                _cancellationDisposable.Dispose();
            }
        }
    }
}
