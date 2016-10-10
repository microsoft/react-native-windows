using ReactNative.Bridge;
using ReactNative.Common;
using System;

namespace ReactNative.Modules.Vibration
{
    /// <summary>
    /// Represents the module for vibration.  Note this is only available on Windows Mobile.
    /// </summary>
    public class VibrationModule : NativeModuleBase
    {

        /// <summary>
        /// Creates a new instance of the Vibration Module.
        /// </summary>
        public VibrationModule() : this(IsMobile())
        {
        }

        /// <summary>
        /// Creates a new instance of the Vibration Module with a flag of whether the platform is mobile.
        /// </summary>
        public VibrationModule(bool isMobile)
        {
        }

        private static bool IsMobile()
        {
            return false;
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "Vibration";
            }
        }

        /// <summary>
        /// Vibrates the device for the specified milliseconds.
        /// </summary>
        /// <param name="duration">The duration in milliseconds to vibrate.</param>
        [ReactMethod]
        public void vibrate(int duration)
        {
        }

        /// <summary>
        /// Cancels the current vibration.
        /// </summary>
        [ReactMethod]
        public void cancel()
        {
        }
    }
}