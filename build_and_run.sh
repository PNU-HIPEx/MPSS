#!/usr/bin/env bash
set -e

echo "Starting HTCondor build-and-run wrapper"

# Some execute nodes start with minimal environment.
export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:${PATH}"

# Load common ROOT/Geant4 environments if available on execute node.
if [ -f /opt/root/bin/thisroot.sh ]; then
  . /opt/root/bin/thisroot.sh
fi

if [ -f /opt/geant4/bin/geant4.sh ]; then
  . /opt/geant4/bin/geant4.sh
fi

if [ -f /opt/geant4/bin/geant4make.sh ]; then
  . /opt/geant4/bin/geant4make.sh
fi

# If the source tarball exists, unpack it in the current directory.
if [ -f MPSS-source.tar.gz ]; then
  echo "Extracting MPSS source tarball"
  tar -xzf MPSS-source.tar.gz
fi

mkdir -p build
cd build

# Configure and build the project
echo "Configuring with cmake"
SCRATCH_BASE=${_CONDOR_SCRATCH_DIR:-$(pwd)}
DATA_DIR_ARG=${DATA_DIR:-${SCRATCH_BASE}/Data}
NAS_SYNC_DIR_ARG=${NAS_SYNC_DIR:-/mnt/condor_data}
echo "Using DATA_DIR=${DATA_DIR_ARG}"
echo "NAS_SYNC_DIR=${NAS_SYNC_DIR_ARG}"
cmake -DDATA_DIR="${DATA_DIR_ARG}" ..

make -j8

# Run the executable with any arguments passed from condor
set +e
./MPSS.out --threads 10
RUN_RC=$?
set -e
echo "Run exit code: ${RUN_RC}"

# Sync results to NAS only when the run succeeded.
if [ "${RUN_RC}" -eq 0 ] && [ -n "${NAS_SYNC_DIR_ARG}" ]; then
  JOB_TAG_ARG=${JOB_TAG:-$(date +%Y%m%d_%H%M%S)_$$}
  DEST_DIR="${NAS_SYNC_DIR_ARG}/${JOB_TAG_ARG}"
  echo "Syncing outputs to ${DEST_DIR}"
  mkdir -p "${DEST_DIR}"
  if [ ! -d "${DATA_DIR_ARG}" ]; then
    echo "Warning: DATA_DIR not found (${DATA_DIR_ARG}), skipping sync"
    exit "${RUN_RC}"
  fi
  if command -v rsync >/dev/null 2>&1; then
    rsync -a "${DATA_DIR_ARG}/" "${DEST_DIR}/"
  else
    cp -a "${DATA_DIR_ARG}/." "${DEST_DIR}/"
  fi
  echo "Sync complete: ${DEST_DIR}"
fi

exit "${RUN_RC}"
