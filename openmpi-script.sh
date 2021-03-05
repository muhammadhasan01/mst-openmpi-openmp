if [ "$#" -ne 1 ]; then
    echo 'Usage: ./openmpi-script.sh <input_file>'
    exit
fi

input=$1
filename=`basename $1`

cat $1 | mpirun -np 4 ./mpi_program > output_mpi/tmp
sed '$d' output_mpi/tmp > output_mpi/$filename
rm -f output_mpi/tmp
