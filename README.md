# s98play

A fork of [s98play](https://amethyst.yui.ne.jp/svn/nekobus/s98play/) that adds PC-9801-26K / YM2203 hardware support, and other stuff.

See `readme.txt` for a readme file that's closer to what upstream ships.

---

##### Why?

Because my machine only has a built-in YM2203, but I'd still like to play YM2203 S98 files on it. And some other QoL stuff.

##### How well-tested is this?

Not very, only in NP2kai. My machine still needs some fixing before I'm willing to turn it on again. Use at your own risk.

##### Any other features?

Non-exhaustive list, cus I might not update this every time.

- Prints song title (S98v1) or entire metadata information (S98v3) when available. May look borked if UTF-8 data is present.

##### Binaries?

Build it yourself, it's safer anyway. Only needs [NASM](https://www.nasm.us/). `build.bat` has the command you need to run. May need to adjust casing depending on your platform.
