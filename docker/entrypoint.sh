#!/bin/bash
set -e

export MAKEFLAGS

if [[ "$*" ]] ; then
    su student -c "env PATH=$PATH $*"
else
    su - student
fi
