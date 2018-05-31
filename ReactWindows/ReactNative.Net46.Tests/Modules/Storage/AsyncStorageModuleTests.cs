// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Modules.Storage;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace ReactNative.Tests.Modules.Storage
{
    [TestFixture]
    public class AsyncStorageModuleTests
    {
        AsyncStorageModule module;
        AutoResetEvent waitHandle;
        JObject error;
        JArray result;
        MockCallback callback;

        [SetUp]
        public void Init()
        {
            module = new AsyncStorageModule();
            waitHandle = new AutoResetEvent(false);

            error = default(JObject);
            result = default(JArray);
            callback = new MockCallback(res =>
             {
                 error = res.Length > 0 ? (JObject)res[0] : null;
                 result = res.Length > 1 ? (JArray)res[1] : null;
                 waitHandle.Set();
             });
        }

        [TearDown]
        public void Cleanup()
        {
            module.clear(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);
        }


        [Test]
        public void AsyncStorageModule_InvalidKeyValue_Method()
        {
            var array = new[]
            {
                new[] { "5", "5", "5" },
            };

            module.multiSet(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error["message"].Value<string>(), Is.EqualTo("Invalid Value"));
            Assert.That(result, Is.Null);
        }

        [Test]
        public void AsyncStorageModule_GetAllKeys_SpecialCharacters()
        {
            var pairs = new[]
            {
                new[] { "\\", "1" },
                new[] { "/", "2" },
                new[] { ":", "3" },
                new[] { "*", "4" },
                new[] { "?", "5" },
                new[] { "<", "6" },
                new[] { ">", "7" },
                new[] { "|", "8" },
                new[] { "\"", "9" },
                new[] { ".", "10" },
                new[] { "{", "11" },
                new[] { "}", "12" },
                new[] { "\\/:*?<>|\".{}", "13" },
                new[] { "abc\\abc/abc:abc*abc?abc<abc>abc|abc\"abc.abc{abc}abc", "13" },
                new[] { "foo:bar", "14" },
            };

            module.multiSet(pairs, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            var expectedKeys = pairs.Select(pair => pair[0]).OrderBy(key => key);
            var actualKeys = result.ToObject<string[]>().OrderBy(key => key);
            Assert.That(expectedKeys, Is.EqualTo(actualKeys));
        }

        [Test]
        public void AsyncStorageModule_multiGet_Method()
        {
            var array = new[]
            {
                new[] { "test1", "5" },
            };

            module.multiSet(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.multiGet(new string[] { "test1", }, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(1));
            Assert.That((result[0]).Last.Value<string>(), Is.EqualTo("5"));

        }

        [Test]
        public void AsyncStorageModule_multiRemove_Method()
        {
            var array = new[]
            {
                new[] { "test1", "1" },
                new[] { "test2", "2" },
                new[] { "test3", "3" },
                new[] { "test4", "4" },
                new[] { "test5", "5" },
                new[] { "test6", "6" },
            };

            module.multiSet(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(6));

            var strArray = new string[result.Count];
            int idx = 0;
            foreach (var item in result)
            {
                strArray[idx++] = item.Value<string>();
            }

            module.multiGet(strArray, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            var expected = array.OrderBy(p => p[0]).Aggregate("", (acc, p) => $"{p[0]},{p[1]};");
            var actual = result.OrderBy(p => p[0]).Aggregate("", (acc, p) => $"{p[0]},{p[1]};");
            Assert.That(expected, Is.EqualTo(actual));

            var keys = new string[]
            {
                "test1",
                "test2",
            };

            module.multiRemove(keys, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(4));
        }

        [Test]
        public void AsyncStorageModule_multiSet_LargeValue()
        {
            var array = new[]
            {
                new[] { "testKey", string.Join("", Enumerable.Repeat("a", 1024 * 16)) },
            };

            module.multiSet(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.multiGet(new[] { "testKey" }, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(JArray.FromObject(array).ToString(Formatting.None),
                Is.EqualTo(result.ToString(Formatting.None)));
        }

        [Test]
        public void AsyncStorageModule_multiMerge_NullValue()
        {
            var array = new[]
            {
                new[] { "testKey", string.Join("", Enumerable.Repeat("a", 1024 * 16)) },
            };

            module.multiMerge(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);
        }

        [Test]
        public void AsyncStorageModule_testMultiSetMultiGet()
        {
            var key1 = "foo1";
            var key2 = "foo2";
            var fakeKey = "fakeKey";
            var value1 = "bar1";
            var value2 = "bar2";

            var keyValues = new List<string[]>
            {
                new[] { key1, value1 },
                new[] { key2, value2 }
            };

            module.multiSet(keyValues.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var keys = new List<string>
            {
                key1,
                key2
            };

            module.multiGet(keys.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            var expected = JArray.FromObject(keyValues);
            Assert.That(result, Is.EqualTo(expected));

            keys.Add(fakeKey);
            keyValues.Add(new[] { fakeKey, null });

            module.multiGet(keys.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            expected = JArray.FromObject(keyValues);
            Assert.That(result, Is.EqualTo(expected));
        }

        [Test]
        public void AsyncStorageModule_testMultiRemove()
        {
            var key1 = "foo1";
            var key2 = "foo2";
            var value1 = "bar1";
            var value2 = "bar2";

            var keyValues = new List<string[]>
            {
                new[] { key1, value1 },
                new[] { key2, value2 }
            };

            module.multiSet(keyValues.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var keys = new List<string>
            {
                key1,
                key2
            };

            module.multiRemove(keys.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(0));

            module.multiSet(keyValues.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            keys.Add("fakeKey");
            module.multiRemove(keys.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(0));
        }

        [Test]
        public void AsyncStorageModule_testMultiMerge()
        {
            var mergeKey = "mergeTest";

            var value = new JObject();
            value.Add("foo1", "bar1");

            value.Add("foo2", new JArray
            {
                "val1",
                "val2",
                3,
            });

            value.Add("foo3", 1001);

            var val = new JObject();
            val.Add("key1", "randomValueThatWillNeverBeUsed");
            value.Add("foo4", val);

            var array = new[]
            {
                new[]
                {
                    mergeKey,
                    value.ToString(Formatting.None)
                },
            };

            module.multiSet(array, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var str = new string[] { mergeKey };

            module.multiGet(str, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            var expected = JToken.FromObject(array);
            Assert.That(result, Is.EqualTo(expected));

            value.Remove("foo1");
            value.Remove("foo2");
            value.Remove("foo3");
            value.Remove("foo4");

            value.Add("foo1", 1001);

            var val2 = new JObject();
            val2.Add("key1", "val1");
            value.Add("foo2", val2);
            value.Add("foo3", "bar1");
            value.Add("foo4", new JArray
            {
                "val1",
                "val2",
                3
            });

            var newValue = new JObject();
            var val3 = new JObject();
            val3.Add("key2", "val2");
            newValue.Add("foo2", val3);

            var newValue2 = new JObject();
            var val4 = new JObject();
            val4.Add("key1", "val3");
            newValue2.Add("foo2", val4);

            var array2 = new[]
            {
                new[]
                {
                    mergeKey,
                    value.ToString(Formatting.None)
                },
            };

            module.multiMerge(array2, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var array3 = new[]
            {
                new[]
                {
                    mergeKey,
                    newValue.ToString(Formatting.None),
                },
            };

            module.multiMerge(array3, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var array4 = new[]
            {
                new[]
                {
                    mergeKey,
                    newValue2.ToString(Formatting.None),
                },
            };

            module.multiMerge(array4, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            value.Remove("foo2");
            var val5 = new JObject();
            val5.Add("key1", "val3");
            val5.Add("key2", "val2");
            value.Add("foo2", val5);

            module.multiGet(str, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            expected = JToken.FromObject(array);
            Assert.That(JObject.Parse(result.Last.Value<JArray>().Last.Value<string>()),
                Is.EquivalentTo(value));
        }

        [Test]
        public void AsyncStorageModule_testGetAllKeys()
        {
            var keys = new[] { "foo", "foo2" };
            var values = new[] { "bar", "bar2" };

            var keyValues = new[]
            {
                new[]
                {
                    keys[0],
                    values[0],
                },
                new[]
                {
                    keys[1],
                    values[1],
                },
            };

            module.multiSet(keyValues, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);

            var storedKeys = new JArray
            {
                keys[0],
                keys[1],
            };

            var set = new SortedSet<string>();
            IEnumerable<string> enumerator = storedKeys.Values<string>();

            foreach (var value in enumerator)
            {
                set.Add(value);
            }

            set.SymmetricExceptWith(result.Values<string>());
            Assert.That(set, Has.Count.EqualTo(0));

            module.multiRemove(keys, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(0));
        }

        [Test]
        public void AsyncStorageModule_testClear()
        {
            var keys = new[] { "foo", "foo2" };
            var values = new[] { "bar", "bar2" };

            var keyValues = new[]
            {
                new[]
                {
                    keys[0],
                    values[0],
                },
                new[]
                {
                    keys[1],
                    values[1],
                },
            };

            module.multiSet(keyValues, callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.clear(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(0));
        }

        [Test]
        public void AsyncStorageModule_testHugeMultiGetMultiGet()
        {
            const string KEY_NAME = "key";
            const string VALUE_NAME = "value";

            // Limitation on Android - not a limitation on Windows
            // Test with many keys, so that it's above the 999 limit per batch imposed by SQLite.
            int keyCount = 1001;
            // don't set keys that divide by this magical number, so that we can check that multiGet works,
            // and returns null for missing keys
            int magicalNumber = 343;

            var keyValues = new List<string[]>();
            for (var i = 0; i < keyCount; i++)
            {
                if (i % magicalNumber > 0)
                {
                    var key = KEY_NAME + i;
                    var value = VALUE_NAME + i;
                    keyValues.Add(new[]
                    {
                        key,
                        value,
                    });
                }
            }
            module.multiSet(keyValues.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            var keys = new List<string>();
            for (var i = 0; i < keyCount; i++)
            {
                keys.Add(KEY_NAME + i);
            }

            module.multiGet(keys.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result.Count, Is.EqualTo(keys.Count));

            var keyReceived = new bool[keyCount];

            for (var i = 0; i < keyCount; i++)
            {
                var keyValue = result[i];
                var key = keyValue.Value<JArray>().First.Value<string>().Substring(3);

                var idx = int.Parse(key);
                Assert.That(keyReceived[idx], Is.False);
                keyReceived[idx] = true;

                if (idx % magicalNumber > 0)
                {
                    var value = keyValue.Value<JArray>().Last.Value<string>().Replace(VALUE_NAME, string.Empty);
                    Assert.That(key, Is.EqualTo(value));
                }
                else
                {
                    Assert.That(keyValue.Value<JArray>().Last.Type, Is.EqualTo(JTokenType.Null));
                }
            }

            var keyRemoves = new List<string>();
            for (var i = 0; i < keyCount; i++)
            {
                if (i % 2 > 0)
                {
                    keyRemoves.Add(KEY_NAME + i);
                }
            }

            module.multiRemove(keyRemoves.ToArray(), callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Is.Null);

            module.getAllKeys(callback);
            Assert.That(waitHandle.WaitOne(), Is.True);
            Assert.That(error, Is.Null);
            Assert.That(result, Has.Count.EqualTo(499));
            for (var i = 0; i < result.Count; i++)
            {
                var key = result[i].Value<string>().Replace(KEY_NAME, string.Empty);
                var idx = int.Parse(key);
                Assert.That(idx % 2, Is.EqualTo(0));
            }
        }
    }
}
