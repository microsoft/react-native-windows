/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Forked from https://github.com/asklar/unbroken
 * Original Copyright (c) 2020-2022 Alexander Sklar
 *
 * @format
 */

import * as path from 'path';

import fs from '@react-native-windows/fs';

function yellowBold(text: string): string {
  return `\x1b[1m\x1b[33m${text}\x1b[0m`;
}

function redBold(text: string): string {
  return `\x1b[1m\x1b[31m${text}\x1b[0m`;
}

async function msleep(n: number) {
  return new Promise(resolve => setTimeout(resolve, n));
}

const DefaultUserAgent = 'Chrome/89.0.4346.0';

/**
 * Set of HTTP status codes that indicate transient server errors worth
 * retrying, since the server may recover on a subsequent attempt.
 */
const TRANSIENT_HTTP_ERRORS = new Set([502, 503, 504]);

export interface Options {
  quiet: boolean;
  superquiet: boolean;
  dir: string;
  exclusions: string;
  'local-only': boolean;
  'allow-local-line-sections': boolean;
  'parse-ids': boolean;
  'user-agent': string;
}

export class Checker {
  constructor(public readonly options: Options) {
    this.errors = [];
    this.options = options;
    this.ids = {};
    this.urlCache = {};
    if (this.options.superquiet) {
      this.options.quiet = true;
    }

    // Normalize options.dir for later
    this.options.dir = path.resolve(this.options.dir || '.');

    const exclusionsFileName =
      this.options.exclusions ||
      path.join(this.options.dir, '.unbroken_exclusions');
    try {
      const contents =
        fs
          .readFileSync(exclusionsFileName)
          .toString()
          .split(/\r?\n/)
          .filter(x => x.trim() !== '');
      this.suppressions = contents
        .filter(x => !x.startsWith('!'))
        .map(x => Checker.normalizeSlashes(x));
      this.exclusions = contents
        .filter(x => x.startsWith('!'))
        .map(x => Checker.normalizeSlashes(path.normalize(x.slice(1))));
    } catch {
      this.suppressions = [];
      this.exclusions = [];
    }
  }

  errors: string[];
  ids: Record<string, string>;
  suppressions: string[];
  exclusions: string[];
  urlCache: Record<string, number>;


  private async recurseFindMarkdownFiles(
    dirPath: string,
    callback: {(path: string): Promise<void>},
  ) {
    const files = await fs.readdir(dirPath);
    await asyncForEach(
      files,
      async (file: string) => {
        const filePath = path.join(dirPath, file);
        const relFilePath = Checker.normalizeSlashes(
          this.getRelativeFilePath(filePath),
        );
        const shouldSkip = this.exclusions.some(pattern =>
          path.matchesGlob(relFilePath, pattern),
        );
        if (shouldSkip) {
          this.log(`Skipping ${Checker.normalizeSlashes(relFilePath)}.`);
        } else {
          const stat = fs.statSync(filePath);
          if (stat.isDirectory()) {
            await this.recurseFindMarkdownFiles(filePath, callback);
          } else if (filePath.toLowerCase().endsWith('.md')) {
            await callback(filePath);
          }
        }
      },
      true,
    );
  }

  private async getAndStoreId(idPath: string) {
    const lines = fs
      .readFileSync(idPath)
      .toString()
      .split(/[\r\n]+/g);
    if (
      lines.length > 2 &&
      lines[0].trim() === '---' &&
      lines[1].toLowerCase().startsWith('id:')
    ) {
      const id = lines[1].slice('id:'.length).trim();
      this.ids[id] = idPath;
    }
  }

  async run(dirPath?: string) {
    if (!dirPath) {
      dirPath = this.options.dir;
    }

    if (this.options['parse-ids']) {
      await this.recurseFindMarkdownFiles(dirPath, x =>
        this.getAndStoreId(x),
      );
    }
    await this.recurseFindMarkdownFiles(dirPath, x =>
      this.verifyMarkDownFile(x),
    );
    let n = 0;
    this.errors.forEach(err => {
      if (this.suppressions.includes(err)) {
        this.log(yellowBold('WARNING:'), err);
      } else {
        this.logError(err);
        n++;
      }
    });
    return n;
  }

  private log(...args: string[]) {
    if (!this.options.quiet) {
      console.log(args.join(' '));
    }
  }

  private logError(error: string) {
    if (!this.options.superquiet) {
      console.log(redBold('ERROR:'), error);
    }
  }

