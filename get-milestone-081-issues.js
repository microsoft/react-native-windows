#!/usr/bin/env node
/**
 * Script to fetch and display open issues for milestone 0.81
 * 
 * This script uses the GitHub CLI or API to fetch all open issues in the 0.81 milestone
 * for the microsoft/react-native-windows repository.
 * 
 * Prerequisites:
 *   - GitHub CLI (gh) installed: https://cli.github.com/
 *   OR
 *   - Access to the GitHub API (may be blocked in some environments)
 * 
 * Usage: node get-milestone-081-issues.js [format]
 *   format: 'detailed' (default) | 'summary' | 'csv' | 'json'
 * 
 * Examples:
 *   node get-milestone-081-issues.js
 *   node get-milestone-081-issues.js summary
 *   node get-milestone-081-issues.js csv > issues.csv
 *   node get-milestone-081-issues.js json > issues.json
 */

const https = require('https');
const { execSync } = require('child_process');

const OWNER = 'microsoft';
const REPO = 'react-native-windows';
const MILESTONE = '0.81';
const FORMAT = process.argv[2] || 'detailed';

function makeRequest(path) {
  return new Promise((resolve, reject) => {
    const options = {
      hostname: 'api.github.com',
      path: path,
      method: 'GET',
      headers: {
        'User-Agent': 'RNW-Milestone-Issues-Script',
        'Accept': 'application/vnd.github.v3+json'
      }
    };

    https.get(options, (res) => {
      let data = '';

      res.on('data', (chunk) => {
        data += chunk;
      });

      res.on('end', () => {
        try {
          resolve(JSON.parse(data));
        } catch (e) {
          reject(e);
        }
      });
    }).on('error', (err) => {
      reject(err);
    });
  });
}

function formatDetailed(items, total) {
  console.log(`\nFetching open issues for milestone: ${MILESTONE}`);
  console.log('=' .repeat(80));
  console.log(`\nTotal Open Issues: ${total}\n`);
  console.log('=' .repeat(80));

  console.log('\n## All Open Issues:\n');
  items.forEach((issue, index) => {
    console.log(`${index + 1}. #${issue.number} - ${issue.title}`);
    console.log(`   URL: ${issue.html_url}`);
    console.log(`   State: ${issue.state}`);
    if (issue.assignee) {
      console.log(`   Assignee: @${issue.assignee.login}`);
    } else if (issue.assignees && issue.assignees.length > 0) {
      console.log(`   Assignees: ${issue.assignees.map(a => '@' + a.login).join(', ')}`);
    }
    if (issue.labels && issue.labels.length > 0) {
      console.log(`   Labels: ${issue.labels.map(l => l.name).join(', ')}`);
    }
    console.log(`   Created: ${new Date(issue.created_at).toLocaleDateString()}`);
    console.log(`   Updated: ${new Date(issue.updated_at).toLocaleDateString()}`);
    console.log('');
  });

  // Display summary by label
  const issuesByLabel = {};
  items.forEach(issue => {
    if (issue.labels && issue.labels.length > 0) {
      issue.labels.forEach(label => {
        if (!issuesByLabel[label.name]) {
          issuesByLabel[label.name] = new Set();
        }
        issuesByLabel[label.name].add(issue.number);
      });
    }
  });

  console.log('\n' + '='.repeat(80));
  console.log('\n## Summary by Label:\n');
  const sortedLabels = Object.keys(issuesByLabel).sort((a, b) => 
    issuesByLabel[b].size - issuesByLabel[a].size
  );

  sortedLabels.forEach(label => {
    console.log(`${label}: ${issuesByLabel[label].size} issue(s)`);
  });

  console.log('\n' + '='.repeat(80));
  console.log(`\nTotal: ${total} open issue(s) in milestone "${MILESTONE}"\n`);
}

function formatSummary(items, total) {
  console.log(`Milestone ${MILESTONE}: ${total} open issues\n`);
  items.forEach((issue, index) => {
    const assignee = issue.assignee ? `(@${issue.assignee.login})` : '(unassigned)';
    console.log(`${index + 1}. #${issue.number} ${assignee} - ${issue.title}`);
  });
}

