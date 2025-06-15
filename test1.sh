echo hello
echo world
!!

echo hi
echo hello

sleep 100 &
sleep 200 &
sleep 300 &
jobs
fg %2
jobs
bg %2
jobs
fg %4
jobs
exit 255