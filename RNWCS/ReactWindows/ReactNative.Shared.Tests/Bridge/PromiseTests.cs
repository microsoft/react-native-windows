// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using System;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class PromiseTests
    {
        [Test]
        public void Promise_Resolve()
        {
            var args = default(object[]);
            var are = new AutoResetEvent(false);
            var resolve = new MockCallback(a =>
            {
                args = a;
                are.Set();
            });
            var reject = new MockCallback(_ => { });
            var promise = new Promise(resolve, reject);

            var o = new object();
            promise.Resolve(o);
            are.WaitOne();

            Assert.IsNotNull(args);
            Assert.AreEqual(1, args.Length);
            Assert.AreSame(o, args[0]);
        }

        [Test]
        public void Promise_Reject()
        {
            var args = default(object[]);
            var are = new AutoResetEvent(false);
            var resolve = new MockCallback(_ => { });
            var reject = new MockCallback(a =>
            {
                args = a;
                are.Set();
            });
            var promise = new Promise(resolve, reject);

            var code = "42";
            var message = "foo";
            promise.Reject(code, message);
            are.WaitOne();
            Assert.IsNotNull(args);
            Assert.AreEqual(1, args.Length);

            var json = args[0] as JObject;
            Assert.IsNotNull(json);
            Assert.AreEqual(code, json["code"].ToString());
            Assert.AreEqual(message, json["message"].ToString());
        }

        [Test]
        public void Promise_Reject_UserInfo()
        {
            var args = default(object[]);
            var are = new AutoResetEvent(false);
            var resolve = new MockCallback(_ => { });
            var reject = new MockCallback(a =>
            {
                args = a;
                are.Set();
            });
            var promise = new Promise(resolve, reject);

            var code = "42";
            var message = "foo";
            var e = new Exception();
            e.Data.Add("qux", "baz");
            promise.Reject(code, message, e);
            are.WaitOne();

            Assert.IsNotNull(args);
            Assert.AreEqual(1, args.Length);
            var json = args[0] as JObject;
            Assert.IsNotNull(json);
            var userInfo = json["userInfo"] as JObject;
            Assert.IsNotNull(userInfo);
            Assert.AreEqual("baz", userInfo["qux"].ToString());
        }
    }
}
