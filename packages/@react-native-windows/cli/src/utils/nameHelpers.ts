/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import _ from 'lodash';

function pascalCase(str: string): string {
  const camelCase = _.camelCase(str);
  return camelCase[0].toUpperCase() + camelCase.substr(1);
}

export function isValidProjectName(name: string): boolean {
  if (name.match(/^[a-z][a-z0-9]*$/gi)) {
    return true;
  }
  return false;
}

export function cleanName(str: string): string {
  str = str.replace('@', ''); // Remove '@' from package scope names
  str = str.slice(str.lastIndexOf('/') + 1); // Remove package scope
  str = pascalCase(str); // Convert to PascalCase
  return str;
}

export function isValidProjectNamespace(namespace: string): boolean {
  if (
    namespace
      .split(/[.]+/)
      .map(isValidProjectName)
      .every(x => x)
  ) {
    // Validate that every part of the namespace is a valid project name
    return true;
  }
  return false;
}

export function cleanNamespace(str: string): string {
  return str.split(/[.:]+/).map(cleanName).join('.');
}

export function isValidModuleName(name: string): boolean {
  // Module names should be valid C++ identifiers
  // They can contain letters, numbers, and underscores, but cannot start with a number
  if (name.match(/^[a-zA-Z_][a-zA-Z0-9_]*$/)) {
    return true;
  }
  return false;
}

export function cleanModuleName(str: string): string {
  // Clean module name to be a valid C++ identifier
  // Remove any invalid characters and ensure it starts with a letter
  str = str.replace(/[^a-zA-Z0-9_]/g, ''); // Remove invalid chars
  if (str.length > 0 && str[0].match(/[0-9]/)) {
    str = 'Module' + str; // Prefix with 'Module' if it starts with a number
  }
  if (str.length === 0) {
    str = 'Module'; // Default fallback
  }
  return str;
}
