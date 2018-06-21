# Lutz One Pass
This repository contains code I've written implementing the Lutz One Pass
algorithm.

## Downloading the code
Clone the repository to your local machine:
```
git clone https://github.com/Jvinniec/lutz_one_pass.git lutz_one_pass
```

## Build the code
The code uses cmake to build the `lutzop` library. 
1. Move into the cloned directory
```
cd lutz_one_pass
```
2. Build the code
```
cmake [-Dprefix=/desired/installation/path/]
make
```
3. (Optional) Install the code if desired
```
make install
```

## Credit
This algorithm is derived from the following paper:
* Title: An Algorithm for the Real Time Analysis of Digitised Images
* Author: R. K. Lutz
* Journal: Comput J (1980) 23 (3): 262-269
