/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import fs from '@react-native-windows/fs';
import {getFinalModuleName} from './moduleNameUtils';

export async function generateStubFiles(
  root: string,
  actualModuleName?: string,
  logging?: boolean,
): Promise<{actualProjectPath?: string}> {
  if (logging) {
    console.log('[SetupModuleWindows] Generating C++ stub files...');
  }

  // Look for codegen directory
  let codegenDir = path.join(root, 'codegen');
  if (!(await fs.exists(codegenDir))) {
    if (logging) {
      console.log(
        '[SetupModuleWindows] No codegen directory found, skipping stub generation',
      );
    }
    return {};
  }

  const files = await fs.readdir(codegenDir);
  const specFiles = files.filter(file => file.endsWith('Spec.g.h'));

  if (specFiles.length === 0) {
    if (logging) {
      console.log('[SetupModuleWindows] No Spec.g.h files found in codegen directory');
    }
    return {};
  }

  if (logging) {
    console.log(
      `[SetupModuleWindows] Found ${specFiles.length} codegen spec file(s): ${specFiles.join(', ')}`,
    );
  }

  const windowsDir = path.join(root, 'windows');
  const moduleName = await getFinalModuleName(root, actualModuleName);
  let moduleDir = path.join(windowsDir, moduleName);
  let actualProjectPath: string | undefined;

  // If the expected directory doesn't exist, find any existing project directory
  if (!(await fs.exists(moduleDir))) {
    try {
      const windowsDirContents = await fs.readdir(windowsDir);
      const projectDirs = [];

      for (const item of windowsDirContents) {
        const itemPath = path.join(windowsDir, item);
        const stats = await fs.stat(itemPath);

        if (
          stats.isDirectory() &&
          !item.startsWith('.') &&
          item !== 'ExperimentalFeatures.props' &&
          !item.endsWith('.sln')
        ) {
          // Check if this directory contains typical project files
          const possibleHeaderFile = path.join(itemPath, `${item}.h`);
          const possibleCppFile = path.join(itemPath, `${item}.cpp`);
          if (
            (await fs.exists(possibleHeaderFile)) ||
            (await fs.exists(possibleCppFile))
          ) {
            projectDirs.push(item);
          }
        }
      }

      if (projectDirs.length > 0) {
        const projectName = projectDirs[0];
        moduleDir = path.join(windowsDir, projectName);
        actualProjectPath = path.join('windows', projectName, projectName);
        if (logging) {
          console.log(
            `[SetupModuleWindows] Found existing Windows project directory: ${moduleDir}`,
          );
        }
      } else {
        await fs.mkdir(moduleDir, {recursive: true});
        actualProjectPath = path.join('windows', moduleName, moduleName);
      }
    } catch (error) {
      if (logging) {
        console.log(
          `[SetupModuleWindows] Error searching for Windows project directory: ${error}`,
        );
      }
      await fs.mkdir(moduleDir, {recursive: true});
      actualProjectPath = path.join('windows', moduleName, moduleName);
    }
  } else {
    actualProjectPath = path.join('windows', moduleName, moduleName);
  }

  const projectName = path.basename(moduleDir);

  // Generate header and cpp files
  const headerPath = path.join(moduleDir, `${projectName}.h`);
  const cppPath = path.join(moduleDir, `${projectName}.cpp`);

  const headerContent = await generateHeaderStub(projectName);
  await fs.writeFile(headerPath, headerContent);
  if (logging) {
    console.log(`[SetupModuleWindows] Generated header stub: ${headerPath}`);
  }

  const cppContent = await generateCppStub(projectName);
  await fs.writeFile(cppPath, cppContent);
  if (logging) {
    console.log(`[SetupModuleWindows] Generated cpp stub: ${cppPath}`);
  }

  return {actualProjectPath};
}

async function generateHeaderStub(moduleName: string): Promise<string> {
  const namespace = `${moduleName}Specs`;
  const codegenNamespace = `${moduleName}Codegen`;

  return `#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native${moduleName}DataTypes.g.h")
  #include "codegen/Native${moduleName}DataTypes.g.h"
#endif
#include "codegen/Native${moduleName}Spec.g.h"

#include "NativeModules.h"

namespace winrt::${namespace}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE(${moduleName})
struct ${moduleName}
{
  using ModuleSpec = ${codegenNamespace}::${moduleName}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  // Reference function for demonstration (from cpp-lib template)
  // double multiply(double a, double b) noexcept { return a * b; }

  // Hello World example to verify module functionality
  REACT_METHOD(sayHello)
  void sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept;

  // TODO: Add your method implementations here
  // Example:
  // REACT_METHOD(getString)
  // void getString(std::string value, std::function<void(std::string)> const & callback) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::${namespace}`;
}

async function generateCppStub(moduleName: string): Promise<string> {
  const namespace = `${moduleName}Specs`;

  return `#include "${moduleName}.h"

namespace winrt::${namespace} {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ${moduleName}::sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept {
  std::string result = "Hello " + name + "! Module is working.";
  callback(result);
}

// TODO: Implement your methods here
// Example:
// void ${moduleName}::getString(std::string value, std::function<void(std::string)> const & callback) noexcept {
//   callback("Hello " + value);
// }

} // namespace winrt::${namespace}
`;
}