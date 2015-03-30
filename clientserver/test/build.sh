reset

make 1> /dev/null
if [ $? != 0 ]
then
	exit
fi

rm database -r >& /dev/null

./disk_server $1 $2
