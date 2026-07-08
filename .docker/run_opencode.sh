#!/bin/bash

DOCKER_PATH=$(dirname $0)
BRANCH=$(git -C $DOCKER_PATH branch --show-current  | tr '[:upper:]' '[:lower:]' | tr -cd '[:lower:][:digit:]-_')
DOCKER_COMPOSE="docker compose --project-directory $DOCKER_PATH -p $BRANCH"

$DOCKER_COMPOSE up -d
$DOCKER_COMPOSE exec qtd opencode
