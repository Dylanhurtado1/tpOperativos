# TP Sistemas Operativos

Los lineamientos e implementacion del TP esta basado en el enunciado [KISS](https://docs.google.com/document/d/17WP76Vsi6ZrYlpYT8xOPXzLf42rQgtyKsOdVkyL5Jj0/edit)

Y los scripts de pruebas estan en [KISS - Tests](https://docs.google.com/document/d/1SBBTCweMCiBg6TPTt7zxdinRh4ealRasbu0bVlkty5o/edit)

Caracteristicas principales de los modulos desarrollados:
* **Consola**: Representa los procesos que se van a planificar. Cada consola tiene su propio conjunto de instrucciones a ejecutar y un tamaño definido por parametro
* **Kernel**: Planificador de 7 estados que implementa los algoritmos FIFO y SRT con estimación
* **CPU**: Modulo que ejecuta las instrucciones de un proceso. Cuenta con una TLB para agilizar las traducciones de DL a DF
* **Memoria**: Utiliza paginación jerarquica de 2 niveles y paginación bajo demanda. Ademas usa archivos swap para manejar un mayor nivel de direccionamiento

---

## Herramientas utilizadas
* Lenguaje C
* Git
* Sistema Operativo: Linux (VM provista por la catedra)
* IDE Eclipse

---

## Instalación

1) Ir al home del usuario: `cd $HOME`
2) Clonar este repositorio: `git clone <repositorio.git>`
3) Pararse en el repo: `cd <nombre repositorio>`
4) Ejecutar el script de deploy:
```
 ./deploy.sh install  <-- instala las commons, crea el directorio swap y clona los scripts de prueba
 ./deploy.sh uninstall  <-- desinstala e elimina todo lo anterior
```

---

## Ejecución

Para ejecutar los modulos se puede realizar usando Valgrind o no. En ambos casos se debe tener en cuenta que cada prueba tiene su configuración correspondiente, por ejemplo la prueba de planificación se puede ejecutar con algoritmo FIFO o SRT.

*Nota: todas las config's estan cargadas en el proyecto, solo se necesita pasar el path de la config al modulo*

Otra cosa a considerar, es que cada modulo es independiente por lo que cada uno debe ejecutarse en una terminal (ejecución local) o máquina (configurar IP's) diferente.

#### Ejemplo ejecución "normal" de planificación

```
# terminal 1
make consola PATH_I=/home/utnso/kiss-pruebas/PLANI_1 TAMANIO=128
# terminal 2
make consola PATH_I=/home/utnso/kiss-pruebas/PLANI_1 TAMANIO=128
# terminal 3
make consola PATH_I=/home/utnso/kiss-pruebas/PLANI_2 TAMANIO=128

# terminal 4
make memoria CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria_plani.config

# terminal 5
make cpu CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu_plani.config

# terminal 6
make kernel CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel_plani_fifo.config
o
make kernel CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel_plani_srt.config
```

#### Ejemplo ejecución con Valgrind de planificación

```
# terminal 1
make memcheck PROCESO=consola PATH_I=/home/utnso/kiss-pruebas/PLANI_1 TAMANIO=128
# terminal 2
make memcheck PROCESO=consola PATH_I=/home/utnso/kiss-pruebas/PLANI_1 TAMANIO=128
# terminal 3
make memcheck PROCESO=consola PATH_I=/home/utnso/kiss-pruebas/PLANI_2 TAMANIO=128

# terminal 4
make memcheck PROCESO=memoria CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria_plani.config

# terminal 5
make memcheck PROCESO=cpu CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu_plani.config

# terminal 6
make memcheck PROCESO=kernel CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel_plani_fifo.config
o
make memcheck PROCESO=kernel CONFIG=/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel_plani_srt.config
```

Cada modulo que se ejecuta con Valgrind arroja sus logs en archivos .log, ejemplo `memcheck-kernel.log`
