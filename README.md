by Lingzhe Cai (chestercai1995@gmail.com)

This repo is for a distrubuted shared memory library between devices using the tcpip server.

Each devices would keep a local copy that is always up to date. 
Therefore, each write would write to both devices. But each read would only happen locally.

