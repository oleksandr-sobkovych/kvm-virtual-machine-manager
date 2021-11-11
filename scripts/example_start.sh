#!/bin/bash -x
# This is an example of start.sh script (this particular can be used for mpi tasks)
#
#
# Main goal of this script - to automate compilation and execution of the project 
#     with only one script. Example containes compilation types debug (Debug) and 
#     optimize (Release), also "number of processes" for MPI run. 
#     It is easy scalable, to make possible change "number of threads", "input file", 
#     "output file" or "number of executions" and much more.
# 
# It can be used for "comparison of result files" or some other post-processing
#     because bash is well-known for it's text processing abilities
#
#
# It is really useful for "Architecture of computer systems" course, not so much 
#     for "Operating systems"

set -o errexit
set -o nounset
set -o pipefail

# config_filename=config.conf
number_of_processes=1
debug=false
debug_build=false
optimize_build=false

while true; do
  case $1 in
  -np | --number_of_processes)
    if [ "$2" -eq "$2" ] 2>/dev/null; then
      number_of_processes=$2
      shift 2
    else
      echo "Option -np requires an numerical argument." >&2
      exit 1
    fi
    ;;
  -D | --debug-build)
    debug_build=true
    shift
    ;;
  -O | --optimize-build)
    optimize_build=true
    shift
    ;;
  -h | --help)
    echo "Usage: ./start.sh [options]
  Options:
    -h      --help                  Show help message.
    -O      --optimize-build        Compile with optimization before executing.
    -D      --debug-build           Compile with debug options.
    -np     --number_of_processes   Number of processes for mpirun. Require second parameter. 2 by default."
    exit 0
    ;;
  \?)
    echo "Invalid option: -$OPTARG" >&2
    exit 1
    ;;
  :)
    echo "Option -$OPTARG requires an numerical argument." >&2
    exit 1
    ;;
  *)
    break
    ;;
  esac
done

# different prepare staff


if [[ "$debug_build" == true ]]; then
  mkdir -p ./cmake-build-debug
  (
    pushd ./cmake-build-debug > /dev/null || exit 1
    echo Compiling...
    cmake -DCMAKE_BUILD_TYPE=Debug -G"Unix Makefiles" .. || exit 1
    make || exit 1
    popd
  )
fi

if [[ "$optimize_build" == true ]]; then
  mkdir -p ./cmake-build-release
  (
    pushd ./cmake-build-release >/dev/null || exit 1
    echo Compiling...
    cmake -lpng -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles" .. || exit 1
    make || exit 1
    popd
  )
fi
