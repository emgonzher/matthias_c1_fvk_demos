#!/bin/bash

# 1️⃣ Create backup folder
BACKUP_DIR=./conflict_backup_$(date +%Y%m%d_%H%M%S)
mkdir -p "$BACKUP_DIR"

# 2️⃣ Get list of conflicted files
CONFLICT_FILES=$(git diff --name-only --diff-filter=U)

if [ -z "$CONFLICT_FILES" ]; then
    echo "✅ No conflicted files found."
    exit 0
fi

# 3️⃣ Backup conflicted files
for file in $CONFLICT_FILES; do
    mkdir -p "$BACKUP_DIR/$(dirname "$file")"
    cp "$file" "$BACKUP_DIR/$file"
done

echo "✅ Conflict files backed up at $BACKUP_DIR"

# 4️⃣ Keep remote version for all conflict files
for file in $CONFLICT_FILES; do
    git checkout --theirs -- "$file"
    git add "$file"
done

# 5️⃣ Finalize the merge
git commit -m "Kept remote version for all conflicted files"

echo "✅ Merge completed. Remote version kept for all conflicts."

