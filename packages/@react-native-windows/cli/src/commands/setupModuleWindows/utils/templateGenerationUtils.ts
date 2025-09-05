/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import fs from '@react-native-windows/fs';
import {getFinalModuleName} from './moduleNameUtils';

interface MethodSignature {
  name: string;
  returnType: string;
  parameters: {name: string; type: string; fullType?: string}[];
}

export async function generateStubFiles(
  root: string,
  actualModuleName?: string,
  logging?: boolean,
): Promise<{actualProjectPath?: string}> {
  if (logging) {
    console.log('[SetupModuleWindows] Generating C++ stub files...');
  }

  // First, find the actual Windows project directory that was created by init-windows
  const windowsDir = path.join(root, 'windows');
  let actualProjectName = '';
  
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
      actualProjectName = projectDirs[0];
      if (logging) {
        console.log(
          `[SetupModuleWindows] Found Windows project directory: ${actualProjectName}`,
        );
      }
    }
  } catch (error) {
    if (logging) {
      console.log(
        `[SetupModuleWindows] Error searching for Windows project directory: ${error}`,
      );
    }
  }

  // Look for codegen directory - try multiple possible locations
  let codegenDir = path.join(root, 'codegen');
  let foundCodegen = false;
  
  // Check root level first
  if (await fs.exists(codegenDir)) {
    foundCodegen = true;
    if (logging) {
      console.log(`[SetupModuleWindows] Found codegen directory at: ${codegenDir}`);
    }
  } 
  // Check inside Windows project directory
  else if (actualProjectName) {
    const projectCodegenDir = path.join(windowsDir, actualProjectName, 'codegen');
    if (await fs.exists(projectCodegenDir)) {
      codegenDir = projectCodegenDir;
      foundCodegen = true;
      if (logging) {
        console.log(`[SetupModuleWindows] Found codegen directory at: ${codegenDir}`);
      }
    }
    // Also check with different casing (Codegen vs codegen)
    else {
      const projectCodegenDirAlt = path.join(windowsDir, actualProjectName, 'Codegen');
      if (await fs.exists(projectCodegenDirAlt)) {
        codegenDir = projectCodegenDirAlt;
        foundCodegen = true;
        if (logging) {
          console.log(`[SetupModuleWindows] Found codegen directory at: ${codegenDir}`);
        }
      }
    }
  }

  if (!foundCodegen) {
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

  // Use the actual project name we found, or fall back to calculated module name
  let projectName = actualProjectName;
  let actualProjectPath: string | undefined;
  let moduleDir: string;

  if (projectName) {
    // Use the actual project directory that was found
    moduleDir = path.join(windowsDir, projectName);
    actualProjectPath = path.join('windows', projectName, projectName);
    if (logging) {
      console.log(
        `[SetupModuleWindows] Using existing Windows project directory: ${moduleDir}`,
      );
    }
  } else {
    // Fall back to calculated module name if no project directory was found
    const moduleName = await getFinalModuleName(root, actualModuleName);
    projectName = moduleName;
    moduleDir = path.join(windowsDir, moduleName);
    actualProjectPath = path.join('windows', moduleName, moduleName);
    
    if (!(await fs.exists(moduleDir))) {
      await fs.mkdir(moduleDir, {recursive: true});
    }
  }

  // Parse methods from codegen files
  const methods = await parseMethodsFromCodegen(codegenDir, specFiles, logging);

  // Generate header and cpp files
  const headerPath = path.join(moduleDir, `${projectName}.h`);
  const cppPath = path.join(moduleDir, `${projectName}.cpp`);

  const headerContent = await generateHeaderStub(projectName, methods);
  await fs.writeFile(headerPath, headerContent);
  if (logging) {
    console.log(`[SetupModuleWindows] Generated header stub: ${headerPath} with ${methods.length} methods`);
  }

  const cppContent = await generateCppStub(projectName, methods);
  await fs.writeFile(cppPath, cppContent);
  if (logging) {
    console.log(`[SetupModuleWindows] Generated cpp stub: ${cppPath} with ${methods.length} methods`);
  }

  return {actualProjectPath};
}

