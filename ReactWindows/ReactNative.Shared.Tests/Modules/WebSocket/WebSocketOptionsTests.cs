using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Modules.WebSocket;

namespace ReactNative.Tests.Modules.WebSocket
{
    [TestFixture]
    public class WebSocketOptionsTests
    {
        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_NoProxy()
        {
            var json = @"{
    origin: 'www.example.com'
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.UserName, Is.Empty);
            Assert.That(options.ProxyAddress, Is.Empty);
            Assert.That(options.Password, Is.Empty);
            Assert.That(options.Origin, Is.Empty);

            var noOptions = new WebSocketOptions(null);
            Assert.That(noOptions.UseDefaultProxy, Is.False);
            Assert.That(noOptions.UserName, Is.Empty);
            Assert.That(noOptions.ProxyAddress, Is.Empty);
            Assert.That(noOptions.Password, Is.Empty);
            Assert.That(noOptions.Origin, Is.Empty);
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_ProxyIsNull()
        {
            var json = @"{
    origin: 'www.example.com',
    proxy: null
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.UserName, Is.Empty);
            Assert.That(options.ProxyAddress, Is.Empty);
            Assert.That(options.Password, Is.Empty);
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_ProxyIsEmpty()
        {
            var json = @"{
    origin: 'www.example.com',
    proxy: {}
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.UserName, Is.Empty);
            Assert.That(options.ProxyAddress, Is.Empty);
            Assert.That(options.Password, Is.Empty);
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_ProxyIsNotValid()
        {
            var json = @"{
    origin: 'www.example.com',
    proxy: 'not valid format'
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.UserName, Is.Empty);
            Assert.That(options.ProxyAddress, Is.Empty);
            Assert.That(options.Password, Is.Empty);
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_DefaultProxy()
        {
            var json = @"{
    origin: 'www.example.com',
    proxy: {
        useDefault: true
    }
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.True);
            Assert.That(options.UserName, Is.Empty);
            Assert.That(options.ProxyAddress, Is.Empty);
            Assert.That(options.Password, Is.Empty);
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_ExplicitProxy()
        {
            var json = @"{
    origin: 'www.example.com',
    proxy: {
        uri: '10.10.10.10:3128',
        user: 'myName',
        password: 'p@ssw0rd'
    }
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.ProxyAddress, Is.EqualTo(@"10.10.10.10:3128"));
            Assert.That(options.UserName, Is.EqualTo("myName"));
            Assert.That(options.Password, Is.EqualTo("p@ssw0rd"));
        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_Origin()
        {
            var json = @"{
    headers: {
        origin: 'www.example.com'
    },
    proxy: {}
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.UseDefaultProxy, Is.False);
            Assert.That(options.Origin, Is.EqualTo("www.example.com"));

        }

        [Test]
        [Category("WebSocket")]
        public void WebSocketOptions_NoOrigin()
        {
            var json = @"{
    headers: {
    }
}";
            var jOptions = JObject.Parse(json);
            var options = new WebSocketOptions(jOptions);
            Assert.That(options.Origin, Is.Empty);
        }

    }
}
