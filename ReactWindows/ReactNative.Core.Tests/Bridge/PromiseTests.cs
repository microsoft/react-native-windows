using NUnit.Framework;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Threading;

namespace ReactNative.Core.Bridge.Tests
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

            Assert.That(args, Is.Not.Null);
            Assert.That(args.Length, Is.EqualTo(1));
            Assert.That(args[0], Is.SameAs(o));
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
            Assert.That(args, Is.Not.Null);
            Assert.That(args.Length, Is.EqualTo(1));

            var json = args[0] as JObject;
            Assert.That(json, Is.Not.Null);
            Assert.That(json["code"].ToString(), Is.EqualTo(code));
            Assert.That(json["message"].ToString(), Is.EqualTo(message));
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

            Assert.That(args, Is.Not.Null);
            Assert.That(args.Length, Is.EqualTo(1));
            var json = args[0] as JObject;
            Assert.That(json, Is.Not.Null);
            var userInfo = json["userInfo"] as JObject;
            Assert.That(userInfo, Is.Not.Null);
            Assert.That(userInfo["qux"].ToString(), Is.EqualTo("baz"));
        }
    }
}