async function parseMethodsFromCodegen(
  codegenDir: string,
  specFiles: string[],
  logging?: boolean,
): Promise<MethodSignature[]> {
  const methods: MethodSignature[] = [];

  for (const specFile of specFiles) {
    const specPath = path.join(codegenDir, specFile);
    try {
      const content = await fs.readFile(specPath, 'utf8');
      const fileMethods = extractMethodsFromCodegenHeader(content, logging);
      methods.push(...fileMethods);
      
      if (logging) {
        console.log(
          `[SetupModuleWindows] Extracted ${fileMethods.length} methods from ${specFile}: ${fileMethods.map(m => m.name).join(', ')}`,
        );
      }
    } catch (error) {
      if (logging) {
        console.log(`[SetupModuleWindows] Error parsing ${specFile}: ${error}`);
      }
    }
  }

  return methods;
}

function extractMethodsFromCodegenHeader(content: string, logging?: boolean): MethodSignature[] {
  const methods: MethodSignature[] = [];

  // Parse from REACT_SHOW_METHOD_SPEC_ERRORS sections which contain the exact method signatures
  // Use a more sophisticated pattern to handle nested parentheses and function callbacks
  const errorSectionPattern =
    /REACT_SHOW_METHOD_SPEC_ERRORS\s*\(\s*\d+,\s*"([^"]+)",\s*"([^"]*)"/g;

  let match;
  while ((match = errorSectionPattern.exec(content)) !== null) {
    const methodName = match[1]; // Method name from first parameter
    const fullMethodSignature = match[2]; // Full method signature string
    
    // Extract parameters from the full method signature
    const methodMatch = fullMethodSignature.match(/REACT_METHOD\([^)]+\)\s+(?:static\s+)?void\s+\w+\s*\(([^]*)\)/);
    if (methodMatch) {
      // Need to carefully parse the parameters, handling nested parentheses
      const parametersString = methodMatch[1];
      const parameters = parseComplexParameters(parametersString);

      methods.push({
        name: methodName,
        returnType: 'void', // Codegen methods are typically void with callbacks or promises
        parameters: parameters,
      });
      
      if (logging) {
        console.log(`[SetupModuleWindows] Parsed method ${methodName} with parameters: ${parametersString}`);
      }
    }
  }

  // Also try to parse from the methods tuple for additional methods
  if (methods.length === 0) {
    const methodsTuplePattern =
      /Method<([^>]+)>\{\s*(\d+),\s*L"([^"]+)"\s*\}/g;

    while ((match = methodsTuplePattern.exec(content)) !== null) {
      const signature = match[1]; // Method signature type
      const methodName = match[3]; // Method name
      const parameters = parseCodegenSignature(signature);

      methods.push({
        name: methodName,
        returnType: 'void',
        parameters: parameters,
      });
    }
  }

  // Try parsing directly from C++ method declarations in the header
  if (methods.length === 0) {
    if (logging) {
      console.log('[SetupModuleWindows] Trying to parse C++ method declarations directly...');
    }

    // Look for virtual method declarations like:
    // virtual void MethodName(parameters) = 0;
    const virtualMethodPattern =
      /virtual\s+(\w+(?:\s*\*)?)\s+(\w+)\s*\(([^)]*)\)\s*(?:const\s*)?=\s*0\s*;/g;

    while ((match = virtualMethodPattern.exec(content)) !== null) {
      const returnType = match[1].trim();
      const methodName = match[2];
      const paramString = match[3];
      const parameters = parseCodegenParameters(paramString);

      methods.push({
        name: methodName,
        returnType: returnType === 'void' ? 'void' : returnType,
        parameters: parameters,
      });
    }
  }

  if (logging && methods.length > 0) {
    console.log(
      `[SetupModuleWindows] Extracted ${methods.length} methods from codegen header using multiple parsing strategies`,
    );
  }

  return methods;
}

function parseComplexParameters(paramString: string): {name: string; type: string; fullType?: string}[] {
  if (!paramString || paramString.trim() === '') {
    return [];
  }

  const params: {name: string; type: string; fullType?: string}[] = [];
  
  // Handle complex parameter parsing with nested parentheses
  let current = '';
  let depth = 0;
  let inString = false;
  let i = 0;
  
  while (i < paramString.length) {
    const char = paramString[i];
    
    if (char === '"' && (i === 0 || paramString[i - 1] !== '\\')) {
      inString = !inString;
    } else if (!inString) {
      if (char === '<' || char === '(') {
        depth++;
      } else if (char === '>' || char === ')') {
        depth--;
      } else if (char === ',' && depth === 0) {
        if (current.trim()) {
          params.push(parseCodegenParameter(current.trim()));
        }
        current = '';
        i++;
        continue;
      }
    }
    
    current += char;
    i++;
  }
  
  if (current.trim()) {
    params.push(parseCodegenParameter(current.trim()));
  }
  
  return params;
}

