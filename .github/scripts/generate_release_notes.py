import os
import requests
import re
from datetime import datetime
from collections import defaultdict
from datetime import timezone

GITHUB_TOKEN = os.environ['GITHUB_TOKEN']
REPO = os.environ['GITHUB_REPOSITORY']
RELEASE_TAG = os.environ.get('RELEASE_TAG', 'Unreleased')
START_DATE_STR = os.environ.get('START_DATE')  # expected format: yyyy-mm-dd
END_DATE_STR = os.environ.get('END_DATE')      # expected format: yyyy-mm-dd

def parse_date_aware(date_str):
    # append UTC offset if missing and parse
    if date_str and 'T' not in date_str:
        date_str = date_str + "T00:00:00+00:00"
    elif date_str and date_str.endswith('Z'):
        date_str = date_str.replace('Z', '+00:00')
    return datetime.fromisoformat(date_str) if date_str else None

START_DATE = parse_date_aware(START_DATE_STR)
END_DATE = parse_date_aware(END_DATE_STR)

API_URL = f"https://api.github.com/repos/{REPO}/commits"
HEADERS = {"Authorization": f"token {GITHUB_TOKEN}"}

def fetch_commits():
    commits = []
    page = 1
    params = {
        "per_page": 100,
    }
    if START_DATE_STR:
        params["since"] = START_DATE_STR + "T00:00:00Z"
    if END_DATE_STR:
        params["until"] = END_DATE_STR + "T23:59:59Z"

    while True:
        params["page"] = page
        response = requests.get(API_URL, headers=HEADERS, params=params)
        data = response.json()
        if not data or 'message' in data:
            break
        commits.extend(data)
        page += 1
    return commits

def is_bot_commit(commit):
    # Same as before, skip commits by known bots (dependabot, etc.)
    author = commit.get('author')
    commit_author_name = commit['commit']['author']['name'].lower() if commit['commit']['author']['name'] else ''
    author_login = author.get('login', '').lower() if author else ''
    bot_indicators = ['bot', 'dependabot', 'actions-user']
    if any(bot_name in author_login for bot_name in bot_indicators):
        return True
    if any(bot_name in commit_author_name for bot_name in bot_indicators):
        return True

    # Also skip commits with messages indicating package bumps or updates
    message = commit['commit']['message'].lower()

    # List of keywords to detect in commit messages
    keywords = ['bump', 'applying package updates', 'no_ci', 'no ci']

    for keyword in keywords:
        if keyword in message:
            return True

    return False

def filter_commits_by_date(commits):
    if not START_DATE and not END_DATE:
        filtered = []
        for c in commits:
            if not is_bot_commit(c):
                filtered.append(c)
        return filtered

    filtered = []
    for commit in commits:
        if is_bot_commit(commit):
            continue
        commit_date = datetime.fromisoformat(commit['commit']['author']['date'].replace("Z", "+00:00"))
        if START_DATE and commit_date < START_DATE:
            continue
        if END_DATE and commit_date > END_DATE:
            continue
        filtered.append(commit)
    return filtered

def categorize_commits(commits):
    categories = {
        "New Architecture-specific changes": [],
        "Reliability": [],
        "New Features": [],
        "Breaking Changes": [],
        "Other": []
    }
    
    # Keywords for each category (lowercase)
    keywords = {
        "Reliability": ["fix", "bug", "error", "issue", "crash", "fault", "defect", "patch"],
        "New Features": ["feature", "add", "implement", "introduce", "support", "enable"],
        "Breaking Changes": ["break", "remove", "deprecated", "incompatible", "remove support", "change api"],
        "New Architecture-specific changes": ["[fabric]","fabric", "arch", "architecture", "refactor", "restructure", "modularize"]
    }
    
    for commit in commits:
        message = commit['commit']['message']
        sha = commit['sha']
        url = commit['html_url']
        entry = f"- {message.splitlines()[0]} [{message.splitlines()[0]} · microsoft/react-native-windows@{sha[:7]} (github.com)]({url})"
        msg_lower = message.lower()
        
        # Track which categories matched to avoid multiple assignments
        matched_categories = []
        
        for category, keys in keywords.items():
            if any(key in msg_lower for key in keys):
                matched_categories.append(category)
        
        # Prioritize categories by order: Breaking > New Features > Reliability > Architecture > Other
        if "Breaking Changes" in matched_categories:
            categories["Breaking Changes"].append(entry)
        elif "New Features" in matched_categories:
            categories["New Features"].append(entry)
        elif "Reliability" in matched_categories:
            categories["Reliability"].append(entry)
        elif "New Architecture-specific changes" in matched_categories:
            categories["New Architecture-specific changes"].append(entry)
        else:
            categories["Other"].append(entry)
    
    return categories

def generate_release_notes(commits, categories):
    if commits:
        # Use input dates if provided, else fallback to commit dates
        start_date = START_DATE_STR or datetime.fromisoformat(
            commits[0]['commit']['author']['date'].replace("Z", "+00:00")).strftime("%Y-%m-%d")
        end_date = END_DATE_STR or datetime.fromisoformat(
            commits[-1]['commit']['author']['date'].replace("Z", "+00:00")).strftime("%Y-%m-%d")
        # Format to mm/dd/yyyy for release notes
        start_date_fmt = datetime.fromisoformat(start_date).strftime("%m/%d/%Y")
        end_date_fmt = datetime.fromisoformat(end_date).strftime("%m/%d/%Y")
    else:
        start_date_fmt = START_DATE_STR or "N/A"
        end_date_fmt = END_DATE_STR or "N/A"

    notes = []
    notes.append(f"{RELEASE_TAG} Release Notes")
    notes.append("")
    notes.append(
        f"We're excited to release React Native Windows {RELEASE_TAG} targeting React Native {RELEASE_TAG}! "
        f"There have been many changes to both react-native-windows and react-native itself, and we would love your "
        f"feedback on anything that doesn't work as expected. This release includes the commits to React Native Windows "
        f"from {start_date_fmt} - {end_date_fmt}."
    )
    notes.append("")
    notes.append("## How to upgrade")
    notes.append("You can view the changes made to the default new React Native Windows applications for C++ and C# "
                 "using React Native Upgrade Helper. See this [document](https://microsoft.github.io/react-native-windows/docs/upgrade-app) for more details.")
    notes.append("")
    for category, entries in categories.items():
        if entries:
            notes.append(f"## {category}")
            notes.extend(entries)
            notes.append("")
    return "\n".join(notes)

def main():
    commits = fetch_commits()
    commits = filter_commits_by_date(commits)
    categories = categorize_commits(commits)
    release_notes = generate_release_notes(commits, categories)
    with open("release_notes.md", "w", encoding="utf-8") as f:
        f.write(release_notes)

if __name__ == "__main__":
    main()
