/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * Post perf test results as a GitHub PR comment.
 *
 * Uses the GitHub API to find/update an existing perf comment on the PR,
 * or create a new one. Designed to be called from GitHub Actions.
 *
 * Usage:
 *   node vnext/Scripts/perf/post-pr-comment.js [options]
 *
 * Options:
 *   --report <path>     Path to markdown report (default: .perf-results/report.md)
 *   --summary <path>    Path to JSON summary (default: .perf-results/report.json)
 *   --pr <number>       PR number (default: from GITHUB_EVENT)
 *   --dry-run           Print the comment but don't post it
 *
 * Environment:
 *   GITHUB_TOKEN        Required for posting comments
 *   GITHUB_REPOSITORY   e.g. microsoft/react-native-windows
 *   GITHUB_EVENT_PATH   Path to event.json (auto-set in GH Actions)
 *
 * @format
 */

'use strict';

const fs = require('fs');
const https = require('https');
const path = require('path');

const COMMENT_MARKER = '<!-- rnw-perf-results -->';

function parseArgs() {
  const args = process.argv.slice(2);
  const opts = {
    report: '.perf-results/report.md',
    summary: '.perf-results/report.json',
    pr: null,
    dryRun: false,
  };

  for (let i = 0; i < args.length; i++) {
    switch (args[i]) {
      case '--report':
        opts.report = args[++i];
        break;
      case '--summary':
        opts.summary = args[++i];
        break;
      case '--pr':
        opts.pr = parseInt(args[++i], 10);
        break;
      case '--dry-run':
        opts.dryRun = true;
        break;
    }
  }

  // Attempt to detect PR number from GH Actions event
  if (!opts.pr && process.env.GITHUB_EVENT_PATH) {
    try {
      const event = JSON.parse(
        fs.readFileSync(process.env.GITHUB_EVENT_PATH, 'utf-8'),
      );
      if (event.pull_request) {
        opts.pr = event.pull_request.number;
      } else if (event.number) {
        opts.pr = event.number;
      }
    } catch {
      // ignore
    }
  }

  return opts;
}

function githubRequest(method, apiPath, body) {
  const token = process.env.GITHUB_TOKEN;
  if (!token) {
    throw new Error('GITHUB_TOKEN environment variable is required');
  }

  const repo =
    process.env.GITHUB_REPOSITORY || 'microsoft/react-native-windows';

  return new Promise((resolve, reject) => {
    const options = {
      hostname: 'api.github.com',
      port: 443,
      path: `/repos/${repo}${apiPath}`,
      method,
      headers: {
        Authorization: `token ${token}`,
        Accept: 'application/vnd.github.v3+json',
        'User-Agent': 'rnw-perf-tests',
        'Content-Type': 'application/json',
      },
    };

    const req = https.request(options, res => {
      let data = '';
      res.on('data', chunk => (data += chunk));
      res.on('end', () => {
        if (res.statusCode >= 200 && res.statusCode < 300) {
          resolve(data ? JSON.parse(data) : null);
        } else {
          reject(
            new Error(
              `GitHub API ${method} ${apiPath} returned ${res.statusCode}: ${data}`,
            ),
          );
        }
      });
    });

    req.on('error', reject);

    if (body) {
      req.write(JSON.stringify(body));
    }
    req.end();
  });
}

async function findExistingComment(prNumber) {
  let page = 1;
  while (true) {
    const comments = await githubRequest(
      'GET',
      `/issues/${prNumber}/comments?per_page=100&page=${page}`,
    );
    if (!comments || comments.length === 0) break;

    const existing = comments.find(
      c => c.body && c.body.includes(COMMENT_MARKER),
    );
    if (existing) return existing;

    if (comments.length < 100) break;
    page++;
  }
  return null;
}

async function main() {
  const opts = parseArgs();

  // Load markdown report
  if (!fs.existsSync(opts.report)) {
    console.error(`❌ Report not found: ${opts.report}`);
    console.error('Run compare-results.js first.');
    process.exit(1);
  }

  const markdown = fs.readFileSync(opts.report, 'utf-8');

  // Load summary for status message
  let summary = {hasRegressions: false};
  if (fs.existsSync(opts.summary)) {
    summary = JSON.parse(fs.readFileSync(opts.summary, 'utf-8'));
  }

  // Build comment body with hidden marker for idempotent updates
  const commentBody = `${COMMENT_MARKER}\n${markdown}`;

  if (opts.dryRun) {
    console.log('=== DRY RUN — would post the following comment ===\n');
    console.log(commentBody);
    console.log('\n=== End of comment ===');
    process.exit(summary.hasRegressions ? 1 : 0);
  }

  if (!opts.pr) {
    console.error('❌ Could not determine PR number.');
    console.error('Use --pr <number> or ensure GITHUB_EVENT_PATH is set.');
    process.exit(1);
  }

  console.log(`📤 Posting perf results to PR #${opts.pr}...`);

  try {
    // Try to update existing comment
    const existing = await findExistingComment(opts.pr);
    if (existing) {
      await githubRequest('PATCH', `/issues/comments/${existing.id}`, {
        body: commentBody,
      });
      console.log(`✅ Updated existing comment (id: ${existing.id})`);
    } else {
      await githubRequest('POST', `/issues/${opts.pr}/comments`, {
        body: commentBody,
      });
      console.log('✅ Created new comment');
    }
  } catch (err) {
    console.error(`❌ Failed to post comment: ${err.message}`);
    // Don't fail the build if commenting fails — the results are in artifacts
    console.log('Results are still available as workflow artifacts.');
  }

  process.exit(summary.hasRegressions ? 1 : 0);
}

main().catch(err => {
  console.error(err);
  process.exit(1);
});
