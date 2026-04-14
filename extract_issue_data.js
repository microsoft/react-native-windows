#!/usr/bin/env node

const https = require('https');

const REPO_OWNER = 'microsoft';
const REPO_NAME = 'react-native-windows';
const MILESTONE_NUMBER = 42; // Milestone 0.82
const ISSUE_STATE = 'all';
const PER_PAGE = 100;

function fetchIssues(url) {
  return new Promise((resolve, reject) => {
    const options = {
      headers: {
        'User-Agent': 'GitHub-Issues-Extractor',
        'Accept': 'application/vnd.github+json',
      }
    };
    if (process.env.GH_TOKEN) {
      options.headers['Authorization'] = `token ${process.env.GH_TOKEN}`;
    }

    const request = https.get(url, options, (response) => {
      let data = '';
      response.on('data', (chunk) => { data += chunk; });
      response.on('end', () => {
        try {
          const issues = JSON.parse(data);
          if (Array.isArray(issues)) {
            // Check for next page via Link header
            const linkHeader = response.headers['link'];
            let nextUrl = null;
            if (linkHeader) {
              const match = linkHeader.match(/<([^>]+)>;\s*rel="next"/);
              if (match) nextUrl = match[1];
            }
            resolve({ issues, nextUrl });
          } else {
            reject(new Error('Response is not an array of issues: ' + data.substring(0, 200)));
          }
        } catch (error) {
          reject(new Error(`Failed to parse JSON: ${error.message}`));
        }
      });
    });
    request.on('error', (error) => { reject(new Error(`Request failed: ${error.message}`)); });
    request.setTimeout(15000, () => { request.abort(); reject(new Error('Request timeout')); });
  });
}

async function fetchAllIssues() {
  let url = `https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/issues?state=${ISSUE_STATE}&milestone=${MILESTONE_NUMBER}&per_page=${PER_PAGE}`;
  let allIssues = [];
  while (url) {
    const { issues, nextUrl } = await fetchIssues(url);
    allIssues = allIssues.concat(issues);
    url = nextUrl;
  }
  return allIssues;
}

function extractIssueDataCSV(issues) {
  console.log('Number,Title,State,Assignee,Labels,URL');
  issues.forEach(issue => {
    const number = issue.number || 'N/A';
    const title = (issue.title || 'No title').replace(/"/g, '""');
    const state = issue.state || 'unknown';
    const assigneeLogin = issue.assignee ? issue.assignee.login : 'Unassigned';
    const labels = issue.labels && Array.isArray(issue.labels)
      ? issue.labels.map(label => label.name).join(';')
      : 'No labels';
    const url = `https://github.com/${REPO_OWNER}/${REPO_NAME}/issues/${number}`;
    console.log(`${number},"${title}",${state},${assigneeLogin},"${labels}",${url}`);
  });
}

async function main() {
  try {
    const issueData = await fetchAllIssues();
    extractIssueDataCSV(issueData);
  } catch (error) {
    console.error('Error fetching issues:', error.message);
    process.exit(1);
  }
}

main();
