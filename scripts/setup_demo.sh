#!/bin/bash

cd zia-demo-site

if [ $# -ne 1 ]; then
    echo "Usage: ./scripts/setup_demo.sh <host>"
    exit 1
fi

HOST=$1
find . -type f -exec sed -i "s@__HOST__@${HOST}@" {} \;
