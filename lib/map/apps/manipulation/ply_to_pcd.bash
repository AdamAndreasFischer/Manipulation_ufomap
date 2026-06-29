#!/usr/bin/env bash

regex="[0-9]+_([a-z]+)_[0-9a-z]*"

mkdir -p "$1/clouds"

readarray -d '' entries < <(printf '%s\0' $1/**/*.ply | sort -zV)

i=0
for entry in "${entries[@]}"; do
	((i=i+1))
	sed -i -e 's/double/float/g' $entry
	number=$(printf '%04d' "$i")
	out_file="$1/clouds/cloud_$number.pcd"
	pcl_ply2pcd "$entry" "$out_file" &
done

wait