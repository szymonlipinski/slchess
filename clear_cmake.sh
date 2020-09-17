#!/bin/bash

# shellcheck disable=SC2038
find . -name 'CMakeFiles' | xargs rm -rf
find . -name '*.cmake' | xargs rm -rf