function parseCodegenParameters(paramString: string): {name: string; type: string; fullType?: string}[] {
  if (!paramString || paramString.trim() === '') {
    return [];
  }

  const params: {name: string; type: string; fullType?: string}[] = [];

  // Split parameters carefully, handling nested templates like std::function<void(bool)>
  const cleanParamString = paramString.trim();
  let current = '';
  let depth = 0;
  let inString = false;

  for (let i = 0; i < cleanParamString.length; i++) {
    const char = cleanParamString[i];

    if (char === '"' && cleanParamString[i - 1] !== '\\') {
      inString = !inString;
    } else if (!inString) {
      if (char === '<' || char === '(') {
        depth++;
      } else if (char === '>' || char === ')') {
        depth--;
      } else if (char === ',' && depth === 0) {
        if (current.trim()) {
          params.push(parseCodegenParameter(current.trim()));
        }
        current = '';
        continue;
      }
    }

    current += char;
  }

  if (current.trim()) {
    params.push(parseCodegenParameter(current.trim()));
  }

  return params;
}

function parseCodegenParameter(param: string): {name: string; type: string; fullType?: string} {
  // Handle common codegen parameter patterns
  param = param.trim();

  // std::function<void(type)> const & callback -> callback parameter
  if (param.includes('std::function')) {
    // Extract the function name from the end
    const parts = param.split(/\s+/);
    let name = parts[parts.length - 1].replace(/[&*]/g, '');
    
    // If name contains 'const', get the previous part
    if (name.includes('const')) {
      name = parts[parts.length - 2] || 'callback';
    }
    
    // Preserve the full function type for accurate generation
    return {
      name: name || 'callback', 
      type: 'function',
      fullType: param
    };
  }

  // Extract parameter name from the end
  const parts = param.split(/\s+/);
  let name = parts[parts.length - 1].replace(/[&*]/g, ''); // Remove references/pointers

  // Handle winrt types and const references
  if (name.includes('const')) {
    name = parts[parts.length - 2] || 'param';
  }

  // Map common codegen types
  let type = 'any';
  if (param.includes('std::string') || param.includes('winrt::hstring')) {
    type = 'string';
  } else if (param.includes('double') || param.includes('float')) {
    type = 'number';
  } else if (param.includes('bool')) {
    type = 'boolean';
  } else if (
    param.includes('int32_t') ||
    param.includes('int64_t') ||
    param.includes('int')
  ) {
    type = 'number';
  } else if (param.includes('JSValue')) {
    type = 'any';
  } else if (param.includes('winrt::')) {
    type = 'string'; // Most winrt types are strings or can be treated as such
  }

  return {name: name || 'param', type, fullType: param};
}

function parseCodegenSignature(signature: string): {name: string; type: string; fullType?: string}[] {
  // Parse Method<void(...)> signature to extract parameters
  const paramMatch = signature.match(/void\s*\(([^)]*)\)/);
  if (!paramMatch) {
    return [];
  }

  return parseCodegenParameters(paramMatch[1]);
}

function mapTSToCppType(tsType: string): string {
  const typeMap: {[key: string]: string} = {
    string: 'std::string',
    number: 'double',
    boolean: 'bool',
    object: 'React::JSValue',
    any: 'React::JSValue',
    'any[]': 'React::JSValueArray',
    void: 'void',
    Double: 'double', // React Native codegen type
    Int32: 'int32_t', // React Native codegen type
    Float: 'float', // React Native codegen type
  };

  // Handle array types
  if (tsType.endsWith('[]')) {
    const baseType = tsType.slice(0, -2);
    const cppBaseType = typeMap[baseType] || 'React::JSValue';
    return `std::vector<${cppBaseType}>`;
  }

  return typeMap[tsType] || 'React::JSValue';
}

function generateDefaultValue(returnType: string): string {
  if (returnType === 'string') {
    return '"example"';
  } else if (returnType === 'number') {
    return '0';
  } else if (returnType === 'boolean') {
    return 'false';
  } else {
    return 'React::JSValue{}';
  }
}

