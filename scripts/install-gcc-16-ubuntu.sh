#!/usr/bin/env bash
set -euo pipefail

readonly GCC_VERSION="16.2.0"
readonly GCC_SHA512="c51c30ca7422d0cbecf504b2e0f33c3aca31e0f90a76b65217f465163fa6fa17b3f5de39e145c47e5bab90ac0ce7fff3b03c8d553ae36e01faaea5a50f8648d1"
readonly INSTALL_ROOT="${HOME}/.local/opt/gcc-${GCC_VERSION}"
readonly LOCAL_BIN="${HOME}/.local/bin"

link_compilers() {
    mkdir -p "${LOCAL_BIN}"
    for tool in gcc-16 g++-16; do
        target="${LOCAL_BIN}/${tool}"
        expected="${INSTALL_ROOT}/bin/${tool%-16}"
        if [[ -e "${target}" || -L "${target}" ]]; then
            [[ "$(readlink -f "${target}")" == "${expected}" ]] || {
                printf 'ERROR: refusing to replace existing %s at %s.\n' "${tool}" "${target}" >&2
                return 1
            }
        else
            ln -s "${expected}" "${target}"
        fi
    done
}

if [[ -x "${INSTALL_ROOT}/bin/g++" ]]; then
    "${INSTALL_ROOT}/bin/g++" -dumpfullversion | grep -Fx "${GCC_VERSION}"
    link_compilers
    exit 0
elif [[ -e "${INSTALL_ROOT}" ]]; then
    printf 'ERROR: refusing to overwrite the existing GCC installation at %s.\n' \
        "${INSTALL_ROOT}" >&2
    exit 1
fi

for tool in gcc-16 g++-16; do
    target="${LOCAL_BIN}/${tool}"
    expected="${INSTALL_ROOT}/bin/${tool%-16}"
    if [[ -e "${target}" || -L "${target}" ]] &&
       [[ "$(readlink -f "${target}")" != "${expected}" ]]; then
        printf 'ERROR: refusing to replace existing %s at %s.\n' "${tool}" "${target}" >&2
        exit 1
    fi
done

for tool in gcc g++ make curl pkg-config sha512sum; do
    command -v "${tool}" >/dev/null 2>&1 || {
        printf 'ERROR: %s is required to build GCC %s.\n' "${tool}" "${GCC_VERSION}" >&2
        exit 1
    }
done

for library in gmp mpfr; do
    pkg-config --exists "${library}" || {
        printf 'ERROR: development files for %s are required (install lib%s-dev).\n' \
            "${library}" "${library}" >&2
        exit 1
    }
done

[[ -f /usr/include/mpc.h ]] &&
    ldconfig -p | grep -q 'libmpc\.so' || {
        printf 'ERROR: development files for mpc are required (install libmpc-dev).\n' >&2
        exit 1
    }

work_root="$(mktemp -d)"
archive="${work_root}/gcc-${GCC_VERSION}.tar.xz"
source_dir="${work_root}/gcc-${GCC_VERSION}"
build_dir="${work_root}/build"
trap 'rm -rf -- "${work_root}"' EXIT

curl -fsSL \
    "https://gcc.gnu.org/pub/gcc/releases/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz" \
    -o "${archive}"
printf '%s  %s\n' "${GCC_SHA512}" "${archive}" | sha512sum --check
tar -xf "${archive}" -C "${work_root}"
mkdir -p "${build_dir}"

(
    cd "${build_dir}"
    CC=gcc CXX=g++ \
        "${source_dir}/configure" \
            --prefix="${INSTALL_ROOT}" \
            --enable-languages=c,c++ \
            --disable-bootstrap \
            --disable-multilib \
            --disable-nls
    make -j"${JOBS:-$(nproc)}"
    make install
)

link_compilers
"${LOCAL_BIN}/g++-16" -dumpfullversion | grep -Fx "${GCC_VERSION}"