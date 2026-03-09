# War

Educationnal project about the creatition of a self-duplicating obfuscated malware over ELF.

## Subject requierements

- Doesn't execute when a specific process (`doom-nukem`) is running.
- Doesn't execute when watched by a debugger
- Infection routine must be ofuscated

## Roadmap

- [x] Check for a `doom-nukem` process on the machine
- [x] Check for a debugger overhead
- [x] Find the place to inject malecious code inside the binary
- [x] Inject the code and modify the binary
