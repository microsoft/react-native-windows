import fetch from 'node-fetch';
import fs from 'fs';
import process from 'process';

const GITHUB_TOKEN = process.env.GITHUB_TOKEN;
const REPO = 'microsoft/react-native-windows';
const RELEASE_TAG = process.env.RELEASE_TAG || 'Unreleased';
const START_DATE = process.env.START_DATE;
const END_DATE = process.env.END_DATE;

if (!GITHUB_TOKEN) {
  console.error('GITHUB_TOKEN is not set. Please set it before running.');
  process.exit(1);
}
if (!START_DATE || !END_DATE) {
  console.error('START_DATE and END_DATE are required.');
  process.exit(1);
}

console.log(`Generating release notes for ${REPO} from ${START_DATE} to ${END_DATE}...`);

const HEADERS = {
  Authorization: `token ${GITHUB_TOKEN}`,
  Accept: 'application/vnd.github+json',
};

function parseDate(dateStr) {
  return dateStr ? new Date(dateStr) : null;
}

const START = parseDate(START_DATE);
const END = parseDate(END_DATE);

function isBotCommit(commit) {
  const author = commit.author;
  const commitAuthorName = (commit.commit.author.name || '').toLowerCase();
  const authorLogin = (author?.login || '').toLowerCase();
  const botIndicators = ['bot', 'dependabot', 'actions-user'];
  const msg = commit.commit.message.toLowerCase();

  if (
    botIndicators.some(
      (bot) => authorLogin.includes(bot) || commitAuthorName.includes(bot)
    )
  )
    return true;
  if (['bump', 'applying package updates', 'no_ci', 'no ci'].some((k) => msg.includes(k)))
    return true;
  return false;
}

function formatDate(date) {
  return date ? new Date(date).toLocaleDateString('en-US') : 'N/A';
}

async function fetchCommits() {
  const commits = [];
  let page = 1;
  const perPage = 100;

  while (true) {
    const url = new URL(`https://api.github.com/repos/${REPO}/commits`);
    url.searchParams.set('per_page', perPage);
    url.searchParams.set('page', page);
    if (START_DATE) url.searchParams.set('since', START_DATE + 'T00:00:00Z');
    if (END_DATE) url.searchParams.set('until', END_DATE + 'T23:59:59Z');

    console.log(`Fetching commits from: ${url.toString()}`);

    const res = await fetch(url, { headers: HEADERS });

    if (!res.ok) {
      console.error(`GitHub API request failed: ${res.status} ${res.statusText}`);
      const errText = await res.text();
      console.error('Response body:', errText);
      break;
    }

    const data = await res.json();

    if (!Array.isArray(data)) {
      console.error('Unexpected response format:', data);
      break;
    }

    console.log(`Fetched page ${page} with ${data.length} commits.`);

    if (data.length === 0) break;

    commits.push(...data);
    page++;
  }

  console.log(`Total commits fetched: ${commits.length}`);
  return commits;
}

function filterCommitsByDate(commits) {
  return commits.filter((c) => {
    if (isBotCommit(c)) return false;
    const commitDate = new Date(c.commit.author.date);
    if (START && commitDate < START) return false;
    if (END && commitDate > END) return false;
    return true;
  });
}

function categorizeCommits(commits) {
// TODO: Update logic for commits categorisation, refer 'All Commits' section for all the commits.
  const categories = {
    'All Commits': [],
    'Breaking Changes': [],
    'New Features': [],
    'Reliability': [],
    'New Architecture-specific changes': [],
    Other: [],
  };

  const keywords = {
    'All Commits': [],
    'Breaking Changes': [
      'break',
      'remove',
      'deprecated',
      'incompatible',
      'remove support',
      'change api',
      'breaking',
    ],
    'New Features': ['feature', 'add', 'introduce', 'support', 'enable'],
    'Reliability': ['fix', 'bug', 'error', 'issue', 'crash', 'fault', 'defect', 'patch'],
    'New Architecture-specific changes': [
      'implement',
      'new',
      'fabric',
      'arch',
      'modal',
      'architecture',
      'refactor',
      'restructure',
      'modularize',
    ],
  };

  for (const c of commits) {
    const msg = c.commit.message;
    const lowerMsg = msg.toLowerCase();
    const sha = c.sha.slice(0, 7);
    const url = c.html_url;
    const entry = `- ${msg.split('\n')[0]} [${msg.split('\n')[0]} · ${REPO}@${sha} (github.com)](${url})`;

    const matched = Object.keys(keywords).filter((k) =>
      keywords[k].some((word) => lowerMsg.includes(word))
    );
    const category = matched.includes('Breaking Changes')
      ? 'Breaking Changes'
      : matched.includes('New Features')
      ? 'New Features'
      : matched.includes('Reliability')
      ? 'Reliability'
      : matched.includes('New Architecture-specific changes')
      ? 'New Architecture-specific changes'
      : 'Other';

    categories['All Commits'].push(entry);
    categories[category].push(entry);
  }

  return categories;
}

function generateReleaseNotes(commits, categories) {
  const start = formatDate(START || new Date(commits[0]?.commit.author.date));
  const end = formatDate(END || new Date(commits.at(-1)?.commit.author.date));

  const lines = [];
  lines.push(`${RELEASE_TAG} Release Notes\n`);
  lines.push(
    `We're excited to release React Native Windows ${RELEASE_TAG} targeting React Native ${RELEASE_TAG}!`
  );
  lines.push(`This release includes the commits to React Native Windows from ${start} - ${end}.\n`);
  lines.push('## How to upgrade');
  lines.push(
    'You can view the changes made to the default new React Native Windows applications for C++ and C# using React Native Upgrade Helper. See this [document](https://microsoft.github.io/react-native-windows/docs/upgrade-app) for more details.\n'
  );

  for (const [category, entries] of Object.entries(categories)) {
    if (entries.length > 0) {
      lines.push(`## ${category}`);
      lines.push(...entries);
      lines.push('');
    }
  }

  return lines.join('\n');
}

async function main() {
  const commits = await fetchCommits();
  const filtered = filterCommitsByDate(commits);
  const categories = categorizeCommits(filtered);
  const notes = generateReleaseNotes(filtered, categories);
  fs.writeFileSync('release_notes.md', notes, 'utf8');
}

main().catch((err) => {
  console.error('Failed to generate release notes:', err);
  process.exit(1);
});
