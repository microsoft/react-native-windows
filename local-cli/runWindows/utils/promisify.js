'use strict';

var child_process = require('child_process');
var exec_ = child_process.exec;
var glob_ = require('glob');

function glob(path, options) {
  return new Promise((resolve, reject) => {
    glob_(path, options, (err, files) => {
      if (err) {
        reject(err);
      } else {
        resolve(files);
      }
    });
  });
}

function exec(command, options) {
  return new Promise((resolve, reject) => {
    exec_(command, options, (error, stdout) => {
      if (error) {
        reject(error);
      } else {
        resolve(stdout);
      }
    });
  });
}

module.exports = {
  exec,
  glob,
};
