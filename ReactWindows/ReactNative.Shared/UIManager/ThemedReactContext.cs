using ReactNative.Bridge;
using System;
using System.Threading.Tasks;

namespace ReactNative.UIManager
{
    /// <summary>
    /// A wrapper <see cref="ReactContext"/> that delegates lifecycle events to
    /// the original instance of <see cref="ReactContext"/>.
    /// </summary>
    public class ThemedReactContext
    {
        private readonly ReactContext _reactContext;

        /// <summary>
        /// Instantiates the <see cref="ThemedReactContext"/>.
        /// </summary>
        /// <param name="reactContext">The inner context.</param>
        public ThemedReactContext(ReactContext reactContext)
        {
            _reactContext = reactContext;
        }

        /// <summary>
        /// Gets the instance of the <see cref="INativeModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of native module.</typeparam>
        /// <returns>The native module instance.</returns>
        public T GetNativeModule<T>() where T : INativeModule
        {
            AssertReactContext();
            return _reactContext.GetNativeModule<T>();
        }

        private void AssertReactContext()
        {
            if ( _reactContext == null )
            {
                throw new InvalidOperationException("React context has not been set.");
            }
        }
    }
}
