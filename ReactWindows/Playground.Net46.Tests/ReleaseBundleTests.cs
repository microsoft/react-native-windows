using NUnit.Framework;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Remote;
using System;

namespace Playground.Net46.Tests
{
    [TestFixture]
    public class ReleaseBundleTests
    {
        private const string WindowsApplicationDriverUrl = "http://127.0.0.1:4723";
            
        private WindowsElement welcomeText;

        private WindowsDriver<WindowsElement> session;

        private static readonly string PlaygroundAppDirectory = Environment.GetEnvironmentVariable("playgroundNet46_dir") + @"\bin\" +
                                                                (Environment.GetEnvironmentVariable("PLATFORM") ?? "x86") + @"\" +
                                                                (Environment.GetEnvironmentVariable("CONFIGURATION") ?? "ReleaseBundle");

        private static readonly string PlaygroundAppExecutable =
            PlaygroundAppDirectory + @"\" +
            "Playground.Net46.exe";

        [SetUp]
        public void Setup()
        {
            if (session != null) return;

            var appCapabilities = new DesiredCapabilities();
            appCapabilities.SetCapability("app", PlaygroundAppExecutable);
            appCapabilities.SetCapability("deviceName", "WindowsPC");
            appCapabilities.SetCapability("appWorkingDir", PlaygroundAppDirectory);
            session = new WindowsDriver<WindowsElement>(new Uri(WindowsApplicationDriverUrl), appCapabilities);
            Assert.That(session, Is.Not.Null);
            session.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(10);

            welcomeText = session.FindElementByAccessibilityId("WelcomeText");

            Assert.That(welcomeText, Is.Not.Null);

        }

        [TearDown]
        public void TearDown()
        {
            if (session == null) return;

            session.Quit();
            session = null;
        }

        [Test]
        public void WelcomeText()
        {
            Assert.That("Welcome to React Native!", Is.EqualTo(welcomeText.Text));
        }
    }
}
