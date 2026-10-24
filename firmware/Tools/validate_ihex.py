"""Validate Intel HEX checksums and report programmed address coverage."""

import sys
from pathlib import Path


def main():
    path = Path(sys.argv[1]).resolve()
    upper_address = 0
    memory = {}
    records = 0
    eof_seen = False

    for line_number, raw_line in enumerate(
        path.read_text(encoding="ascii").splitlines(),
        1,
    ):
        line = raw_line.strip()
        if not line:
            continue
        if not line.startswith(":"):
            raise ValueError("line %d: missing ':'" % line_number)

        record = bytes.fromhex(line[1:])
        if len(record) < 5:
            raise ValueError("line %d: record is too short" % line_number)
        if (sum(record) & 0xFF) != 0:
            raise ValueError("line %d: checksum failed" % line_number)

        byte_count = record[0]
        address = (record[1] << 8) | record[2]
        record_type = record[3]
        data = record[4:-1]
        if byte_count != len(data):
            raise ValueError("line %d: byte count mismatch" % line_number)
        records += 1

        if record_type == 0x00:
            base = upper_address + address
            for offset, value in enumerate(data):
                absolute = base + offset
                if absolute in memory and memory[absolute] != value:
                    raise ValueError(
                        "line %d: conflicting data at 0x%08X"
                        % (line_number, absolute)
                    )
                memory[absolute] = value
        elif record_type == 0x01:
            eof_seen = True
        elif record_type == 0x02:
            upper_address = int.from_bytes(data, "big") << 4
        elif record_type == 0x04:
            upper_address = int.from_bytes(data, "big") << 16
        elif record_type in (0x03, 0x05):
            pass
        else:
            raise ValueError(
                "line %d: unsupported record type 0x%02X"
                % (line_number, record_type)
            )

    if not eof_seen:
        raise ValueError("EOF record is missing")
    if not memory:
        raise ValueError("HEX contains no data")

    minimum = min(memory)
    maximum = max(memory)
    print("HEX=%s" % path)
    print("RECORDS=%d" % records)
    print("DATA_BYTES=%d" % len(memory))
    print("MIN_ADDRESS=0x%08X" % minimum)
    print("MAX_ADDRESS=0x%08X" % maximum)
    print("EOF=OK")
    print("CHECKSUMS=OK")


if __name__ == "__main__":
    main()
