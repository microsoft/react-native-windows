using NMock;
using NUnit.Framework;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ShadowNodeRegistryTests
    {
        [Test]
        public void ShadowNodeRegistry_ArgumentChecks()
        {
            var registry = new ShadowNodeRegistry();

            ArgumentNullException ex1 = Assert.Throws<ArgumentNullException>(
                () => registry.AddNode(null));
            Assert.AreEqual("node", ex1.ParamName);

            ArgumentNullException ex2 = Assert.Throws<ArgumentNullException>(
                () => registry.AddRootNode(null));
            Assert.AreEqual("node", ex2.ParamName);

            Assert.Throws<KeyNotFoundException>(() => registry.GetNode(1));
            registry.RemoveNode(1); /* does not throw */
            Assert.Throws<KeyNotFoundException>(() => registry.RemoveRootNode(1));

            registry.AddNode(new ReactShadowNode { ReactTag = 42 });
            registry.AddRootNode(new ReactShadowNode { ReactTag = 43 });

            Assert.Throws<KeyNotFoundException>(() => registry.RemoveRootNode(42));
            Assert.Throws<KeyNotFoundException>(() => registry.RemoveNode(43));
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
                Assert.Throws<KeyNotFoundException>(() => registry.GetNode(i));
            }
        }
    }
}
