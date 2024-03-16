# Integration Test Website

This is a generic ASP.NET Core application that provides arbitrary HTTP responses and WebSocket endpoints to validate React Native Windows's functionality.

It is meant to complement (and partially replace) the in-process C++ test servers used for integration  which have very limited functionality and are hard to program against.

This application enhances web testing via a number of features such as:
- Server-side Origin Policy (CORS) enforcement
- Static file serving
- HTTP authentication and authorization
- HTTP sessions
- Response caching
- Cookie handling

Deploying this website in Continuous Integration/Delivery environments also removes the dependency on remote web endpoints for end to end scenarios.

## Usage

### Developer Workflow

1.  Build React Native Windows.
1.  Start the Node Packager.
    ```pwsh
    Set-Location vnext
    npm run start
    ```
1.  Trust the .NET-provided development SSL certificate:\
    This should be a one-time step per .NET version.\
    See https://learn.microsoft.com/en-us/aspnet/core/security/enforcing-ssl#trust-the-aspnet-core-https-development-certificate-on-windows-and-macos.
    ```cmd
    dotnet dev-certs https --trust
    ```
1.  Start this website.
    ```cmd
    dotnet run --project vnext\TestWebSite
    ```
1.  Run integration tests.

### Continuous Integration Workflow
The website gets deployed to a web server.\
This reduces the process management overhead and increases stability and availability in CI environments.

The website can also be installed on a development environment.\
*Note: This would require to update the website's publish directory with any new changes, but removes the need to manage the additonal process and terminal window.*

Here are the steps for Micrososoft's IIS:
1.  Publish the website.
    ```pwsh
    Set-Location vnext\TestWebSite
    dotnet publish
    ```
1.  Install IIS.
1.  Install the .NET 7 SDK.\
    https://dotnet.microsoft.com/en-us/download/dotnet/thank-you/sdk-7.0.407-windows-x64-installer
1.  Install the ASP.NET Core Hosting Bundle.\
    https://dotnet.microsoft.com/en-us/download/dotnet/thank-you/runtime-aspnetcore-8.0.3-windows-hosting-bundle-installer
1.  Create or update a website that binds ports `5555` to `http` and `5543` to `https`.\
    *For `https`, any trusted SSL certificate will work. There is no need to install `localhost.cicert`.*
    1.  Set the location to the publish directory (i.e. `C:\repo\react-native-windows\vnext\target\x64\Debug\Microsoft.ReactNative.Test.Website\Publish`)

## Technical Information

- The website is meant to run on ports `5555` (`http`, `ws`) and `5543` (`https`, `wss`).\
  This can be modified via command line arguments or in [Properties/launchSettings.json](Properties/launchSettings.json).
- For encryption, self-signed PKCS12 certificate `localhost.cicert` is used (DevOps only).
  The certificate is set to expire on January 1st, 2030.\
  After that date, new one must be generated.

### Replacing the DevOps SSL certificate

1.  Generate a new self-signed x509 web certificate.\
    Use common name `CN=localhost`.
1.  Replace the `localhost.cicert` with the new certificate.
1.  Update the password in the Continuous Integration system.\
    *The certificate password is mapped to DevOps variable `$(TestWebsiteCertPassword)`.*
