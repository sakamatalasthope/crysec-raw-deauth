#crysecdeauth
A simple 802.11 deauthentication tool written in C using raw sockets. i wrote this for some local "box" testing to see how different devices handle L2 frame injection.

features
L2 raw injection: uses AF_PACKET to bypass the networking stack.

Radiotap support: compatible with standard linux monitor mode drivers.

Broadcast/Unicast: target a specific station or the whole BSSID.

how to run
deps: you only need a standard C compiler and linux headers.

compile: gcc crysecdeauth.c -o crysecdeauth

usage: sudo ./crysecdeauth <client_mac> <ap_bssid> <iface>

setup
your interface must be in monitor mode and locked to the same channel as the AP:

Bash
sudo ip link set wlp0s20f0u9 down
sudo iw wlp0s20f0u9 set monitor control
sudo ip link set wlp0s20f0u9 up
sudo iw dev wlp0s20f0u9 set channel 5
example
Bash
sudo ./crysecdeauth CE:D9:FB:4B:C7:50 E4:FA:C4:27:DF:5A wlp0s20f0u9
todo
fix potential memory leaks in packet buffer

add automatic channel switching via ioctl

add a more aggressive flood mode (variable usleep)

support for 802.11w (PMF) detection
