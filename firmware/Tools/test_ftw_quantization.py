"""Verify fixed AD9910 FTW quantization without frequency dithering."""


SYSCLK_HZ = 1_000_000_000
FTW_SCALE = 2**32


def main():
    lsb_hz = SYSCLK_HZ / FTW_SCALE
    print(f"AD9910_FTW_LSB={lsb_hz:.12f}Hz")
    for target_hz in (1_000, 10_000, 100_000, 200_000):
        ftw = round(target_hz * FTW_SCALE / SYSCLK_HZ)
        output_hz = ftw * SYSCLK_HZ / FTW_SCALE
        error_hz = output_hz - target_hz
        assert abs(error_hz) <= lsb_hz / 2
        print(
            f"target={target_hz:9.3f}Hz ftw={ftw:7d} "
            f"fixed_output={output_hz:13.9f}Hz "
            f"error={error_hz:+.9f}Hz"
        )
    print("RESULT=PASS")


if __name__ == "__main__":
    main()
