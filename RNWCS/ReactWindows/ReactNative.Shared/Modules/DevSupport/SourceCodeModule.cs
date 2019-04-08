// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;

namespace ReactNative.Modules.DevSupport
{
    /// <summary>
    /// Module that exposes the URL to the source code map (used for exception
    /// stack trace parsing) to JavaScript.
    /// </summary>
    class SourceCodeModule : NativeModuleBase
    {
        private readonly string _sourceMapUrl;
        private readonly string _sourceUrl;

        /// <summary>
        /// Instantiates the <see cref="SourceCodeModule"/>.
        /// </summary>
        /// <param name="sourceUrl">The source URL.</param>
        /// <param name="sourceMapUrl">The source map URL.</param>
        public SourceCodeModule(string sourceUrl, string sourceMapUrl)
        {
            _sourceMapUrl = sourceMapUrl;
            _sourceUrl = sourceUrl;
        }

        /// <summary>
        /// The name of the module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "SourceCode";
            }
        }

        /// <summary>
        /// The module constants.
        /// </summary>
        public override JObject ModuleConstants
        {
            get
            {
                return new JObject
                {
                    { "scriptURL", _sourceUrl },
                };
            }
        }

        /// <summary>
        /// Gets the script mapping URL.
        /// </summary>
        /// <param name="promise">The promise.</param>
        [ReactMethod]
        public void getScriptText(IPromise promise)
        {
            var map = new JObject
            {
                { "fullSourceMappingURL", _sourceMapUrl },
            };

            promise.Resolve(map);
        }
    }
}
