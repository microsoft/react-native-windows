using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;

namespace ReactNative.UIManager
{
    public class CatalystStylesDiffMap
    {
        private readonly JObject _properties;

        public CatalystStylesDiffMap(JObject properties)
        {
            if (properties == null)
                throw new ArgumentNullException(nameof(properties));

            _properties = properties;
        }

        public ICollection<string> Keys
        {
            get
            {
                return ((IDictionary<string, JToken>)_properties).Keys;
            }
        }

        public object GetProperty(string name, Type propertyType)
        {
            var token = default(JToken);
            if (_properties.TryGetValue(name, out token))
            {
                return token.ToObject(propertyType);
            }

            return null;
        }
    }
}