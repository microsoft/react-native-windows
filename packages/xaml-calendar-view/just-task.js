/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, series} = require('just-scripts');
const fs = require('fs');
const glob = require('glob');
const path = require('path');

// Use the shared base configuration
require('@rnw-scripts/just-task');

// The TS build process will strip all the types from the NativeComponent spec file. Which means that the codegen babel will not correctly generate the JS view config for the component.
// So here we manually run babel, overwriting the tsc output, so that we ship the generated code.
task('codegenNativeComponents', () => {
  const babel = require('@babel/core');
  const matches = glob.sync('src/**/*NativeComponent.ts');

  matches.forEach(matchedPath => {
    const relativePath = path.relative(
      path.resolve(process.cwd(), 'src'),
      matchedPath,
    );
    const code = fs.readFileSync(matchedPath).toString();
    const filename = path.resolve(process.cwd(), matchedPath);

    const res = babel.transformSync(code, {
      ast: false,
      filename,
      cwd: process.cwd(),
      sourceRoot: process.cwd(),
      root: process.cwd(),
      babelrc: true,
    });

    const relativeOutputPath = relativePath.replace(/\.ts$/, '.js');

    fs.writeFileSync(
      path.resolve(process.cwd(), 'lib-commonjs', relativeOutputPath),
      res?.code,
    );
  });
});

task(
  'buildWithCodegetNativeComponents',
  series('build', 'codegenNativeComponents'),
);
