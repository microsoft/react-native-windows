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
    public class JavaScriptModuleRegistryTests : JavaScriptModuleRegistrySharedTests
    {
        private MockFactory _mockFactory;
        private Mock<IReactInstance> _mockReactInstance;

        [SetUp]
        public override void SetUp()
        {
            base.SetUp();

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
    }
}