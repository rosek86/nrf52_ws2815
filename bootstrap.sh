#!/bin/bash
cd "$(dirname "$0")"

unameInfo="$(uname -s)"
case "${unameInfo}" in
  Linux*)     machine=Linux;;
  Darwin*)    machine=Darwin;;
  *)          machine="UNKNOWN:${unameInfo}"
esac

if [[ "$machine" == "Darwin" ]]; then
  echo "MacOS - bootstrapping..."
  ./scripts/bootstrap_darwin.sh
elif [[ "$machine" == "Linux" ]]; then
  echo "Linux - unsupported"
else
  echo "Unknown machine - unsupported"
fi
