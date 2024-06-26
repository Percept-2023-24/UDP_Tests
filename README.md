# UDP_Tests

This repo contains scripts to test UDP communication between a single server and client, with simple strings, txt files, and JSON files.

### Installation
Before using the scripts in this repository, the RapidJSON C++ library must be installed in order to interact with JSON data.
1. Download the latest RapidJSON release [here](https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.tar.gz)
2. Unzip the downloaded `rapidjson-1.1.0.tar.gz`
  * `tar -xvf rapidjson-1.1.0.tar.gz`
3. Copy the `include/rapidjson/` directory to the `/usr/include` directory
  * `cd rapidjson-1.1.0/`
  * `sudo cp -r include/rapidjson/ /usr/include/`

Now, clone this repository to any location to begin using it.

### Miscellaneous
If encountering `bind failed. Error: Address already in use`, use `netstat -tulpn` to view running processes and `kill -9 <pid>` to kill the processes occupying the desired address.
