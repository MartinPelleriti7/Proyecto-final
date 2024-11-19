#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines y variables
#define BOTON1 8  // Pin del primer botón
#define BOTON2 9  // Pin del segundo botón
#define BOTON3 10 // Pin del tercer botón
bool BOTON1STATE = false;  // Estado del primer botón (presionado o no)
bool BOTON2STATE = false;  // Estado del segundo botón
bool BOTON3STATE = false;  // Estado del tercer botón

#define BOMBA 7  // Pin que controla la bomba

// Configuración del LCD (dirección 0x27, 16 columnas, 2 filas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines para el sensor ultrasónico
const int Trigger = 2;  // Pin de disparo del sensor
const int Echo = 3;     // Pin de recepción del sensor

bool esperandoTamano = false;  // Estado que controla si se está eligiendo el tamaño
bool sistemaActivo = false;    // Estado que indica si el sistema está activo

void setup() {
  // Configurar pines de los botones como entradas
  pinMode(BOTON1, INPUT);
  pinMode(BOTON2, INPUT);
  pinMode(BOTON3, INPUT);

  // Configurar el pin de la bomba como salida
  pinMode(BOMBA, OUTPUT);

  // Configuración del sensor ultrasónico (Trigger como salida, Echo como entrada)
  Serial.begin(9600);    // Inicializa la comunicación serial para depuración
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);  // Asegura que el Trigger esté en bajo al inicio

  // Configuración del LCD
  lcd.init();       // Inicializa el LCD
  lcd.backlight();  // Enciende la retroiluminación del LCD
  lcd.print("Sistema iniciado");  // Muestra mensaje de inicio
  delay(1000);      // Espera un segundo
  lcd.clear();      // Limpia la pantalla del LCD
}

void loop() {
  // Leer el estado de los botones (LOW indica que el botón está presionado)
  BOTON1STATE = digitalRead(BOTON1) == LOW; 
  BOTON2STATE = digitalRead(BOTON2) == LOW;
  BOTON3STATE = digitalRead(BOTON3) == LOW;

  // Leer la distancia del sensor ultrasónico
  long t;  // Variable para almacenar el tiempo del pulso
  long d;  // Variable para almacenar la distancia medida

  digitalWrite(Trigger, HIGH);  // Activa el Trigger del sensor
  delayMicroseconds(10);        // Espera 10 microsegundos
  digitalWrite(Trigger, LOW);   // Desactiva el Trigger

  // Mide el tiempo del pulso de retorno en el pin Echo
  t = pulseIn(Echo, HIGH);
  d = t / 58;  // Conversión del tiempo a distancia en centímetros

  // Lógica principal para el comportamiento del sistema
  if (d <= 5 && !sistemaActivo) {  // Si la distancia es menor o igual a 5 cm y el sistema no está activo
    mostrarBienvenida();           // Muestra la pantalla de bienvenida
  }

  if (sistemaActivo && !esperandoTamano && BOTON1STATE) {  // Si el sistema está activo y se presiona el primer botón
    elegirTamano();  // Cambia a la pantalla para elegir el tamaño de la bebida
  }

  if (esperandoTamano) {  // Si estamos en la pantalla de selección de tamaño
    seleccionarTamano();  // Llama a la función para seleccionar el tamaño de la bebida
  }
}

void mostrarBienvenida() {
  lcd.setCursor(0, 0);  // Establece el cursor en la primera fila, primera columna
  lcd.print("BIENVENIDO");  // Muestra mensaje de bienvenida
  lcd.setCursor(0, 1);      // Establece el cursor en la segunda fila
  lcd.print("Presione BOTON 1");  // Instrucción para presionar el botón 1
  sistemaActivo = true;  // Activa el sistema
}

void elegirTamano() {
  lcd.clear();  // Limpia la pantalla LCD
  lcd.setCursor(0, 0);  // Establece el cursor en la primera fila
  lcd.print("Elija TamaNo:");  // Muestra mensaje para elegir tamaño
  lcd.setCursor(0, 1);  // Establece el cursor en la segunda fila
  lcd.print("2-Grande 3-Chico");  // Opciones para elegir el tamaño
  esperandoTamano = true;  // Cambia al estado de espera para seleccionar tamaño
}

void seleccionarTamano() {
  if (BOTON2STATE) {  // Si se presiona el botón 2 (Bebida Grande)
    lcd.clear();  // Limpia la pantalla LCD
    lcd.setCursor(0, 0);  // Establece el cursor en la primera fila
    lcd.print("Bebida Grande");  // Muestra que se ha seleccionado bebida grande
    delay(2000);  // Espera 2 segundos
    digitalWrite(BOMBA, HIGH);  // Activa la bomba para dispensar la bebida
    delay(10000);  // La bomba se mantiene activa durante 10 segundos (para bebida grande)
    reiniciarSistema();  // Reinicia el sistema después de dispensar la bebida
  } else if (BOTON3STATE) {  // Si se presiona el botón 3 (Bebida Pequeña)
    lcd.clear();  // Limpia la pantalla LCD
    lcd.setCursor(0, 0);  // Establece el cursor en la primera fila
    lcd.print("Bebida PequeNa");  // Muestra que se ha seleccionado bebida pequeña
    delay(2000);  // Espera 2 segundos
    digitalWrite(BOMBA, HIGH);  // Activa la bomba para dispensar la bebida
    delay(5000);  // La bomba se mantiene activa durante 5 segundos (para bebida pequeña)
    reiniciarSistema();  // Reinicia el sistema después de dispensar la bebida
  }
}

void reiniciarSistema() {
  lcd.clear();  // Limpia la pantalla LCD
  sistemaActivo = false;  // Desactiva el sistema
  esperandoTamano = false;  // Resetea el estado de espera de tamaño
  digitalWrite(BOMBA, LOW);  // Apaga la bomba
}