async function generateHeaderStub(moduleName: string, methods: MethodSignature[]): Promise<string> {
  const namespace = `${moduleName}Specs`;
  const codegenNamespace = `${moduleName}Codegen`;

  // Add hello world method first
  const helloWorldMethod = `  REACT_METHOD(helloWorld)
  void helloWorld() noexcept;`;

  const methodDeclarations = methods
    .map(method => {
      const cppParams = method.parameters
        .map(p => {
          if (p.type === 'function') {
            // Use the full type if available, otherwise fallback to simplified version
            if (p.fullType) {
              return p.fullType;
            }
            // Handle callback functions from codegen
            if (p.name.includes('onSuccess') || p.name === 'callback') {
              return `std::function<void()> const & ${p.name}`;
            } else if (p.name.includes('onError')) {
              return `std::function<void(::React::JSValue const &)> const & ${p.name}`;
            } else {
              return `std::function<void()> const & ${p.name}`;
            }
          } else {
            let cppType = mapTSToCppType(p.type);
            // Use winrt::hstring for string parameters to match Windows conventions
            if (p.type === 'string') {
              cppType = 'winrt::hstring const&';
            }
            return `${cppType} ${p.name}`;
          }
        })
        .join(', ');

      // Determine if this is a getter method (no parameters and non-void return type)
      const isGetter =
        method.parameters.length === 0 && method.returnType !== 'void';
      const returnType = isGetter
        ? mapTSToCppType(method.returnType)
        : 'void';
      const constModifier = isGetter ? ' const' : '';

      return `  REACT_METHOD(${method.name})
  ${returnType} ${method.name}(${cppParams}) noexcept${constModifier};`;
    })
    .join('\n\n');

  // Combine hello world with parsed methods
  const allMethodDeclarations = methodDeclarations ? [helloWorldMethod, methodDeclarations].join('\n\n') : helloWorldMethod;

  const defaultMethods = allMethodDeclarations;

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

${defaultMethods}

private:
  React::ReactContext m_context;
};

} // namespace winrt::${namespace}`;
}

async function generateCppStub(moduleName: string, methods: MethodSignature[]): Promise<string> {
  const namespace = `${moduleName}Specs`;

  // Add hello world implementation first
  const helloWorldImplementation = `void ${moduleName}::helloWorld() noexcept {
  // Log a welcome message using Windows console
  OutputDebugStringA("Hello, world! Welcome to the ${moduleName} module!\\n");
}`;

  const methodImplementations = methods
    .map(method => {
      const cppParams = method.parameters
        .map(p => {
          if (p.type === 'function') {
            // Use the full type if available, otherwise fallback to simplified version
            if (p.fullType) {
              return p.fullType;
            }
            // Handle callback functions from codegen
            if (p.name.includes('onSuccess') || p.name === 'callback') {
              return `std::function<void()> const & ${p.name}`;
            } else if (p.name.includes('onError') || p.name.includes('reject')) {
              return `std::function<void(::React::JSValue const &)> const & ${p.name}`;
            } else {
              return `std::function<void()> const & ${p.name}`;
            }
          } else {
            let cppType = mapTSToCppType(p.type);
            // Use winrt::hstring for string parameters to match Windows conventions
            if (p.type === 'string') {
              cppType = 'winrt::hstring const&';
            }
            return `${cppType} ${p.name}`;
          }
        })
        .join(', ');

      // Determine if this is a getter method (no parameters and non-void return type)
      const isGetter =
        method.parameters.length === 0 && method.returnType !== 'void';
      const returnType = isGetter
        ? mapTSToCppType(method.returnType)
        : 'void';
      const constModifier = isGetter ? ' const' : '';

      // Generate implementation based on method type
      const hasCallback = method.parameters.some(
        p =>
          p.type === 'function' &&
          (p.name.includes('onSuccess') || p.name === 'callback'),
      );
      const hasErrorCallback = method.parameters.some(
        p => p.type === 'function' && (p.name.includes('onError') || p.name.includes('reject')),
      );

      let implementation = `  // TODO: Implement ${method.name}`;

      if (isGetter) {
        // Getter method - return default value
        const defaultValue = generateDefaultValue(method.returnType);
        implementation += `\n  return ${defaultValue};`;
      } else if (hasCallback && hasErrorCallback) {
        // Method with success and error callbacks
        implementation += `\n  // Example: callback(); // Call on success\n  // Example: onError(React::JSValue{"Error message"}); // Call on error`;
      } else if (hasCallback) {
        // Method with just callback
        implementation += `\n  // Example: callback(); // Call when complete`;
      }

      return `${returnType} ${moduleName}::${method.name}(${cppParams}) noexcept${constModifier} {
${implementation}
}`;
    })
    .join('\n\n');

  // Combine hello world with parsed method implementations
  const allImplementations = methodImplementations ? [helloWorldImplementation, methodImplementations].join('\n\n') : helloWorldImplementation;

  return `#include "${moduleName}.h"

namespace winrt::${namespace} {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

${allImplementations}

} // namespace winrt::${namespace}
`;
}