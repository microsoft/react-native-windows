"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.installDependencies = exports.copyProjectTemplateAndReplace = void 0;
const chalk = require("chalk");
const path = require("path");
const username = require("username");
const uuid = require("uuid");
const childProcess = require("child_process");
const fs = require("fs");
const os = require("os");
const semver = require("semver");
const configUtils_1 = require("../config/configUtils");
const generator_common_1 = require("../generator-common");
const windowsDir = 'windows';
const bundleDir = 'Bundle';
async function generateCertificate(srcPath, destPath, newProjectName, currentUser) {
    console.log('Generating self-signed certificate...');
    let toCopyTempKey = false;
    if (os.platform() === 'win32') {
        try {
            const timeout = 10000; // 10 seconds;
            const thumbprint = childProcess
                .execSync(`powershell -command "Write-Output (New-SelfSignedCertificate -KeyUsage DigitalSignature -KeyExportPolicy Exportable -Subject 'CN=${currentUser}' -TextExtension @('2.5.29.37={text}1.3.6.1.5.5.7.3.3', '2.5.29.19={text}Subject Type:End Entity') -CertStoreLocation 'Cert:\\CurrentUser\\My').Thumbprint"`, { timeout })
                .toString()
                .trim();
            if (!fs.existsSync(path.join(windowsDir, newProjectName))) {
                fs.mkdirSync(path.join(windowsDir, newProjectName));
            }
            childProcess.execSync(`powershell -command "$pwd = (ConvertTo-SecureString -String password -Force -AsPlainText); Export-PfxCertificate -Cert 'cert:\\CurrentUser\\My\\${thumbprint}' -FilePath ${path.join(windowsDir, newProjectName, newProjectName)}_TemporaryKey.pfx -Password $pwd"`, { timeout });
            console.log(chalk.green('Self-signed certificate generated successfully.'));
            return thumbprint;
        }
        catch (err) {
            console.log(chalk.yellow('Failed to generate Self-signed certificate. Using Default Certificate. Use Visual Studio to renew it.'));
            toCopyTempKey = true;
        }
    }
    else {
        console.log(chalk.yellow('Using Default Certificate. Use Visual Studio to renew it.'));
        toCopyTempKey = true;
    }
    if (toCopyTempKey) {
        await generator_common_1.copyAndReplaceWithChangedCallback(path.join(srcPath, 'keys', 'MyApp_TemporaryKey.pfx'), destPath, path.join(windowsDir, newProjectName, newProjectName + '_TemporaryKey.pfx'));
    }
    return null;
}
async function copyProjectTemplateAndReplace(srcRootPath, destPath, newProjectName, namespace, options) {
    if (!srcRootPath) {
        throw new Error('Need a path to copy from');
    }
    if (!destPath) {
        throw new Error('Need a path to copy to');
    }
    if (!newProjectName) {
        throw new Error('Need a project name');
    }
    generator_common_1.createDir(path.join(destPath, windowsDir));
    generator_common_1.createDir(path.join(destPath, windowsDir, newProjectName));
    generator_common_1.createDir(path.join(destPath, windowsDir, newProjectName, bundleDir));
    generator_common_1.createDir(path.join(destPath, windowsDir, newProjectName, 'BundleBuilder'));
    const language = options.language;
    const namespaceCpp = toCppNamespace(namespace);
    if (options.experimentalNuGetDependency) {
        console.log('Using experimental NuGet dependency.');
    }
    if (options.useWinUI3) {
        console.log('Using experimental WinUI3 dependency.');
        fs.writeFileSync(path.join(destPath, windowsDir, 'UseWinUI3'), '');
    }
    const projDir = 'proj';
    const srcPath = path.join(srcRootPath, language);
    const sharedPath = path.join(srcRootPath, 'shared');
    const projectGuid = uuid.v4();
    const rnwVersion = require('react-native-windows/package.json').version;
    const nugetVersion = options.nuGetTestVersion || rnwVersion;
    const packageGuid = uuid.v4();
    const currentUser = username.sync(); // Gets the current username depending on the platform.
    const certificateThumbprint = await generateCertificate(srcPath, destPath, newProjectName, currentUser);
    const xamlNamespace = options.useWinUI3
        ? 'Microsoft.UI.Xaml'
        : 'Windows.UI.Xaml';
    const xamlNamespaceCpp = toCppNamespace(xamlNamespace);
    const winui3PropsPath = require.resolve('react-native-windows/PropertySheets/WinUI.props', { paths: [process.cwd()] });
    const winui3Props = configUtils_1.readProjectFile(winui3PropsPath);
    const winui3Version = configUtils_1.findPropertyValue(winui3Props, 'WinUI3Version', winui3PropsPath);
    const csNugetPackages = [
        {
            id: 'Microsoft.NETCore.UniversalWindowsPlatform',
            version: '6.2.9',
        },
    ];
    const cppNugetPackages = [
        {
            id: 'Microsoft.Windows.CppWinRT',
            version: '2.0.200615.7',
            propsTopOfFile: true,
            hasProps: true,
            hasTargets: true,
        },
        {
            id: options.useWinUI3 ? 'Microsoft.WinUI' : 'Microsoft.UI.Xaml',
            version: options.useWinUI3 ? winui3Version : '2.3.191129002',
            hasProps: false,
            hasTargets: false,
        },
    ];
    if (options.experimentalNuGetDependency) {
        csNugetPackages.push({
            id: 'Microsoft.ReactNative.Managed',
            version: nugetVersion,
        });
        cppNugetPackages.push({
            id: 'Microsoft.ReactNative',
            version: nugetVersion,
            hasProps: false,
            hasTargets: true,
        });
        cppNugetPackages.push({
            id: 'Microsoft.ReactNative.Cxx',
            version: nugetVersion,
            hasProps: false,
            hasTargets: true,
        });
    }
    const templateVars = {
        useMustache: true,
        regExpPatternsToRemove: [],
        name: newProjectName,
        namespace: namespace,
        namespaceCpp: namespaceCpp,
        languageIsCpp: language === 'cpp',
        // Visual Studio is very picky about the casing of the guids for projects, project references and the solution
        // https://www.bing.com/search?q=visual+studio+project+guid+casing&cvid=311a5ad7f9fc41089507b24600d23ee7&FORM=ANAB01&PC=U531
        // we therefore have to precariously use the right casing in the right place or risk building in VS breaking.
        projectGuidLower: `{${projectGuid.toLowerCase()}}`,
        projectGuidUpper: `{${projectGuid.toUpperCase()}}`,
        // packaging and signing variables:
        packageGuid: packageGuid,
        currentUser: currentUser,
        certificateThumbprint: certificateThumbprint,
        useExperimentalNuget: options.experimentalNuGetDependency,
        nuGetTestFeed: options.nuGetTestFeed,
        // cpp template variables
        useWinUI3: options.useWinUI3,
        xamlNamespace: xamlNamespace,
        xamlNamespaceCpp: xamlNamespaceCpp,
        cppNugetPackages: cppNugetPackages,
        // cs template variables
        csNugetPackages: csNugetPackages,
        // autolinking template variables
        autolinkPropertiesForProps: '',
        autolinkProjectReferencesForTargets: '',
        autolinkCsUsingNamespaces: '',
        autolinkCsReactPackageProviders: '',
        autolinkCppIncludes: '',
        autolinkCppPackageProviders: '\n    UNREFERENCED_PARAMETER(packageProviders);',
    };
    const commonMappings = [
        { from: path.join(srcRootPath, 'metro.config.js'), to: 'metro.config.js' },
        {
            from: path.join(srcRootPath, '_gitignore'),
            to: path.join(windowsDir, '.gitignore'),
        },
        {
            from: path.join(srcRootPath, 'b_gitignore'),
            to: path.join(windowsDir, newProjectName, '.gitignore'),
        },
        {
            from: path.join(srcRootPath, 'index.windows.bundle'),
            to: path.join(windowsDir, newProjectName, bundleDir, 'index.windows.bundle'),
        },
        {
            from: path.join(srcPath, projDir, 'MyApp.sln'),
            to: path.join(windowsDir, newProjectName + '.sln'),
        },
    ];
    for (const mapping of commonMappings) {
        await generator_common_1.copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite);
    }
    if (language === 'cs') {
        const csMappings = [
            {
                from: path.join(srcPath, projDir, 'MyApp.csproj'),
                to: path.join(windowsDir, newProjectName, newProjectName + '.csproj'),
            },
        ];
        for (const mapping of csMappings) {
            await generator_common_1.copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite);
        }
    }
    else {
        const cppMappings = [
            {
                from: path.join(srcPath, projDir, 'MyApp.vcxproj'),
                to: path.join(windowsDir, newProjectName, newProjectName + '.vcxproj'),
            },
            {
                from: path.join(srcPath, projDir, 'MyApp.vcxproj.filters'),
                to: path.join(windowsDir, newProjectName, newProjectName + '.vcxproj.filters'),
            },
            {
                from: path.join(srcPath, projDir, 'packages.config'),
                to: path.join(windowsDir, newProjectName, 'packages.config'),
            },
        ];
        for (const mapping of cppMappings) {
            await generator_common_1.copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite);
        }
    }
    // Once we are publishing to nuget.org, this shouldn't be needed anymore
    if (options.experimentalNuGetDependency) {
        [
            {
                from: path.join(sharedPath, projDir, 'NuGet.Config'),
                to: path.join(windowsDir, 'NuGet.Config'),
            },
        ].forEach(mapping => generator_common_1.copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars, options.overwrite));
    }
    await generator_common_1.copyAndReplaceAll(path.join(sharedPath, 'assets'), destPath, path.join(windowsDir, newProjectName, 'Assets'), templateVars, options.overwrite);
    await generator_common_1.copyAndReplaceAll(path.join(sharedPath, 'src'), destPath, path.join(windowsDir, newProjectName), templateVars, options.overwrite);
    await generator_common_1.copyAndReplaceAll(path.join(srcPath, 'src'), destPath, path.join(windowsDir, newProjectName), templateVars, options.overwrite);
    console.log(chalk.white.bold('To run your app on UWP:'));
    console.log(chalk.white('   npx react-native run-windows'));
}
exports.copyProjectTemplateAndReplace = copyProjectTemplateAndReplace;
function toCppNamespace(namespace) {
    return namespace.replace(/\./g, '::');
}
function installDependencies(options) {
    const cwd = process.cwd();
    // Extract react-native peer dependency version
    const rnwPackageJsonPath = require.resolve('react-native-windows/package.json', { paths: [process.cwd()] });
    const rnwPackageJson = JSON.parse(fs.readFileSync(rnwPackageJsonPath, { encoding: 'UTF8' }));
    let rnPeerDependency = rnwPackageJson.peerDependencies['react-native'];
    const depDelim = ' || ';
    const delimIndex = rnPeerDependency.indexOf(depDelim);
    if (delimIndex !== -1) {
        rnPeerDependency = rnPeerDependency.slice(0, delimIndex);
    }
    const rnPackageJsonPath = require.resolve('react-native/package.json', {
        paths: [process.cwd()],
    });
    const rnPackageJson = JSON.parse(fs.readFileSync(rnPackageJsonPath, { encoding: 'UTF8' }));
    if (!semver.satisfies(rnPackageJson.version, rnPeerDependency)) {
        console.log(chalk.green('Installing a compatible version of react-native:'));
        console.log(chalk.white(`    ${rnPeerDependency}`));
        // Patch package.json to have proper react-native version and install
        const projectPackageJsonPath = path.join(cwd, 'package.json');
        const projectPackageJson = JSON.parse(fs.readFileSync(projectPackageJsonPath, { encoding: 'UTF8' }));
        projectPackageJson.scripts.windows = 'react-native run-windows';
        projectPackageJson.dependencies['react-native'] = rnPeerDependency;
        fs.writeFileSync(projectPackageJsonPath, JSON.stringify(projectPackageJson, null, 2));
        // Install dependencies using correct package manager
        const isYarn = fs.existsSync(path.join(cwd, 'yarn.lock'));
        childProcess.execSync(isYarn ? 'yarn' : 'npm i', options.verbose ? { stdio: 'inherit' } : {});
    }
}
exports.installDependencies = installDependencies;
//# sourceMappingURL=index.js.map