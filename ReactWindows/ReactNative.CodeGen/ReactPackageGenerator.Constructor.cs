// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;

namespace ReactNative.CodeGen
{
    public partial class ReactPackageGenerator
    {
        private readonly JToken _metadata;

        public ReactPackageGenerator(JToken metadata)
        {
            _metadata = metadata;
        }
    }
}
