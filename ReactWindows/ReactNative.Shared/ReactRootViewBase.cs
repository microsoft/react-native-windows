using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Text;

namespace ReactNative
{
    /// <summary>
    /// Base class for a panel that hosts React content.
    /// Custom implementations may use their own logic for injecting the view
    /// into <see cref="ReactInstanceManager"/>.
    /// </summary>
    public abstract class ReactRootViewBase : SizeMonitoringCanvas
    {
        /// <summary>
        /// The name of the JS module this view loads
        /// </summary>
        public virtual string JavaScriptModuleName { get; set; }

        /// <summary>
        /// The props value passed to the React component
        /// </summary>
        public virtual JObject InitialProps { get; set; }
    }
}
