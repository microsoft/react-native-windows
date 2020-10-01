## Testing NuGet package for new react-native init templates.
Today's default is to use the C++ and C# code from the `react-native-windows` npm package as source. We have been working on moving to a binary distribution model for this code via NuGet. 

Today we support the following projects as a NuGet package:
 * Microsoft.ReactNative
 * Microsoft.ReactNative.Managed

 Testing NuGet packages is frequently a painful process as msbuild and NuGet lack referential transparency. The steps in this document hope to alleviate some of the challenges.

## Complicating factors
* NuGet packages can only be loaded from a feed
  > These scripts help maintain a test feed on your local disk
* Each change requires a version bump.
  NuGet caches by directory and simply checks if a directory with that version exists, regardless of whether the package has changed. It has implicit assumptions all up its stack that versions are immutable. Therefore each time you create a new package, you are forced to bump the versionpackage, you are forced to bupm the version
* UWP NuGet packages contain multiple slices specific for each platform.
  > These scripts allow you to filter to specific slices so you don't have to build all flavors


## Steps
1. Decide which slices you want. The default is a single slice `'Debug,X64'` but more are supported. 
1. Decide on the version scheme. Recommended is: `0.0.1-MyTest001` and keeping bumping the last 3 digigs
1. Build the bits in your repo for slice you selected
   1. open terminal for your enlistment (in vs dev prompt if you haven't setup your box otherwise)
   1. yarn
   1. yarn build
   1. pushd vnext
   1. NuGet restore Microsoft.ReactNative.sln
   1. msbuild Microsoft.ReactNative.sln /bl /m /p:Configuration=Debug /p:Platform=x64
   1. Repeat the preivous msbuild step for any other slices you might want to include
1. Publish the NuGet packages
   1. open terminal for your enlistment
   1. `vnext\Scripts\PublishNuGetPackagesLocally.cmd 0.0.1-MyTest001`
   > See below for a breakdown of the arguments
1. Link your repo for fast CLI testing
   1. open terminal for your enlsitment
   1. push vnext
   1. yarn link
      > if it fails with already being registred for anotehr enlistment you can run `yarn unlink`
1. Generate a test project
   1. open terminal in a test folder
   1. `npx react-native init MyProj`
   1. cd `MyProj`
   1. Apply the react-native-windows template:
      1. `node z:\src\r3\packages\react-native-windows-init\lib-commonjs\Cli.js --useDevMode --overwrite  --language cs --experimentalNuGetDependency --nuGetTestFeed c:\temp\RnWNuGetTesting\feed --nuGetTestVersion 0.0.1-MyTest001 `
         > See below for a breakdown of the arguments:
1. Deal with npm / nuget version mismatch
   1. The nuget package checks to see if there is a version match between the nuget package and the npm package.
      In testing these likely don't line up. You'll get the following error:
      ```
      error : Mismatch detected between npm package versions and nuget package version.
      error :     Npm:   '0.0.0-canary.148' - Z:\Temp\tst2\node_modules\react-native-windows\package.json
      error :     NuGet: '0.0.1-MyTest010' - Z:\Temp\tst2\windows\tst2\tsts2.csproj
      error : To update the nuget version, please see https://microsoft.github.io/react-native-windows/docs/nuget-update for instructions
      ```
      
      You can disable the check by setting the environment variable:

      `set ReactNativeWindowsAllowNugetNpmMismatch=true`
1. Do your testing

## Changes:
* Changes to the binaries in the NuGet package
  1. Rerun the builds for the slices you use
  1. Rerun publish NuGet script bumping the version number by one
  1. Reapply the windows template with the new version number
  1. Do your tests
* Change to the template generation
  1. yarn build
  1. Reapply the windows template
  1. Do your tests

## Scripts
### PublishNugetPackagesLocally
This script takes more options for more customization: `<version> [<targetDir>] [<slices>] [<baseConfiguration>] [<basePlatform>]`.
      
Let's break down all arguments:
* `targetDir`: Where the packages and feed should be placed. Default is `c:\temp\RnWNugetTesting`
* `slices`: This is the set of slices to produce in powershell array syntax. The default is: `@('x64.Debug')`. An example of more slices would be: `@('x64.Debug', 'X86.Release', 'ARM64.Release')`
* `baseConfiguration`: The slice that is used for the non configuration specific build outputs. Default is: `Debug`
* `basePlatform`: The slice that is used for the non platform specific build outputs. Default is `x64`

### react-native-windows-init ... cli.js
This script has many options, but let's break down the ones from the sample:
* `--version 0.0.0 --useDevMode`: The version doesn't matter when using dev mode, which means that the npm package is used that is registred as the dev package using `yarn link` per instructions above. This saves copying all the output files to your package directory, to your yarn cache. It is also a lot faster and since it is a link, you can make changes to your enlistment i.e. to `vnext\StyleSheets` and you don't have to reinstall between testing
* `--overwrite`: This just overwites files that already exist instead of prompting. This is very usefull when running the script multiple times to test the templates or to code that emits the template.
* `--language cs`: this is one of the public flags to generate a cs project instead of a cpp project. 
* `--experimentalNuGetDependency`: This is the critical one to let the template emit referneces to NuGet package rather than source pacakges. When we make it the default this will likely be renamed back to source for those that need a tight loop between test projects from template and the product code for developers working 'on' react-native-windows. If it proves that the sample and test apps we have in the repo satisfy this need, it can be removed and the tempaltes can be cleaned up by removing a lot of the conditionals bewteen the two.
* `--nuGetTestFeed c:\temp\RnWNugetTesting\feed`: This flag adds an extra feed to the nuget.config so that for local testing you can use a different feed. This is the default location that is pushed to by the `PublishNugetPackagesLocally.cmd` script.
* `--nuGetTestVersion 0.0.1-MyTest001`: By default the NuGet version matches the react-native version. For testing this can be cumbersome. Therefore this flag allows you to specify the version number for the NuGet packages. 
