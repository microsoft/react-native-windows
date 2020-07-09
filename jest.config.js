/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

console.error(
  "\x1b[31mThe react-native-windows-repo doesn't support running jest directly from its root. Please run 'yarn test' instead.\x1b[0m",
);

// Jest should support running across multiple-projects, but doesn't seem to
// fully respect their configs when doing so. Don't run any tests if someone
// tries to run Jest from the repo root (e.g. using the VS code with the Jest
// extension opened to the repo root).
module.exports = {
  roots: [],
};
