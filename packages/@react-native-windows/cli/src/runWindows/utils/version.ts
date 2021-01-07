/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

const VERSION_EXPRESSION = /^\d{1,8}(\.\d{1,8}){0,3}$/;

export default class Version {
  constructor(
    private readonly major: number,
    private readonly minor: number = 0,
    private readonly build: number = 0,
    private readonly qfe: number = 0,
  ) {}

  eq(other: Version): boolean {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    return (
      this.major === other.major &&
      this.minor === other.minor &&
      this.build === other.build &&
      this.qfe === other.qfe
    );
  }

  gt(other: Version): boolean {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    if (this.major > other.major) {
      return true;
    }
    if (this.major < other.major) {
      return false;
    }
    if (this.minor > other.minor) {
      return true;
    }
    if (this.minor < other.minor) {
      return false;
    }
    if (this.build > other.build) {
      return true;
    }
    if (this.build < other.build) {
      return false;
    }
    if (this.qfe > other.qfe) {
      return true;
    }
    if (this.qfe < other.qfe) {
      return false;
    }

    return false;
  }

  gte(other: Version): boolean {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    if (this.major > other.major) {
      return true;
    }
    if (this.major < other.major) {
      return false;
    }
    if (this.minor > other.minor) {
      return true;
    }
    if (this.minor < other.minor) {
      return false;
    }
    if (this.build > other.build) {
      return true;
    }
    if (this.build < other.build) {
      return false;
    }
    if (this.qfe > other.qfe) {
      return true;
    }
    if (this.qfe < other.qfe) {
      return false;
    }

    return true;
  }

  toString(): string {
    return `${this.major}.${this.minor}.${this.build}.${this.qfe}`;
  }

  static compare(x: Version, y: Version): number {
    if (x.constructor !== Version || y.constructor !== Version) {
      throw new TypeError('Only can compare Version objects');
    }

    return x.gt(y) ? 1 : x.eq(y) ? 0 : -1;
  }

  static fromString(str: string): Version {
    const version = Version.tryParse(str);
    if (!version) {
      throw new TypeError('Cannot construct a version from "' + str + '".');
    }
    return version;
  }

  static tryParse(str: string): Version | null {
    if (VERSION_EXPRESSION.test(str)) {
      const versionArray = str.split('.').map(x => parseInt(x, 10));
      return new Version(
        versionArray[0],
        versionArray[1],
        versionArray[2],
        versionArray[3],
      );
    }
    return null;
  }
}
