# XMRig (Doug Edition)

[![Github All Releases](https://img.shields.io/github/downloads/xmrig/xmrig/total.svg)](https://github.com/xmrig/xmrig/releases)
[![GitHub release](https://img.shields.io/github/release/xmrig/xmrig/all.svg)](https://github.com/xmrig/xmrig/releases)
[![GitHub Release Date](https://img.shields.io/github/release-date/xmrig/xmrig.svg)](https://github.com/xmrig/xmrig/releases)
[![GitHub license](https://img.shields.io/github/license/xmrig/xmrig.svg)](https://github.com/xmrig/xmrig/blob/master/LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/xmrig/xmrig.svg)](https://github.com/xmrig/xmrig/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/xmrig/xmrig.svg)](https://github.com/xmrig/xmrig/network)

XMRig (Doug Edition) is a high performance, open source, cross platform RandomX unified CPU/GPU miner and [RandomX benchmark](https://xmrig.com/benchmark). Official binaries are available for Windows, Linux, macOS. 

I made this fork for myself. Explicitly, only Monero (currently RandomX/rx0 algo) CPU mining will be supported by me. 

## Fork-unique features: 
* 60% lower default-donation percentage (0.4%) - configurable down to 0.2%. Recompile to 0, if you please. 
* 30-second donation mining bursts, instead of stock 1-minute for every 100ish. 
* opt-in block-candidate notification

By default, xmrig tracks share-diff, but has no source of truth for net-diff. Since a block-candidate by definition is when accepted share difficulty is greater than network difficulty, this means 
xmrig has no ability to tell you when you've (most likely) found a block. The opportunity cost for this feature is essentially 1 integer comparison per accepted share as that is the only ops done "within the mining loop" - in 
other words, this feature does not affect hashrate whatsoever. 
It is also opt-in, with only 1 mandatory flag and 2 settings-flags. 
```
      --candidate-rpc=URL       Monero get_info URL for block-candidate alerts
      --candidate-refresh=N     candidate RPC refresh seconds (default: 30)
      --candidate-max-stale=N   maximum cache age seconds (default: 120)
```   
Like other xmrig options, they can be passed as an argument on the cli....: 


<img width="1092" height="42" alt="image" src="https://github.com/user-attachments/assets/d6a154df-8b97-4868-aed4-e14a15fa43e8" />

....or placed within the config.json block: 


<img width="328" height="129" alt="image" src="https://github.com/user-attachments/assets/84be4794-568b-40a4-adde-55dd62f115a9" />


--candidate-rpc is designed to take a monero node rpc /get_info as its' argument, though anything that fits the same shape and has network-difficulty defined similarly can be substituted. 

You can use your own node, my node, or omit it entirely. 

Upstream fixes will be merged as appropriate: our versioning system will simply be xmrig's version and then -fork.<version>. So, the current release tag is  v6.26.0-fork.2 
This is nice because even though you are using a fork, you know the xmrig base it depends on strictly from our release/versioning. 

## Mining backends
- **CPU** (x86/x64/ARMv7/ARMv8/RISC-V)
- **OpenCL** for AMD GPUs.
- **CUDA** for NVIDIA GPUs via external [CUDA plugin](https://github.com/xmrig/xmrig-cuda).

## Download
* **[Binary releases](https://github.com/xmrig/xmrig/releases)**
* **[Build from source](https://xmrig.com/docs/miner/build)**

## Usage
The preferred way to configure the miner is the [JSON config file](https://xmrig.com/docs/miner/config) as it is more flexible and human friendly. The [command line interface](https://xmrig.com/docs/miner/command-line-options) does not cover all features, such as mining profiles for different algorithms. Important options can be changed during runtime without miner restart by editing the config file or executing [API](https://xmrig.com/docs/miner/api) calls.

* **[Wizard](https://xmrig.com/wizard)** helps you create initial configuration for the miner.
* **[Workers](http://workers.xmrig.info)** helps manage your miners via HTTP API.

## Donations
* Default donation is 60% less from stock xmrig: only 0.4% (30 seconds every 125 minutes) instead of 1% (1 minute every 100 minutes) can be decreased/increased via option `donate-level` or disabled entirely in source code.
* If you recompile with zero donation please consider making a donation to the original xmrig developer. 
* XMR: `48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD`

## Developers
* **[xmrig](https://github.com/xmrig)**
* **[sech1](https://github.com/SChernykh)**

## Contacts
* support@xmrig.com
* [reddit](https://www.reddit.com/user/XMRig/)
* [twitter](https://twitter.com/xmrig_dev)
