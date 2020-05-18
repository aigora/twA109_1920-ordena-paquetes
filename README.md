# Título del trabajo

Compra/venta de un inventario y robot que ordena paquetes.

## Integrantes del equipo

Hugo, Lopez Janquin. Número de matrícula: 54703. Usuario GitHub: hugolopezjanquin.

Zeqi, Lin. Número de matrícula: 54694. Usuario GitHub: zeqilin.

## Objetivos del trabajo

Utilizar conocimientos aprendidos en clase como es aprender a programar en lenguaje C y usar el hardware Arduino para desarrollar el robot.
Creación de un programa en lenguaje C de gestión de inventario, que peuede ordenar a un robot a ordenar o añadir paquetes a los diferentes pasillos de un almacén. Además de otro programa en C para la compra de los productos almacenados. 
El software del robot sera Arduino, a este robot se le dara las ordenes de mover los paquetes.

## Sensores y actuadores
- Arduino(arduino uno rev 3)

- Servomotor (micro servo 9G SG90)

- Motor de corriente continua con reductora

- Sensor infrarrojo (módulo TCRT5000)

- Puente H (Módulo controlador de motores L298N)
## Directorios
- podigos_prueba. son diversos codigos de prueba para comprobar como funcionan ciertos componentes en arduino o como se ejecuta un porgrama en C.
- PruebaPuertoSerie. codigos para probar como se pueden mandar y recibir datos con la conexión puerto serie entre arduino y C. Donde el programa C se encarga mandar datos para que arduino pueda mover el robot, mientras C no sigue ejecutando el codigo hasta que arduino le diga que ha acabado.
- diseño_3. son stls de para imprimirlas en 3D y con ellas construir el chasis del robot.
