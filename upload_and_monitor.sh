#!/bin/bash

set -ex
pio -c clion run --target upload -e Debug
pio -c clion device monitor -e Debug
