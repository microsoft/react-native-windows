'use strict';

const VERSION_EXPRESSION = /^\d{1,8}(\.\d{1,8}){0,3}$/;

function zeroIfUndefined(val) {
  return typeof val === 'undefined' ? 0 : val;
}

class Version {
  constructor(major, minor, build, qfe) {
    this.major = major;
    this.minor = zeroIfUndefined(minor);
    this.build = zeroIfUndefined(build);
    this.qfe = zeroIfUndefined(qfe);
  }

  eq(other) {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    return this.major === other.major &&
      this.minor === other.minor &&
      this.build === other.build &&
      this.qfe === other.qfe;
  }

  gt(other) {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    if (this.major > other.major) { return true; }
    if (this.major < other.major) { return false; }
    if (this.minor > other.minor) { return true; }
    if (this.minor < other.minor) { return false; }
    if (this.build > other.build) { return true; }
    if (this.build < other.build) { return false; }
    if (this.qfe > other.qfe) { return true; }
    if (this.que < other.qfe) { return false; }

    return false;
  }

  gte(other) {
    if (other.constructor !== Version) {
      throw new TypeError('other must be a Version instance');
    }

    if (this.major > other.major) { return true; }
    if (this.major < other.major) { return false; }
    if (this.minor > other.minor) { return true; }
    if (this.minor < other.minor) { return false; }
    if (this.build > other.build) { return true; }
    if (this.build < other.build) { return false; }
    if (this.qfe > other.qfe) { return true; }
    if (this.que < other.qfe) { return false; }

    return true;
  }

  toString() {
    return `${this.major}.${this.minor}.${this.build}.${this.qfe}`;
  }

  static compare(x, y) {
    if (x.constructor !== Version || y.constructor !== Version) {
      throw new TypeError('Only can compare Version objects');
    }

    return x.gt(y) ? 1 : x.eq(y) ? 0 : -1;
  }

  static fromString(str) {
    const version = Version.tryParse(str);
    if (!version) {
      throw new TypeError('Cannot construct a version from "' + str + '".');
    }
    return version;
  }

  static tryParse(str) {
    if (VERSION_EXPRESSION.test(str)) {
      const versionArray = str.split('.').map(x => parseInt(x, 10));
      return new Version(versionArray[0], versionArray[1], versionArray[2], versionArray[3]);
    }
    return null;
  }
}

module.exports = Version;
