# Fuzzing GLib and libsoup with OSS-Fuzz

## Project Overview
This project employs OSS-Fuzz to perform fuzz testing on Gnome's GLib and libsoup and libraries. To be able to leverage OSS-Fuzz's infrastructure while allowing for local changes to the projects harnesses, dictionaries and seeds, we work with local, modified repositories of these libraries. This approach enables us to implement custom harnesses, dictionaries, and seeds tailored to our testing needs.
  
For the testing campaign, five dedicated harnesses were developed to target specific components in the libraries:

- The regex engine in GLib, fuzz_regex.c
- The scanner in GLib, fuzz_scanner_parsing.c
- The variant binary byteswap functionality in GLib, fuzz_variant__binary_byteswap2.c
- The HTTP header parser in libsoup, fuzz_http_headers_parse_request2.c
- The HTTP/2 parser in libsoup, fuzz_http2.c
  
A total of 4 bugs were identified during the testing campaign. While these issues had already been reported by security researchers and their reports are publicly available, our testing campaign independently rediscovered them without prior knowledge. As of now, patches and backports are actively being pushed or developed to address these vulnerabilities.

## Installation
### Prerequisites
Before setting up the project, ensure your system meets the following requirements:

- A **Linux-based environment** (e.g., Ubuntu, Debian).
- **Docker** for containerization. (Note that OSS-Fuzz does not currently support Podman. See [this issue](https://github.com/google/oss-fuzz/issues/13966) for details.)
- **Python 3** (version 3.8 or higher)


### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/ivan-rm/tfg-fuzzing-glib-libsoup.git
   ```
2. Navigate to the project directory:
   ```bash
   cd tfg-fuzzing-glib-libsoup
   ```
3. Run the setup script:
   ```bash
   ./setup.sh
   ```
## Workflow
For detailed instructions on how to work with the environment and all the available commands, see [WORKFLOW.md](https://github.com/ivan-rm/tfg-fuzzing-glib-libsoup/blob/main/WORKFLOW.md).
