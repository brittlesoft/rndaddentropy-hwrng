# rndaddentropy-hwrng
> Simple tool to fill linux's entropy pool with data from a hardware random number generator (`/dev/hwrng`) on boot.

It was written as simple and single purpose alternative to
[rng-tools](https://github.com/nhorman/rng-tools) in order to speed up the first boot of a Raspberry Pi
Zero which was blocking for a few minutes on the ssh host key generation.

# Compiling

```
make
```

# Running

As root:
```
./rndaddentropy-hwrng
```

The program exits with 0 once the available entropy count reaches 2048 bits.
(Tunable via the `TARGET_EC` environment variable.)
If anything goes wrong, it exits with 1 and an appropriate message.
