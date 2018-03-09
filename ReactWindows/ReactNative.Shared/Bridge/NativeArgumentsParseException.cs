using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// An exception thrown when converting between JavaScript and native arguments.
    /// </summary>
    public class NativeArgumentsParseException : ArgumentException
    {
        /// <summary>
        /// Instantiates the <see cref="NativeArgumentsParseException"/>. 
        /// </summary>
        public NativeArgumentsParseException()
        {
        }

        /// <summary>
        /// Instantiates the <see cref="NativeArgumentsParseException"/>. 
        /// </summary>
        /// <param name="message">The exception message.</param>
        public NativeArgumentsParseException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Instantiates the <see cref="NativeArgumentsParseException"/>. 
        /// </summary>
        /// <param name="message">The exception message.</param>
        /// <param name="innerException">The inner exception.</param>
        public NativeArgumentsParseException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        /// <summary>
        /// Instantiates the <see cref="NativeArgumentsParseException"/>.
        /// </summary>
        /// <param name="message">The exception message.</param>
        /// <param name="parameterName">The parameter name.</param>
        public NativeArgumentsParseException(string message, string parameterName)
            : base(message, parameterName)
        {
        }

        /// <summary>
        /// Instantiates the <see cref="NativeArgumentsParseException"/>.
        /// </summary>
        /// <param name="message">The exception message.</param>
        /// <param name="parameterName">The parameter name.</param>
        /// <param name="innerException">The inner exception.</param>
        public NativeArgumentsParseException(string message, string parameterName, Exception innerException)
            : base(message, parameterName, innerException)
        {
        }
    }
}
