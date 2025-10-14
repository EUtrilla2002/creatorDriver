# CREATOR ESP-32 Driver
By Elisa Utrilla Arroyo


## Installation and execution
1. Install [Python 3.9](https://www.python.org/downloads/release/python-3913/).
  - In Ubuntu:
    ```bash
    sudo apt install software-properties-common
    sudo add-apt-repository ppa:deadsnakes/ppa
    sudo apt install python3.9 python3.9-venv python3.9-distutils
    ```
  - With [uv](https://docs.astral.sh/uv):
    ```sh
    uv python install 3.9
    ```
3. Install the ESP-IDF framework, following
   [espressif's documentation](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32/get-started/linux-macos-setup.html)  
   To ensure Python 3.9 is used for the installation, first create a virtual environment in `~/.espressif/python_env/idf5.3_py3.9_en`, and activate it, before executing the `install.sh` script.
   ```bash
   python3.9 -m venv ~/.espressif/python_env/idf5.3_py3.9_en
   source ~/.espressif/python_env/idf5.3_py3.9_env/bin/activate
   ```
4. Install the requirements:
   ```
   pip3 install -r requirements.txt
   ```
5. [Load the ESP-IDF environment variables](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32/get-started/linux-macos-setup.html#step-4-set-up-the-environment-variables) (`export.sh`)
6. Execute the gateway web service:
   ```
   python3 gateway.py
   ```
