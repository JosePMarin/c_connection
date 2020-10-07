#!/bin/sh


rm -r external
mkdir -p external

cd external

git submodule add --force https://github.com/chriskohlhoff/asio asio
git submodule init && git submodule update

