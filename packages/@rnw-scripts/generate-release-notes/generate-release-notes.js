import fetch from "node-fetch";
import fs from "fs";
import process from "process";

function printHelp() {
  console.log(`
Usage:
  yarn release-notes --token <GITHUB_TOKEN> --start <START_DATE> --end <END_DATE> [--repo <OWNER/REPO>] [--tag <RELEASE_TAG>]

Options:
  --token       (required) GitHub personal access token.
  --start       (required) Start date in YYYY-MM-DD.
  --end         (required) End date in YYYY-MM-DD.
  --repo        Repository in OWNER/REPO format. Default: microsoft/react-native-windows
  --tag         Release tag label. Default: Unreleased
  --help        Show this help message.
`);
}

function parseArgs() {
  const args = process.argv.slice(2);
  const options = {};
  for (let i = 0; i < args.length; i++) {
    const arg = args[i];
    if (arg === "--help") {
      printHelp();
      process.exit(0);
    } else if (arg === "--token") {
      options.token = args[++i];
    } else if (arg === "--start") {
      options.start = args[++i];
    } else if (arg === "--end") {
      options.end = args[++i];
    } else if (arg === "--repo") {
      options.repo = args[++i];
    } else if (arg === "--tag") {
      options.tag = args[++i];
    } else {
      console.error(`Unknown argument: ${arg}`);
      printHelp();
      process.exit(1);
    }
  }
  return options;
}

const args = parseArgs();

const GITHUB_TOKEN = args.token;
const REPO = args.repo || "microsoft/react-native-windows";
const RELEASE_TAG = args.tag || "Unreleased";
const START_DATE = args.start;
const END_DATE = args.end;

if (!GITHUB_TOKEN || !START_DATE || !END_DATE) {
  console.error("Error: --token, --start, and --end are required.");
  printHelp();
  process.exit(1);
}

console.log(`Generating release notes for ${REPO} from ${START_DATE} to ${END_DATE}...`);

const HEADERS = {
  Authorization: `token ${GITHUB_TOKEN}`,
  Accept: "application/vnd.github+json",
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

function extractTypeOfChange(prDescription) {
  if (!prDescription) return null;
  
  // Look for the "### Type of Change" section
  const marker = '### Type of Change';
  const markerIndex = prDescription.indexOf(marker);
  
  if (markerIndex === -1) return null;
  
  // Get text after the marker until the next section (###) or end
  const afterMarker = prDescription.substring(markerIndex + marker.length);
  const nextSectionIndex = afterMarker.indexOf('###');
  const sectionText = nextSectionIndex !== -1 
    ? afterMarker.substring(0, nextSectionIndex)
    : afterMarker;
  
  // Convert to lowercase for easier matching
  const lowerSectionText = sectionText.toLowerCase();
  
  // Check for each type of change
  if (lowerSectionText.includes('bug fix')) {
    return 'Bug fix';
  } else if (lowerSectionText.includes('new feature')) {
    return 'New feature';
  } else if (lowerSectionText.includes('breaking change')) {
    return 'Breaking change';
  }
  
  return null;
}

async function categorizeCommits(commits) {
  const categories = {
    'All Commits': [],
    'Breaking Changes': [],
    'New Features': [],
    'Reliability': [],
    'New Architecture-specific changes': [],
    Other: [],
  };

  for (const c of commits) {
    const msg = c.commit.message;
    const sha = c.sha.slice(0, 7);
    const url = c.html_url;
    const commitTitle = msg.split('\n')[0];
    
    // Try to get a better summary from PR description
    const prNumber = extractPRNumber(commitTitle);
    let summary = commitTitle;
    let shouldInclude = true; // Default to include if we can't determine
    let category = 'Other'; // Default category
    
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
          
          // Determine category based on PR description "Type of Change"
          const typeOfChange = extractTypeOfChange(prDetails.body);
          const prTitle = prDetails.title || '';
          const prDescription = prDetails.body || '';
          
          // Check for special architecture keywords first
          const lowerTitle = prTitle.toLowerCase();
          const lowerDescription = prDescription.toLowerCase();
          const hasArchKeywords = lowerTitle.includes('fabric') || 
                                  lowerTitle.includes('implement') || 
                                  lowerTitle.includes('prop');
          
          if (hasArchKeywords) {
            category = 'New Architecture-specific changes';
          } else if (typeOfChange === 'Bug fix') {
            category = 'Reliability';
          } else if (typeOfChange === 'New feature') {
            category = 'New Features';
          } else if (typeOfChange === 'Breaking change') {
            category = 'Breaking Changes';
          } else {
            category = 'Other';
          }
          
          console.log(`PR #${prNumber}: Type of Change = "${typeOfChange}", Category = "${category}"`);
        } else {
          console.log(`Skipping PR #${prNumber} - not marked for inclusion in release notes`);
          continue; // Skip this commit
        }
      }
    }
    
    const entry = `- ${summary} [${commitTitle} · ${REPO}@${sha} (github.com)](${url})`;

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