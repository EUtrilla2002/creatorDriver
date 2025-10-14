# creatorDriver
Upgrade of CREATOR's actual driver with
- GDBGUI interface
- ECALL module


## Development
This project uses [uv](https://docs.astral.sh/uv) to manage dependencies.

To add a new dependency, use `uv add <dep>`.

To generate the `requirements.txt` file:
```
uv export --format=requirements-txt --no-hashes
```
