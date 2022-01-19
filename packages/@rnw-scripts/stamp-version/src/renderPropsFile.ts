/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import mustache from 'mustache';
import path from 'path';
import semver from 'semver';

/**
 * Render PackageVersion.g.props
 */
export default async function renderPropsFile(
  version: string,
): Promise<string> {
  const templatePath = path.join(
    __dirname,
    '..',
    'template',
    'PackageVersion.g.props.hbs',
  );
  const templateContent = await fs.readFile(templatePath);

  return mustache.render(templateContent.toString(), {
    version,
    major: semver.major(version),
    minor: semver.minor(version),
    patch: semver.patch(version),
    canary: semver.prerelease(version)?.[0] === 'canary',
  });
}
