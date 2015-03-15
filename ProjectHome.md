# Diseño y desarrollo de una cabina casera para el Flight Simulator 2004. #

## Introducción ##
El proyecto consiste en el diseño y fabricación de una cabina casera, usada en simulación.
El diseño se dirigirá a simular la cabina de un avión ligero, en particular el Cessna 182S usado para vuelos VFR.
Usa como motor de simulación el programa "Microsoft Flight Simulator 2004" el cual ha sido elegido por sus bajas exigencias de recursos del ordenador en el que será instalado.

El diseño también está orientado para el uso en aviación virtual, por lo que será equipado con los programas necesarios para su conexión a redes de voz y datos, de aviación virtual y en particular con los programas base de la aerolínea virtual "AirHispania".


## Características ##
  * Simulación del Cessna 182S.
  * Multi-monitor.
  * Bus de comunicación propietario.
  * Instrumentos analógicos basados en servos.
  * Uso de tarjeta ioCard Master para controlar los interruptores y luces del panel de intrumentos.
  * Mueble de madera para el panel .



## Elementos del sistema ##
Las partes del sistema son:
  * FS9   Software de simulación en PC.
  * FSCOM  Aplicación para leer y escribir  variables usando FSUIPC, y comunicarlas mediante USB con la placa BusCard.
  * BusCard  Placa electrónica cuya funcionalidad es la comunicar la cabina casera con el PC y controlar el panel de instrumentos usando la ioCard Master y un bus RS485 para instrumentos inteligentes como el COMM.
  * ioCard Master  Placa iocard para manejar entradas y salidas
  * Cabina  Formada panel de instrumentos y la estructura de soporte de todo el simulador.

Página principal [Cabsim](http://code.google.com/p/cabsim/wiki/cabsim)

## BusCard ##
<a href='Hidden comment: 
Comprada en eBay.
Vendedor: [http://myworld.ebay.es/cstech_co_uk/ cstech.co.uk].
Precio: 16.75€.
'></a>

![http://cabsim.googlecode.com/files/13a7_35.jpg](http://cabsim.googlecode.com/files/13a7_35.jpg)

PIC40 - Prototyping Kit for any of the 40 pin Microchip PIC microcontrollers

Para mas informacion pulsar [este enlace](http://www.cstech.plus.com/ebay/pic40.html).

Utiliza el microcontrolador [PIC18F4550](http://ww1.microchip.com/downloads/en/DeviceDoc/39632e.pdf)
