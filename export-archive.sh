#!/bin/bash
git archive \
    --format=zip \
    --prefix=rase/ \
    --output=../rase-MinXu-$(date +"%Y-%m-%d-%H-%M").zip HEAD .
