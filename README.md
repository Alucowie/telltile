Small syscall wrapper inspired by freedreno but for PowerVR SGX.

This tool is in an early stage.

# build

```
mkdir build
cd build
meson setup ..
ninja
```

# run

`# LD_PRELOAD=/root/telltile.git/build-arm/wrap/libwrap.so ./test-es2gears > test-es2gears.log`
