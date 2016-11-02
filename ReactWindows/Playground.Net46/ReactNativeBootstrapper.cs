using ReactNative;
using System;
using System.Windows;

namespace Playground.Net46
{
    public abstract class ReactNativeBootstrapper
    {
        #region Protected Properties

        protected string Arguments { get; private set; }

        protected ReactPage RootPage { get; private set; }

        protected DependencyObject Shell { get; private set; }

        #endregion

        #region Public Methods

        public void Run()
        {
            this.Run("");
        }

        #endregion

        #region Protected Virtual Methods

        protected virtual ReactPage CreateRootPage()
        {
            return null;
        }

        protected virtual void InitializeRootPage()
        {
            if (this.RootPage == null)
            {
                throw new InvalidOperationException("RootPage is null");
            }

            this.RootPage.OnResume(() =>
            {
                //OnBackPressed
                //Application.Current.Shutdown(0);
            });

            this.RootPage.OnCreate(this.Arguments);
        }

        protected virtual DependencyObject CreateShell()
        {
            return null;
        }

        protected virtual void InitializeShell()
        {
        }

        public virtual void Run(string arguments)
        {
            this.Arguments = arguments;

            this.RootPage = this.CreateRootPage();

            if (this.RootPage == null)
            {
                throw new InvalidOperationException("RootPage is null");
            }

            if (this.RootPage != null)
            {
                this.InitializeRootPage();
            }

            this.Shell = this.CreateShell();

            if (this.Shell != null)
            {
                this.InitializeShell();
            }
        }

        #endregion
    }
}
