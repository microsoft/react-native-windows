/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

//
// Copy generated documentation files to Docusaurus location.
//

import path from 'path';
import {log} from './logger';
import {promises as fs} from 'fs';

export async function copyDocusaurusFiles(files: string[]) {
  const docsPath = process.env.DOCUSAURUS_DOCS;
  if (!docsPath) {
    log(`[Not found] environment var {DOCUSAURUS_DOCS} to copy files`);
    return;
  }

  log(`[Found] environment var {DOCUSAURUS_DOCS} = {${docsPath}}`);
  log(`[Start] copying files to Docusaurus Docs {${docsPath}}`);

  for (const file of files) {
    const target = path.join(docsPath, path.basename(file));
    log(`[Copying] file to {${target}}`);
    await fs.copyFile(file, target);
  }

  log(`[Finished] copying files to Docusaurus Docs: {${docsPath}}`);
}
