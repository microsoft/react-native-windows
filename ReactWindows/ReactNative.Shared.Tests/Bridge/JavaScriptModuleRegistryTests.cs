using Newtonsoft.Json.Linq;
using NMock;
using NMock.Syntax;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistryTests
    {
        private MockFactory _mockFactory;
        private Mock<IReactInstance> _mockReactInstance;
        private JavaScriptModuleRegistry _registry;
        private JavaScriptModuleRegistry.Builder _registryBuilder;


        [SetUp]
        public void SetUp()
        {
            _registryBuilder = new JavaScriptModuleRegistry.Builder();
            _mockFactory = new MockFactory();
            _mockReactInstance = _mockFactory.CreateMock<IReactInstance>();
        }

        [Test]
        public void InvokesReactInstanceWhenFetchedModuleIsCalled()
        {
            _registry = _registryBuilder
                .Add(typeof(RCTEventEmitter))
                .Add(typeof(AppRegistry))
                .Add(typeof(TestJavaScriptModule))
                .Build();

            var are = new AutoResetEvent(false);

            var module = _registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance.MockObject);

            _mockReactInstance.Expects.One.Method(
                _ => _.InvokeFunction(null, null, null, null))
                .With(
                    nameof(TestJavaScriptModule),
                    nameof(TestJavaScriptModule.Foo),
                    JArray.FromObject(new[] { 42 }),
                    NMock.Is.StringContaining("_"))
                .Will(Invoke.Action(() => are.Set()));

            module.Foo(42);

            are.WaitOne();
        }

        [Test]
        public void ThrowsWhenUnknownModuleRequested()
        {
            _registry = _registryBuilder.Build();
            Assert.That(
                () => _registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance.MockObject),
                Throws.InvalidOperationException);
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

        class TestJavaScriptModule : JavaScriptModuleBase
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