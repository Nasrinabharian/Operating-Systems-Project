sudo rmmod ebbchar.ko
make
sudo insmod ebbchar.ko
sudo mknod ebbchar c 234 1
gcc test.c -o test
sudo ./test
sudo rmmod ebbchar.ko
rm test
make clean

