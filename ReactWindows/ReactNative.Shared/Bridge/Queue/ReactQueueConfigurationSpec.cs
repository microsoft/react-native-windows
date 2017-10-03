using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Specification for creating a <see cref="IReactQueueConfiguration"/>.
    /// This exists so the <see cref="IReactInstance"/> is able to set
    /// exception handlers on the <see cref="IActionQueue"/>s it uses.
    /// </summary>
    public sealed class ReactQueueConfigurationSpec
    {
        private ReactQueueConfigurationSpec(
            ActionQueueSpec nativeModulesQueueThreadSpec,
            ActionQueueSpec jsQueueThreadSpec)
        {
            NativeModulesQueueThreadSpec = nativeModulesQueueThreadSpec;
            JSQueueThreadSpec = jsQueueThreadSpec;
        }

        /// <summary>
        /// The native modules <see cref="IActionQueue"/> specification.
        /// </summary>
        public ActionQueueSpec NativeModulesQueueThreadSpec
        {
            get;
        }

        /// <summary>
        /// The JavaScript <see cref="IActionQueue"/> specification.
        /// </summary>
        public ActionQueueSpec JSQueueThreadSpec
        {
            get;
        }

        /// <summary>
        /// The default <see cref="ReactQueueConfigurationSpec"/> instance.
        /// </summary>
        public static ReactQueueConfigurationSpec Default { get; } = CreateDefault();

        private static ReactQueueConfigurationSpec CreateDefault()
        {
            return new Builder()
            {
                JSQueueThreadSpec = ActionQueueSpec.Create("js", ActionQueueKind.NewThread),
                NativeModulesQueueThreadSpec = ActionQueueSpec.Create("native_modules", ActionQueueKind.TaskPool),
            }
            .Build();
        }

        /// <summary>
        /// Builder for <see cref="ReactQueueConfigurationSpec"/>.
        /// </summary>
        public sealed class Builder
        {
            private ActionQueueSpec _nativeModulesQueueThreadSpec;
            private ActionQueueSpec _jsQueueThreadSpec;

            /// <summary>
            /// Set the native modules <see cref="ActionQueueSpec"/>.
            /// </summary>
            public ActionQueueSpec NativeModulesQueueThreadSpec
            {
                set
                {
                    if (_nativeModulesQueueThreadSpec != null)
                    {
                        throw new InvalidOperationException("Setting native modules queue thread spec multiple times!");
                    }

                    _nativeModulesQueueThreadSpec = value;
                }
            }

            /// <summary>
            /// Set the JavaScript <see cref="ActionQueueSpec"/>.
            /// </summary>
            public ActionQueueSpec JSQueueThreadSpec
            {
                set
                {
                    if (_jsQueueThreadSpec != null)
                    {
                        throw new InvalidOperationException("Setting JavaScript queue thread spec multiple times!");
                    }

                    _jsQueueThreadSpec = value;
                }
            }

            /// <summary>
            /// Build the <see cref="ReactQueueConfigurationSpec"/>.
            /// </summary>
            /// <returns>The instance.</returns>
            public ReactQueueConfigurationSpec Build()
            {
                if (_nativeModulesQueueThreadSpec == null)
                {
                    throw new InvalidOperationException("Native modules queue thread spec has not been set.");
                }

                if (_jsQueueThreadSpec == null)
                {
                    throw new InvalidOperationException("JS queue thread spec has not been set.");
                }

                return new ReactQueueConfigurationSpec(_nativeModulesQueueThreadSpec, _jsQueueThreadSpec);
            }
        }
    }
}
