using NUnit.Framework;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ShadowNodeRegistryTests
    {
        [Test]
        public void ShadowNodeRegistry_ArgumentChecks()
        {
            var registry = new ShadowNodeRegistry();

            AssertEx.Throws<ArgumentNullException>(
                () => registry.AddNode(null),
                ex => Assert.AreEqual("node", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => registry.AddRootNode(null),
                ex => Assert.AreEqual("node", ex.ParamName));

            AssertEx.Throws<KeyNotFoundException>(() => registry.GetNode(1));
            registry.RemoveNode(1); /* does not throw */
            AssertEx.Throws<KeyNotFoundException>(() => registry.RemoveRootNode(1));

            registry.AddNode(new ReactShadowNode { ReactTag = 42 });
            registry.AddRootNode(new ReactShadowNode { ReactTag = 43 });

            AssertEx.Throws<KeyNotFoundException>(() => registry.RemoveRootNode(42));
            AssertEx.Throws<KeyNotFoundException>(() => registry.RemoveNode(43));
        }

        [Test]
        public void ShadowRegistryNode_AddRootNode()
        {
            var count = 5;
            var nodes = Enumerable.Range(0, count).Select(i => new ReactShadowNode { ReactTag = i }).ToList();
            var registry = new ShadowNodeRegistry();
            foreach (var node in nodes)
            {
                registry.AddRootNode(node);
            }

            for (var i = 0; i < count; ++i)
            {
                Assert.AreSame(nodes[i], registry.GetNode(i));
                Assert.IsTrue(registry.IsRootNode(i));
            }

            Assert.AreEqual(count, registry.RootNodeTags.Count);

            for (var i = 0; i < count; ++i)
            {
                registry.RemoveRootNode(i);
            }

            Assert.AreEqual(0, registry.RootNodeTags.Count);
        }

        [Test]
        public void ShadowRegistryNode_AddNode()
        {
            var count = 5;
            var nodes = Enumerable.Range(0, count).Select(i => new ReactShadowNode { ReactTag = i }).ToList();
            var registry = new ShadowNodeRegistry();
            foreach (var node in nodes)
            {
                registry.AddNode(node);
            }

            for (var i = 0; i < count; ++i)
            {
                Assert.AreSame(nodes[i], registry.GetNode(i));
            }

            for (var i = 0; i < count; ++i)
            {
                registry.RemoveNode(i);
                AssertEx.Throws<KeyNotFoundException>(() => registry.GetNode(i));
            }
        }

        [Test]
        public async Task ShadowRegistryNode_AddRootNode_WhileEnumerating()
        {
            var n1 = new ReactShadowNode { ReactTag = 1 };
            var n2 = new ReactShadowNode { ReactTag = 2 };

            var enter = new AutoResetEvent(false);
            var exit = new AutoResetEvent(false);

            var registry = new ShadowNodeRegistry();

            var task = Task.Run(() =>
            {
                enter.WaitOne();
                registry.AddRootNode(n2);
                exit.Set();
            });

            registry.AddRootNode(n1);

            foreach (var tag in registry.RootNodeTags)
            {
                enter.Set();
                exit.WaitOne();
            }

            await task;

            Assert.AreEqual(2, registry.RootNodeTags.Count);
        }
    }
}
