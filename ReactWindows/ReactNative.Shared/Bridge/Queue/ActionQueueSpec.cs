using System;
using static System.FormattableString;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Specification for creating a <see cref="IActionQueue"/>.
    /// </summary>
    public class ActionQueueSpec
    {
        private ActionQueueSpec(ActionQueueKind kind, string name)
        {
            Name = name;
            Kind = kind;
        }

        /// <summary>
        /// The name of the <see cref="IActionQueue"/>.
        /// </summary>
        public string Name { get; }

        /// <summary>
        /// The type of the <see cref="IActionQueue"/>.
        /// </summary>
        internal ActionQueueKind Kind { get; }

        /// <summary>
        /// Singleton dispatcher <see cref="IActionQueue"/> specification.
        /// </summary>
        public static ActionQueueSpec DispatcherThreadSpec { get; } = new ActionQueueSpec(ActionQueueKind.Dispatcher, "main_ui");

        /// <summary>
        /// Singleton layout <see cref="IActionQueue"/> specification. 
        /// </summary>
        public static ActionQueueSpec LayoutThreadSpec { get; } = new ActionQueueSpec(ActionQueueKind.Layout, "layout");

        /// <summary>
        /// Factory for creating <see cref="ActionQueueSpec"/>s.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="kind">The kind.</param>
        /// <returns>The instance.</returns>
        public static ActionQueueSpec Create(string name, ActionQueueKind kind)
        {
            if (kind == ActionQueueKind.Dispatcher)
            {
                throw new NotSupportedException(Invariant($"Use the singleton {nameof(DispatcherThreadSpec)} instance."));
            }

            if (kind == ActionQueueKind.Layout)
            {
                throw new NotSupportedException(Invariant($"Use the singleton {nameof(LayoutThreadSpec)} instance."));
            }

            return new ActionQueueSpec(kind, name);
        }
    }
}
