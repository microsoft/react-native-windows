using NUnit.Framework;
using ReactNative.Bridge;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistrySharedTests
    {
        protected JavaScriptModuleRegistry _registry;
        protected JavaScriptModuleRegistry.Builder _registryBuilder;

        [SetUp]
        public virtual void SetUp()
        {
            _registryBuilder = new JavaScriptModuleRegistry.Builder();
        }

#if DEBUG
        [Test]
        public void BuilderThrowsWithBaseClass()
        {
            Assert.That(
                () => _registryBuilder.Add(typeof(JavaScriptModuleBase)),
                Throws.ArgumentException.With.Property("ParamName").EqualTo("type"));
        }

        [Test]
        public void BuilderThrowsWithEmptyDerivedClass()
        {
            Assert.That(
                () => _registryBuilder.Add(typeof(IDerivedButEmptyJavaScriptModule)),
                Throws.ArgumentException.With.Property("ParamName").EqualTo("type"));
        }

        [Test]
        public void BuilderThrowsWithUnlreatedClass()
        {
            Assert.That(
                () => _registryBuilder.Add(typeof(object)),
                Throws.ArgumentException.With.Property("ParamName").EqualTo("type"));
        }
        [Test]
        public void BuilderThrowsWithNonEmptyDerivedClassWithoutCtor()
        {
            Assert.That(
                () => _registryBuilder.Add(typeof(NoPublicConstructorJavaScriptModule)),
                Throws.ArgumentException.With.Property("ParamName").EqualTo("type"));
        }
#endif

        public interface IDerivedButEmptyJavaScriptModule : IJavaScriptModule
        {
        }

        public class NoPublicConstructorJavaScriptModule : JavaScriptModuleBase
        {
            private NoPublicConstructorJavaScriptModule() { }
        }

        protected class TestJavaScriptModule : JavaScriptModuleBase
        {
            public void Bar()
            {
                Invoke();
            }

            public void Baz()
            {
                Invoke();
            }

            public void Foo(int x)
            {
                Invoke(x);
            }
        }
    }
}