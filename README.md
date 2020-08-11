# Tetra-kit

TETRA downlink decoder/recorder kit

![](screenshots/main.png)

Generalities
============

This project aim is to provide an extensible TETRA downlink receiver kit for RTL-SDR dongle with following ideas:
- Stays as close as possible to TETRA specification layers defined in `ETSI EN 300 392-2 v3.4.1
(2010-08)`
- Transmit downlink informations (including speech frames) in Json plain text format to be recorded or analyzed
by an external program
- Reassociate speech frames with a simple method based on associated `caller id` and `usage marker` (save messages transmitted simultaneously in separated files)

The decoder implements a soft synchronizer allowing missing frames (50 bursts) before loosing synchronization.

Workflow
========

The decoder get physical layer bits from gnuradio PI/4 DQPSK receiver and transmit TETRA downlink
informations in Json format to be analyzed and recorded.

Speech frames are compressed with `zlib` and coded in `Base64` to be transmitted in Json text.

The 3 parts are linked with UDP sockets:

    Physical (TX on UDP port 42000) -> receiver (TX on UDP port 42100) -> recorder

Physical layer
-------------

The physical `PI/4 DQPSK` gnuradio receiver is inspired from [Tim's tetra-toolkit](https://github.com/Tim---/tetra-toolkit).
It works fine with RTL-SDR dongles at 2Mbps.
Results are much better than with HackRF which is more noisy.

Decoder
-------

The decoder role is to interpret and reconstruct TETRA packets and transmit it in Json format
for recording and analysis. Only a few fields are transmitted for now, but using Json, it can
be extensed very easily.
It implements partially the downlink `MAC`, `LLC`, `MLE`, `CMCE` and `UPLANE` layers.

Recorder
--------

The recorder maintain a store of associated `caller id`, suscriber identities `ssi` and `usage marker` by interpreting Json text received. It also handles the `D-RELEASE` to remove a given `caller id` from list.

The speech `.out` files are stored in the `recorder/out` folder and can be processed with TETRA codec to recover speech. The script is provided in `recoder/wav` folder to convert all `.out` files to `.wav`

```sh
$ cd recorder/wav/
$ ./out2wav.sh
```

Build
=====

    Note: don't forget to run `make clean` and rebuild the decoder and recorder when the repositery is updated


You will need:
* gnuradio v3.7.14 and gnuradio-companion with rtl-sdr (works also with GnuRadio 3.7.11)
* gcc
* rapidjson v1.1.0 (packages available in Ubuntu, Debian/Devuan and Slackware from SlackBuild.org)
* zlib v1.2.11 (other versions may work)
* sox for audio processing
* ncurses (optional interface for the recorder. If you don't want it, set `#undef WITH_NCURSES` in file `recorder/window.h`)

Build decoder
```sh
$ cd decoder
$ make clean
$ make
```

Build recorder with ncurses
```sh
$ cd recorder
$ make clean
$ make
```

Build recorder

```sh
$ cd recorder
$ make clean
$ make
```

Build speech codec (source code from ETSI example) [OPTIONAL see below]
```sh
$ cd codec
$ make clean
$ make
$ cp cdecoder ../recorder/wav/
$ cp sdecoder ../recorder/wav/
```
NEW: Internal speech codec is now available in `recorder` and `.raw` output files may
be generated directly in `recorder/raw/` folder when using `-a` flag (experimental).
Then use the script `raw2wav.sh` in folder `recorder/raw/` to generate `.wav` files.


Physical layer
```sh
$ cd phy
$ gnuradio-companion pi4dqpsk_rx.grc
```

Usage
=====

Open 3 shells in the 3 folders:

* In recorder/ run `./recorder`

```sh
Usage: recorder [OPTIONS]

Options:
  -a use raw output format with internal codec (experimental)
  -r <UDP socket> receiving Json data from decoder [default port is 42100]
  -i <file> replay data from Json text file instead of UDP
  -o <file> to record Json data in different text file [default file name is 'log.txt'] (can be replayed with -i option)
  -l <ncurses line length> maximum characters printed on a report line
  -n <maximum lines in ssi window> ssi window will wrap when max. lines are printed
  -h print this help
```

* In decoder/ run `./decoder`

```sh
Usage: decoder [OPTIONS]

Options:
  -r <UDP socket> receiving from phy [default port is 42000]
  -t <UDP socket> sending Json data [default port is 42100]
  -i <file> replay data from binary file instead of UDP
  -o <file> record data to binary file (can be replayed with -i option)
  -d <level> print debug information
  -f keep fill bits
  -h print this help
```

* In phy/ run your flowgraph from gnuradio-companion and tunes the frequency (and eventually the baseband offset which may be positive or negative)

Then you should see frames in `decoder`.
You will see less data in `recorder` but it maintains all received frames into the file `log.txt`.
Notice that this file may become big since it is never overwritten between sessions.

# Submitting bugs

When you find a bug, it is very important to record the incoming bits so I can check what's going on.
This is done with `./decoder -o out.bits` command.
You can zip and attach the `out.bits` file to the issue, it is very useful.

Typical debug session is:
1. you have some bits from `./decoder -o out.bits`
2. in folder recorder remove file `log.txt`
3. start `./recorder`
4. in folder decoder, replay your bits file with `./decoder -i out.bits`
5. go back to recorder folder and read the `log.txt` file

Note that the `out.bits` file can be read by sq5bpf program `tetra-rx out.bits`.

# Previous work

This kit has been done thanks to the work of:
* [osmo-tetra](https://git.osmocom.org/osmo-tetra/)
* [sq5bpf osmo-tetra](https://github.com/sq5bpf/osmo-tetra-sq5bpf)
* [sq5bpf telive](https://github.com/sq5bpf/telive)
* [brmlab.cz](https://brmlab.cz/project/sdr/tetra)
* [tetra-listener](https://jenda.hrach.eu/gitweb/?p=tetra-listener;a=summary)

Viterbi codec
* [Min Xu Viterbi codec](https://github.com/xukmin/viterbi)

Base64 decoder
* [Joe DF base64.c](https://github.com/joedf/base64.c)

FEC correction [NEW]
* Lollo Gollo (see issue #21)

# To be done

* LLC reassembly of segmented TL-SDU
* SDS tries to decode unknown protocols with 8 bits alphabets
* UDP packet size is limited to 2048 bytes, may be small for all Json text informations
