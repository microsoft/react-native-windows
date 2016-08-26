using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Interface that represents a JavaScript Promise which can be passed to 
    /// the native module as a method parameter.
    /// </summary>
    /// <remarks>
    /// Methods annotated with <see cref="ReactMethodAttribute"/> that use 
    /// <see cref="IPromise"/> as type of the last parameter will be marked as
    /// "remoteAsync" and will return a promise when invoked from JavaScript.
    /// </remarks>
    public interface IPromise
    {
        /// <summary>
        /// Resolve the promise with the given value.
        /// </summary>
        /// <param name="value">The value.</param>
        void Resolve(object value);

        /// <summary>
        /// Report an error which wasn't caused by an exception.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <param name="message">The error message.</param>
        void Reject(string code, string message);

        /// <summary>
        /// Report an exception.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <param name="e">The exception.</param>
        void Reject(string code, Exception e);

        /// <summary>
        /// Report an exception with a custom error message.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <param name="message">The error message.</param>
        /// <param name="e">The exception.</param>
        void Reject(string code, string message, Exception e);

        /// <summary>
        /// Report an error which wasn't caused by an exception.
        /// </summary>
        /// <param name="message">The error message.</param>
        /// <remarks>
        /// Using this method will pass the error code "EUNSPECIFIED".
        /// </remarks>
        [Obsolete("Prefer passing a module-specific error code to JavaScript.")]
        void Reject(string message);

        /// <summary>
        /// Reject the promise with the given exception.
        /// </summary>
        /// <param name="exception">The exception.</param>
        void Reject(Exception exception);
    }
}
