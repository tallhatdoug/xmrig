# Fork features

## Block-candidate alerts

Poll a Monero daemon get_info endpoint and compare each rx/0 result's actual
difficulty with the cached network difficulty. RPC polling and JSON parsing are
outside the submission hot path; the share-path check is one integer comparison.

Example:

    "block-candidates": {
      "rpc": "https://monero.ceo:18090/get_info",
      "refresh": 30,
      "max-stale": 120
    }

If no endpoint is configured, the feature is disabled. Failed refreshes keep a
previously valid cache only until max-stale; mining continues throughout.

POTENTIAL BLOCK CANDIDATE means the result met the last known network difficulty.
It does not prove pool, daemon, or canonical-chain acceptance.

## Donation schedule

Levels use 0.2% units and 30-second bursts:

- 0: disabled
- 1: 0.2%, approximately every 250 minutes
- 2: 0.4%, approximately every 125 minutes; default
- 5: 1.0%, approximately every 50 minutes

TLS builds use tallhatdoug.com:8888 over TLS and do not fall back to the upstream
XMRig donation service. The randomized upstream scheduling behavior is retained.
