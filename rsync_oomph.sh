#!/bin/bash
#
# Usage:
#   nohup ./rsync_sync.sh push <source_dir/> <user@remote:/dest_dir/> &
#   nohup ./rsync_sync.sh pull <user@remote:/source_dir/> <dest_dir/> &
#
# Examples:
#   nohup ./rsync_sync.sh push RESLT/ user@hpc:/scratch/myuser/RESLT/ &
#   nohup ./rsync_sync.sh pull user@hpc:/scratch/myuser/RESLT/ ./RESLT/ &
#

set -euo pipefail

if [ "$#" -ne 3 ]; then
    echo "Usage:"
    echo "  $0 push <source_dir/> <user@remote:/dest_dir/>"
    echo "  $0 pull <user@remote:/source_dir/> <dest_dir/>"
    exit 1
fi

MODE=$1
SOURCE=$2
DEST=$3

# Log file
LOGFILE="rsync_${MODE}_$(date +%Y%m%d_%H%M%S).log"

echo "=== Starting rsync $MODE ==="
echo "Source: $SOURCE"
echo "Dest:   $DEST"
echo "Log:    $LOGFILE"
echo "================================"

# Common rsync options
RSYNC_OPTS="-avh --partial --progress --info=progress2 --compress"

if [ "$MODE" == "push" ]; then
    rsync $RSYNC_OPTS "$SOURCE" "$DEST" | tee "$LOGFILE"
elif [ "$MODE" == "pull" ]; then
    rsync $RSYNC_OPTS "$SOURCE" "$DEST" | tee "$LOGFILE"
else
    echo "Error: MODE must be 'push' or 'pull'"
    exit 1
fi

echo "=== rsync $MODE complete ==="

