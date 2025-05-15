import os
import requests
from datetime import datetime
from collections import defaultdict

GITHUB_TOKEN = os.environ['GITHUB_TOKEN']
REPO = os.environ['GITHUB_REPOSITORY']
RELEASE_TAG = os.environ.get('RELEASE_TAG', 'Unreleased')
API_URL = f"https://api.github.com/repos/{REPO}/commits"
HEADERS = {"Authorization": f"token {GITHUB_TOKEN}"}

def fetch_commits():
    commits = []
    page = 1
    while True:
        response = requests.get(API_URL, headers=HEADERS, params={"per_page": 100, "page": page})
        data = response.json()
        if not data or 'message' in data:
            break
        commits.extend(data)
        page += 1
    return commits

def categorize_commits(commits):
    categories = {
        "Reliability": [],
        "New Features": [],
        "Breaking Changes": [],
        "Other": [],
        "New Architecture-specific changes": []
    }
    for commit in commits:
        message = commit['commit']['message']
        sha = commit['sha']
        url = commit['html_url']
        entry = f"- {message.splitlines()[0]} [{sha[:7]}]({url})"
        # Simple categorization based on keywords
        if "fix" in message.lower():
            categories["Reliability"].append(entry)
        elif "feature" in message.lower() or "add" in message.lower():
            categories["New Features"].append(entry)
        elif "break" in message.lower() or "remove" in message.lower():
            categories["Breaking Changes"].append(entry)
        elif "arch" in message.lower():
            categories["New Architecture-specific changes"].append(entry)
        else:
            categories["Other"].append(entry)
    return categories

def generate_release_notes(commits, categories):
    dates = [commit['commit']['author']['date'] for commit in commits]
    dates = sorted(dates)
    start_date = datetime.fromisoformat(dates[0].replace("Z", "+00:00")).strftime("%m/%d/%Y")
    end_date = datetime.fromisoformat(dates[-1].replace("Z", "+00:00")).strftime("%m/%d/%Y")

    notes = []
    notes.append(f"{RELEASE_TAG} Release Notes")
    notes.append("")
    notes.append(f"We're excited to release React Native Windows {RELEASE_TAG} targeting React Native {RELEASE_TAG}! There have been many changes to both react-native-windows and react-native itself, and we would love your feedback on anything that doesn't work as expected. This release includes the commits to React Native Windows from {start_date} - {end_date}.")
    notes.append("")
    notes.append("## How to upgrade")
    notes.append("You can view the changes made to the default new React Native Windows applications for C++ and C# using React Native Upgrade Helper. See this [document](https://microsoft.github.io/react-native-windows/docs/upgrade-app) for more details.")
    notes.append("")
    for category, entries in categories.items():
        if entries:
            notes.append(f"## {category}")
            notes.extend(entries)
            notes.append("")
    return "\n".join(notes)

def main():
    commits = fetch_commits()
    categories = categorize_commits(commits)
    release_notes = generate_release_notes(commits, categories)
    with open("release_notes.md", "w", encoding="utf-8") as f:
        f.write(release_notes)

if __name__ == "__main__":
    main()
