#!/bin/bash

set -ex

cd "$(dirname "$0")"/examples/webpage

pio -c clion run --target upload -e Debug
pio -c clion device monitor -e Debug