function formatCsv(items) {
  console.log('Number,Title,State,Assignee,Labels,Created,Updated,URL');
  items.forEach(issue => {
    const assignee = issue.assignee ? issue.assignee.login : '';
    const labels = issue.labels ? issue.labels.map(l => l.name).join(';') : '';
    const created = new Date(issue.created_at).toISOString().split('T')[0];
    const updated = new Date(issue.updated_at).toISOString().split('T')[0];
    console.log(`${issue.number},"${issue.title}",${issue.state},${assignee},"${labels}",${created},${updated},${issue.html_url}`);
  });
}

function formatJson(items, total) {
  const output = {
    milestone: MILESTONE,
    total_count: total,
    issues: items.map(issue => ({
      number: issue.number,
      title: issue.title,
      state: issue.state,
      url: issue.html_url,
      assignee: issue.assignee ? issue.assignee.login : null,
      assignees: issue.assignees ? issue.assignees.map(a => a.login) : [],
      labels: issue.labels ? issue.labels.map(l => l.name) : [],
      created_at: issue.created_at,
      updated_at: issue.updated_at
    }))
  };
  console.log(JSON.stringify(output, null, 2));
}

function tryGitHubCLI() {
  try {
    // Try using GitHub CLI
    const output = execSync(
      `gh issue list --repo ${OWNER}/${REPO} --milestone "${MILESTONE}" --state open --limit 1000 --json number,title,state,assignees,labels,createdAt,updatedAt,url`,
      { encoding: 'utf-8', stdio: ['pipe', 'pipe', 'pipe'] }
    );
    const issues = JSON.parse(output);
    return {
      total_count: issues.length,
      items: issues.map(issue => ({
        number: issue.number,
        title: issue.title,
        state: issue.state.toLowerCase(),
        html_url: issue.url,
        assignee: issue.assignees && issue.assignees.length > 0 ? { login: issue.assignees[0].login } : null,
        assignees: issue.assignees || [],
        labels: issue.labels || [],
        created_at: issue.createdAt,
        updated_at: issue.updatedAt
      }))
    };
  } catch (error) {
    return null;
  }
}

async function getMilestoneIssues() {
  try {
    let result = tryGitHubCLI();
    
    if (!result) {
      console.error('GitHub CLI not available. Trying GitHub API...');
      // Try GitHub API
      const searchPath = `/search/issues?q=repo:${OWNER}/${REPO}+milestone:${MILESTONE}+is:open&per_page=100`;
      try {
        result = await makeRequest(searchPath);
      } catch (apiError) {
        console.error('\nError: Unable to fetch issues from GitHub.');
        console.error('Please try one of the following alternatives:\n');
        console.error('1. Install GitHub CLI: https://cli.github.com/');
        console.error('   Then run: gh issue list --repo microsoft/react-native-windows --milestone "0.81" --state open\n');
        console.error('2. Visit the web interface:');
        console.error('   https://github.com/microsoft/react-native-windows/milestone/41\n');
        console.error('3. Use the GitHub API with authentication:');
        console.error('   curl -H "Authorization: token YOUR_TOKEN" \\');
        console.error('     "https://api.github.com/search/issues?q=repo:microsoft/react-native-windows+milestone:0.81+is:open"\n');
        process.exit(1);
      }
    }

    if (result.total_count === 0) {
      console.log(`\nNo open issues found for milestone "${MILESTONE}"\n`);
      return;
    }

    // Format output based on requested format
    switch (FORMAT.toLowerCase()) {
      case 'summary':
        formatSummary(result.items, result.total_count);
        break;
      case 'csv':
        formatCsv(result.items);
        break;
      case 'json':
        formatJson(result.items, result.total_count);
        break;
      case 'detailed':
      default:
        formatDetailed(result.items, result.total_count);
        break;
    }

  } catch (error) {
    console.error('Error fetching issues:', error.message);
    process.exit(1);
  }
}

// Run the script
getMilestoneIssues();
