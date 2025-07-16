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

function extractPRNumber(commitMessage) {
  // Extract PR number from commit message like "(#14813)"
  const match = commitMessage.match(/\(#(\d+)\)/);
  return match ? parseInt(match[1]) : null;
}

async function fetchPRDetails(prNumber) {
  if (!prNumber) return null;
  
  try {
    const url = `https://api.github.com/repos/${REPO}/pulls/${prNumber}`;
    const res = await fetch(url, { headers: HEADERS });
    
    if (!res.ok) {
      console.warn(`Failed to fetch PR #${prNumber}: ${res.status}`);
      return null;
    }
    
    return await res.json();
  } catch (error) {
    console.warn(`Error fetching PR #${prNumber}:`, error.message);
    return null;
  }
}

function shouldIncludeInReleaseNotes(prDescription) {
  if (!prDescription) return false;
  
  // Look for the inclusion marker
  const marker = 'Should this change be included in the release notes:';
  const markerIndex = prDescription.indexOf(marker);
  
  if (markerIndex === -1) return true;
  
  // Get text after the marker
  const afterMarker = prDescription.substring(markerIndex + marker.length);
  
  // Extract the next line or paragraph after the marker
  const lines = afterMarker.split('\n').map(line => line.trim()).filter(line => line.length > 0);
  
  if (lines.length === 0) return false;
  
  // Check if the first non-empty line contains "no" or "_no_"
  const firstLine = lines[0].toLowerCase();
  return !(firstLine.includes('no') || firstLine.includes('_no_'));
}

function extractReleaseNotesSummary(prDescription) {
  if (!prDescription) return null;
  
  // Look for the release notes summary marker
  const marker = 'Add a brief summary of the change to use in the release notes for the next release.';
  const markerIndex = prDescription.indexOf(marker);
  
  if (markerIndex === -1) return null;
  
  // Get text after the marker
  const afterMarker = prDescription.substring(markerIndex + marker.length);
  
  // Split into lines and get all non-empty lines
  const lines = afterMarker.split('\n')
    .map(line => line.trim())
    .filter(line => line.length > 0);
  
  if (lines.length === 0) return null;
  
  // Get the first non-empty line after the marker
  let summary = lines[0];
  
  // Remove Microsoft Reviewers text if it exists anywhere in the summary
  const reviewersMarker = 'Microsoft Reviewers: [Open in CodeFlow';
  if (summary.includes(reviewersMarker)) {
    const reviewersIndex = summary.indexOf(reviewersMarker);
    summary = summary.substring(0, reviewersIndex).trim();
  }
  
  // Filter out lines that contain Microsoft Reviewers text
  if (!summary || summary.length === 0) {
    // Try the next lines if the first one was entirely Microsoft Reviewers text
    for (let i = 1; i < lines.length; i++) {
      const line = lines[i];
      if (!line.includes('Microsoft Reviewers: [Open in CodeFlow')) {
        summary = line;
        break;
      }
    }
  }
  
  return summary && summary.length > 0 ? summary : null;
}

async function categorizeCommits(commits) {
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
    const commitTitle = msg.split('\n')[0];
    
    // Try to get a better summary from PR description
    const prNumber = extractPRNumber(commitTitle);
    let summary = commitTitle;
    let shouldInclude = true; // Default to include if we can't determine
    
    if (prNumber) {
      console.log(`Fetching PR details for #${prNumber}...`);
      const prDetails = await fetchPRDetails(prNumber);
      if (prDetails) {
        // Check if this PR should be included in release notes
        shouldInclude = shouldIncludeInReleaseNotes(prDetails.body);
        
        if (shouldInclude) {
          const releaseNotesSummary = extractReleaseNotesSummary(prDetails.body);
          if (releaseNotesSummary) {
            summary = releaseNotesSummary;
            console.log(`Found release notes summary for PR #${prNumber}: ${summary}`);
          }
        } else {
          console.log(`Skipping PR #${prNumber} - not marked for inclusion in release notes`);
          continue; // Skip this commit
        }
      }
    }
    
    const entry = `- ${summary} [${commitTitle} · ${REPO}@${sha} (github.com)](${url})`;

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
  const categories = await categorizeCommits(filtered);
  const notes = generateReleaseNotes(filtered, categories);
  fs.writeFileSync('release_notes.md', notes, 'utf8');
}

main().catch((err) => {
  console.error('Failed to generate release notes:', err);
  process.exit(1);
});