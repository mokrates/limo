#!/bin/bash

echo $(
    for mod in $(cat inlined.mods | grep -v '^#'); do
	cat libs/"$mod"/ldflags | grep -v '^#'
    done
    )
