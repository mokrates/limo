#!/bin/bash

echo $(
    for mod in $(cat inlined.mods | grep -v '^#'); do
	bash libs/"$mod"/ldflags | grep -v '^#'
    done
    )
