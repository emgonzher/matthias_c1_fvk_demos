#!/bin/bash
# === Configuration ===
MAIN_REPO_PATH="$HOME/projects/myproject"              # Path to your main repo
BACKUP_REPO_PATH="$HOME/projects/myproject-results"    # Path to your backup repo
RESULTS_FOLDER_NAME="RESLT_egh"                        # Name of the results folder to back up

# === Get commit hash from main repo ===
cd "$MAIN_REPO_PATH" || exit
COMMIT_HASH=$(git rev-parse --short HEAD)
DATE_STR=$(date +%Y-%m-%d)

# === Create backup folder in backup repo ===
TARGET_FOLDER="$BACKUP_REPO_PATH/${DATE_STR}_commit-${COMMIT_HASH}"
mkdir -p "$TARGET_FOLDER"

# === Copy results folder ===
cp -r "$MAIN_REPO_PATH/$RESULTS_FOLDER_NAME" "$TARGET_FOLDER/"

# === Commit and push to backup repo ===
cd "$BACKUP_REPO_PATH" || exit
git add .
git commit -m "Backup $RESULTS_FOLDER_NAME from commit $COMMIT_HASH on $DATE_STR"
git push

echo "✅ Backup complete: $TARGET_FOLDER"

