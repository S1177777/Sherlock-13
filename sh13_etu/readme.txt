lib：
sudo apt update
sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev



wsl：
cd /mnt/d/EIS8/OSuser/Sherlock13/sh13_etu
gcc -o sh13 sh13.c -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread
gcc -o server server.c


server：

./server 12345

./sh13 127.0.0.1 12345 127.0.0.1 20001 Player1


