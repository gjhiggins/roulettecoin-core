#!/bin/bash
set -e

if [ "$1" = 'vcored' ]; then
    mkdir -p "$VCORE_DATA"

    if [ ! -s "$VCORE_DATA/vcore.conf" ]; then
        cat <<-EOF > "$VCORE_DATA/vcore.conf"
        printtoconsole=1
        rpcpassword=${VCORE_RPC_PASSWORD:-$(dd if=/dev/urandom bs=22 count=1 status=none | base64)}
        rpcuser=${VCORE_RPC_USER:-vcoreuser}
        dns=1
        noirc=1
        EOF
    fi

    if [ ! -e "$VCORE_RPC_PASSWORD" ]; then
        cat $VCORE_DATA/vcore.conf
    fi

    chown -R vcore "$VCORE_DATA"
    exec gosu vcore "$@"
fi

exec "$@"