#!/bin/bash

set -xa

gcc main.c -o main.out -lm -lraylib -Wall -Wextra