  private getRelativeFilePath(filePath: string) {
    return filePath.substr(this.options.dir.length + 1);
  }

  private static normalizeSlashes(str: string) {
    return str.replace(/\\/g, '/');
  }

  private validateFile(name: string, value: string, filePath: string) {
    const dir = path.dirname(filePath);
    const pathToCheck = path.join(dir, value);
    if (!fs.existsSync(pathToCheck)) {
      const pathToCheckReplaced = path.join(dir, value.replace(/_/g, '-'));
      if (!fs.existsSync(pathToCheckReplaced)) {
        if (!this.ids[value]) {
          this.errors.push(
            `File not found ${Checker.normalizeSlashes(
              path.normalize(value),
            )} while parsing ${Checker.normalizeSlashes(
              this.getRelativeFilePath(filePath),
            )}`,
          );
          return undefined;
        } else {
          return this.ids[value];
        }
      } else {
        return pathToCheckReplaced;
      }
    }
    return path.normalize(pathToCheck);
  }

  private getAnchors(content: string): string[] {
    const anchorRegex = /(^#+|\n+#+)\s*(?<anchorTitle>[^\s].*)/g;
    const anchors: string[] = [];
    const results = content.matchAll(anchorRegex);
    for (const result of results) {
      const title = result.groups!.anchorTitle;
      const transformed = title.replace(/[^\w\d\s-]+/g, '').replace(/ /g, '-');
      let newItem = transformed;
      const found = anchors.includes(transformed);
      if (found) {
        const regex = new RegExp(`${transformed}-\\d+`);
        const lastIndex = anchors.find(x => regex.test(x));
        const newIndex = lastIndex ? lastIndex.length + 1 : 1;
        newItem = transformed + '-' + newIndex;
      }
      anchors.push(newItem);
    }
    return anchors;
  }

  private validateSection(
    name: string,
    value: string,
    contents: string | null,
    filePath: string,
  ) {
    const hash = value.indexOf('#');
    const sectionAnchor = value.substring(hash + 1);
    const page = value.substring(0, hash);
    let extra = '';
    if (page !== '') {
      extra = ` while parsing ${Checker.normalizeSlashes(
        this.getRelativeFilePath(filePath),
      )}`;
      const realFilePath = this.validateFile(name, page, filePath);
      if (realFilePath) {
        contents = fs.readFileSync(realFilePath).toString();
      } else {
        // file doesn't exist - already logged that the file is missing
        return;
      }
    }
    if (!contents) {
      this.errors.push("Couldn't read contents");
      return;
    }

    const anchors = this.getAnchors(contents.toLowerCase());
    if (!anchors.includes(sectionAnchor.toLowerCase())) {
      if (
        !anchors.includes(sectionAnchor.replace(/\./g, '').toLowerCase())
      ) {
        if (
          !(
            this.options['allow-local-line-sections'] &&
            !Checker.isWebLink(value) &&
            sectionAnchor.length > 1 &&
            sectionAnchor.startsWith('L') &&
            !isNaN(parseInt(sectionAnchor.substring(1), 10))
          )
        ) {
          this.errors.push(
            `Section ${sectionAnchor} not found in ${Checker.normalizeSlashes(
              this.getRelativeFilePath(filePath),
            )}${extra}. Available anchors: ${JSON.stringify(anchors)}`,
          );
        }
      }
    }
  }

  private async validateURL(name: string, value: string, filePath: string) {
    const maxIterations = 5;
    const ignoring429 =
      this.suppressions.findIndex(x => x === 'HTTP/429') !== -1;
    const relativeFilePath = Checker.normalizeSlashes(
      this.getRelativeFilePath(filePath),
    );

    let result: number | undefined =
      value in this.urlCache ? this.urlCache[value] : undefined;

    // spin while pending validates finish
    while (result === -1) {
      await msleep(100);
      result = this.urlCache[value];
    }

    if (result === 200) {
      // Previous success, bail early
      return true;
    } else if (result === undefined) {
      // No previous result, actually check URL
      this.urlCache[value] = -1; // block other validates until this is finished
      this.log(`Verifying ${value} for ${relativeFilePath}`);
      for (let i = 0; i < maxIterations; i++) {
        if (i > 0) {
          this.log(`Retrying ${value} for ${relativeFilePath}, attempt #${i}`);
        }

        try {
          const userAgent = this.options['user-agent'] || DefaultUserAgent;
          const r = await fetch(value, {
            headers: {
              'User-Agent': userAgent,
              'Accept-Encoding': 'gzip, deflate, br',
            },
            redirect: 'follow',
          });
          const status = r.status;

          if (status === 200) {
            result = status;
            break;
          }

          let sleepSeconds = 0;
          if (TRANSIENT_HTTP_ERRORS.has(status)) {
            // Transient server error - retry with backoff
            sleepSeconds = (i + 1) * 2;
            this.log(
              `HTTP/${status} for ${value}, transient error, sleeping for ${sleepSeconds}s before retry`,
            );
          } else if (status === 429 && !ignoring429) {
            // Being throttled with HTTP/429
            const retryAfterSeconds = r.headers.has('retry-after')
              ? parseInt(r.headers.get('retry-after')!, 10)
              : 0;

            sleepSeconds = ((i + 1) / maxIterations) * retryAfterSeconds;
            this.log(
              `HTTP/429 for ${value}, requested retry after ${retryAfterSeconds}s, sleeping for ${sleepSeconds}s`,
            );
          } else {
            // Non-transient error (or a 429 we're ignoring) - bail
            result = status;
            break;
          }
          await msleep(100 + sleepSeconds * 1000);
        } catch {
          // Network error (DNS failure, connection refused, etc.) - retry
          await msleep(100);
        }
      } // for
    } // else if

    // Normalize result to -1 sentinel if we hit the max retries
    result = result === undefined ? -1 : result;

    // Save result
    this.urlCache[value] = result;

    if (result === 200 || (result === 429 && ignoring429)) {
      return true;
    } else if (result === -1) {
      this.errors.push(
        `URL not found ${value} while parsing ${relativeFilePath} after ${maxIterations} retries`,
      );
      return false;
    } else {
      this.errors.push(
        `URL not found ${value} while parsing ${relativeFilePath} (HTTP ${result})`,
      );
      return false;
    }
  }

  private async validateLink(
    name: string,
    value: string,
    contents: string | null,
    filePath: string,
  ) {
    if (value.startsWith('mailto:')) {
      // Not implemented
    } else if (Checker.isWebLink(value)) {
      if (!this.options['local-only']) {
        await this.validateURL(name, value, filePath);
      }
    } else if (value.includes('#')) {
      this.validateSection(name, value, contents, filePath);
    } else {
      this.validateFile(name, value, filePath);
    }
  }

  private static isWebLink(url: string) {
    return url.startsWith("https://") || url.startsWith('https://');
  }

  async verifyMarkDownFile(filePath: string) {
    this.log(
      `Verifying ${Checker.normalizeSlashes(
        this.getRelativeFilePath(filePath),
      )}`,
    );
    const contents = fs.readFileSync(filePath).toString();

    const balancedParensTwoLevels = '([^(\\s]*\\([^)\\s]*\\))?[^()\\s]*';
    const baseLink = `\\[(?=([^\`]*\`[^\`]*\`)*[^\`]*$)(?<nameBASE>[^\\]]+)\\]\\s*\\((?<valueBASE>${balancedParensTwoLevels}?)("(?<titleBASE>[^"]*)")?\\)`;
    const imgLink = `(?<imageLinkTag>\\[\\!)${baseLink.replace(
      /BASE/g,
      'img',
    )}(?<endImageLinkTag>\\]\\((?<linkTarget>[^)]+)\\))`;
    const nonImgLink = baseLink.replace(/BASE/g, '');
    const mdLinkRegex = new RegExp(`(${imgLink})|(${nonImgLink})`, 'g');

    const results = contents.matchAll(mdLinkRegex);

    let imgSrc: string | undefined;
    for (const result of results) {
      const groups = result.groups!;
      let name = groups.name;
      let value = groups.value;

      if (groups.imageLinkTag) {
        name = groups.nameimg;
        value = groups.linkTarget;
        imgSrc = groups.valueimg;
      }

      await this.validateLink(name, value, contents, filePath);
      if (imgSrc) {
        await this.validateLink(name, imgSrc, null, filePath);
      }
    }
  }
}

async function asyncForEach(
  array: string[],
  callback: {(file: string, index: number, arr: string[]): Promise<void>},
  parallel: boolean,
) {
  const calls = [];
  for (let index = 0; index < array.length; index++) {
    const call = callback(array[index], index, array);
    if (parallel) {
      calls.push(call);
    } else {
      await call;
    }
  }
  if (parallel) {
    await Promise.all(calls);
  }
}
