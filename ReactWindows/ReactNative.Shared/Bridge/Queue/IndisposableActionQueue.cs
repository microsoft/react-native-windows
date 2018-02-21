using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// An action queue that cannot be disposed.
    /// </summary>
    /// <remarks>
    /// Use this to wrap an <see cref="IActionQueue" /> that you want to pass to
    /// somebody but you want to ensure they don't dispose it.
    /// </remarks>
    public class IndisposableActionQueue : IActionQueue
    {
        private readonly IActionQueue _actionQueue;

        /// <summary>
        /// Creates an action queue which behaves like the action queue that is
        /// passed in except it isn't disposable.
        /// </summary>
        /// <param name="actionQueue">The action queue to wrap.</param>
        public IndisposableActionQueue(IActionQueue actionQueue)
        {
            if (actionQueue == null)
                throw new ArgumentNullException(nameof(actionQueue));

            _actionQueue = actionQueue;
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
            _actionQueue.Dispatch(action);
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
            return _actionQueue.IsOnThread();
        }

        /// <summary>
        /// Disposes the action queue.
        /// </summary>
        public void Dispose()
        {
            throw new InvalidOperationException("You shouldn't call Dispose on this ActionQueue. Somebody else is responsible for disposing it.");
        }
    }
}
