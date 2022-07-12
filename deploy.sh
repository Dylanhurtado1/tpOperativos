#!/bin/bash

PROJECT_PATH=/home/utnso/tp-2022-1c-lo-importante-es-aprobar
TESTING_PATH=/home/utnso/kiss-pruebas
SWAP_PATH=/home/utnso/swap

if [ "$1" == "" ]; then
	echo "Error de ejecucion: ./deploy.sh <proceso> [path_instrucciones] [tam_memoria]"
	exit(1)
fi

proceso=$1

echo -e "\n\nInstalling commons libraries...\n\n"

cd $HOME

COMMONS="so-commons-library"

rm -rf $COMMONS
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
sudo make install


echo -e "\n\nInstalling commons libraries... DONE\n\n"

echo -e "\n\n **** Descargando Pruebas... ****\n\n"
cd $HOME
git clone "https://github.com/sisoputnfrba/kiss-pruebas"

echo -e "\n\n **** Descargando Pruebas... DONE ****\n\n"


echo -e "\n\n ***** Ejecutando modulo: $proceso *****\n"

mkdir -p $SWAP_PATH
cd $PROJECT_PATH

do
    case $proceso in
        "consola")
		PATH_I=${TESTING_PATH}/$2
		TAMANIO=$3
		make consola $PATH_I $TAMANIO
        	;;
	"kernel")
		make kernel
        	;;
        "cpu")
		make cpu
        	;;
	"memoria")
		make memoria
        *)
		echo "Proceso $proceso invalido."
		exit(1)
        	;;
    esac
done

