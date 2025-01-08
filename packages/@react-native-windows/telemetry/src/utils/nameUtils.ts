/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

export function isValidTelemetryPackageName(name: string): boolean {
  // Accepted characters: alphanumeric, underscore, dot, starts with letter.
  // Size: 1-100 characters.
  if (name.match(/^[a-zA-Z][a-zA-Z0-9_.]{0,99}$/gi)) {
    return true;
  }
  return false;
}

export function cleanTelemetryPackageName(str: string): string {
  return str.replace(/[^a-zA-Z0-9_.]/g, '_').slice(0, 100);
}
