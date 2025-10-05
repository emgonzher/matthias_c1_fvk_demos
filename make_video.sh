#!/bin/bash
# Usage: ./make_video.sh input_prefix fps duration output.mp4
# Example: ./make_video.sh anim 30 10 output.mp4

set -e

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 input_prefix fps duration output.mp4"
  exit 1
fi

prefix=$1      # e.g. anim
fps=$2         # e.g. 30
duration=$3    # e.g. 10 (seconds)
output=$4      # e.g. output.mp4

# Calculate required number of frames = fps × duration
frames=$((fps * duration))

echo "Generating video from $frames frames ($fps fps, $duration s)..."

ffmpeg -y -framerate $fps -i "${prefix}.%04d.png" \
  -frames:v $frames \
  -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" \
  -c:v libx264 -crf 18 -pix_fmt yuv420p "$output"

