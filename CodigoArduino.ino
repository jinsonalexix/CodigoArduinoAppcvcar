
//Libreria para comunicaciones seriales
#include <SPI.h>
//Libreria para tarjetas SD
#include <SD.h>
//Libreria para cifrado de datos con md5

//Constante CS de conexion del modulo SD
const int chipSelect = 53;

//Variable para manejo de archivos
File myFile;

//Variable para estado de tarjeta
boolean tarjeta=false;

//setup de arduino
void setup(){
  
  
   //Establecemos el pin 39 como salida 1 de relay 
  pinMode(22, OUTPUT);
  digitalWrite(22,LOW);

  //Establecemos el pin 39 como salida 2 de relay 
  pinMode(23, OUTPUT);
  digitalWrite(23,LOW);

  //Led indicadores de atividad
  pinMode(30, OUTPUT);//Led actividad
  pinMode(31, OUTPUT);//Led bloqueo encendido
  pinMode(32, OUTPUT);//Led bloqueo seguros

  //Establecemos la velocidad de conexion serial
  Serial.begin(9600);
  //Establecemos el pin 53 como salida
  pinMode(chipSelect, OUTPUT);
  //Limpiamos el puerto serial de datos
  Serial.flush();
  //Esperamos 5 milisegundos
  delay(500);


  //Inicializacion de tarjeta sd
  tarjeta=SD.begin(chipSelect);
  if(!tarjeta){
    Serial.println("SD no inicializada");
    return;
    }
    Serial.println("SD inicializada");
}

//Buble de operacion arduino
void loop(){
  
  // Esperamos ha recibir datos a traves del puerto serie (Bluetooth)
  if (Serial.available()){
    //Leemos el comando de entrada
    String comando = Serial.readString();
    //Limpiamos el puerto serie
    Serial.flush();
  
   //Verificamos el estado de configuración del Arduino  ok
    if (comando.startsWith("893ce80729195afed54dd71286ef15bc")){
      //Verifica si hay datos de configuración guardados en la tarjeta SD
      String mac=comando.substring(32,comando.length());
      String res="";

      //Verificamos si existen los archivos conexion y mac
      if(SD.exists("conexion.txt")&& SD.exists("mac.txt")){
        String orden=leerArchivo(0,"conexion.txt");
        //si el acceso esta permitido verificamos la mac
        if(orden.equals("permitido")){
          if(verificarsoloMac(mac)){
            res="6e432137782303c9c10a7312f0b96670";
          }else{
            res="40f3d08a8addd234ec4cd24a6bb893b9";
          }
        }else{
            res="2d83e1c088a687a3891e653f098ae001";
        }
        indicadorLed();
        Serial.println(verificarEstado()+res);
        Serial.flush();  
      }else{
        Serial.println(verificarEstado()+"40f3d08a8addd234ec4cd24a6bb893b9");
        Serial.flush(); 
      } 
    }
   
   //Escribimos datos de usuario ok
    if(comando.startsWith("1df54ee1231697f4ac4bfee0ddf9ab7b")){ 
      indicadorLed();
      escribirUsuario(comando.substring(33,comando.length()));
      Serial.flush();
    }  
    //Escribimos datos del dispositivo móvil ok
    if(comando.startsWith("04255cbb81ee9f7f51d1ce038a3e65eb")){ 
      indicadorLed();
      escribirMac(comando.substring(32,comando.length()));
      eliminarArchivo("conexion.txt");
      modificarConexion("denegado");
      Serial.flush();
    }  
     
    //Login con clave de usuario ok
    if(comando.startsWith("c4ba244f086a4f71b53628bcc6a63eb0")){ 
      String mac=comando.substring(comando.length()-17,comando.length());
      String clave=comando.substring(33,65);
      String rol=verificarUser(clave);
      int resultMac=verificarMac(mac);
      //Verificacion de mac de dispositivo
      if(resultMac==1){
        if(rol.equals("nan")){
          Serial.println("6e3a4793fd8236cf495a4b6eec2061ea");
          Serial.flush();
        }else{
          if(rol.equals("admin")){
            Serial.println("21232f297a57a5a743894a0e4a801fc3");
            Serial.flush();
          }else{
            Serial.println("ee11cbb19052e40b07aac0ca060c23ee");
            Serial.flush();
          }
          indicadorLed();
        }
      }else{
        Serial.println("d119fc9a2d655536532684e377e130ba");
        Serial.flush();
      }
    }

    //Login facial ok
    if(comando.startsWith("531396d42462ca73fdfb7fb0f49b426b")){ 
      String mac=comando.substring(comando.length()-17,comando.length());
      int resultMac=verificarMac(mac);
      String imagen=comando.substring(33,comando.length()-18);
      String rol= verificarDatos(imagen);
      if(resultMac==1){
        if(rol.equals("denegado")){
          Serial.println("6e3a4793fd8236cf495a4b6eec2061ea");
          Serial.flush();
        }else if(rol.equals("no_existe")){
          Serial.println("7e88684b6f408898d7b54f7ca963cf05");
          Serial.flush();
        }else{
          if(rol.equals("admin")){
            Serial.println("21232f297a57a5a743894a0e4a801fc3");
            Serial.flush();
          }else{
            Serial.println("ee11cbb19052e40b07aac0ca060c23ee");
            Serial.flush();
          }
          indicadorLed();
        }
      }else{
        Serial.println("d119fc9a2d655536532684e377e130ba");
        Serial.flush();
      }
    }


    //Verificar estado de pines ok
    if(comando.equals("b5adab51c39c7f74ac4f8e4444d3e6a9")){
      //Desactivamos el bloqueo
      digitalWrite(22,HIGH);
      digitalWrite(31,HIGH);

      //Desactivamos los seguros
      digitalWrite(23,HIGH);
      digitalWrite(32,HIGH);
      
       int pin22=digitalRead(22);
       int pin23=digitalRead(23);
       int pin31=digitalRead(31);
       indicadorLed();
       Serial.println(String(pin22)+" "+String(pin23)+" "+String(pin31));
       Serial.flush();
      }
      
      //Ejecucion de orden pines encender y apagar ok
    if(comando.startsWith("ec0cd3cb91fe82b9501f62a528eb07a9")){ 
      String orden=comando.substring(32,comando.length());
      
      if(orden.equals("20a1c542e9586436967b61603500347d")){
        digitalWrite(22,HIGH);
        digitalWrite(31,HIGH);
        Serial.println("on");
        Serial.flush();
      }else if(orden.equals("64d2263a3ab0e15ed40a50fe90fbb8be")){
        digitalWrite(22,LOW);
        digitalWrite(31,LOW);
        Serial.println("off");
        Serial.flush();
      } 
      
      if(orden.equals("3eca20a8ad8b8ef7b2b2ada28c800845")){
        digitalWrite(23,HIGH);
        digitalWrite(32,HIGH);
        Serial.println("on");
        Serial.flush();
      }else if(orden.equals("48982eef4aaccae9ab9c08ce38aadbcb")){
        digitalWrite(23,LOW);
        digitalWrite(32,LOW);
        Serial.println("off");
        Serial.flush();
      } 
    } 

//////////////////////////////////////////////////////////////////////////////////
    //Comando para obtener todos los usuarios ok
    if(comando.equals("2804940b006ba92bf554fe979132fd60")){
      String users="";
      String fila="";
      int limite=contadorFilas("config.txt");
      for(int i=1;i<limite;i++){
        fila=leerArchivo(i,"config.txt");
        fila.replace('\t','%');
        users=users+fila+"#";
      }
      indicadorLed();
      Serial.println(users);
      Serial.flush();  
    }

  //Comando para modificar usuario ok
    if(comando.startsWith("ca79021d11e8ef9d4401645a62f7748d")){
      String filaM=comando.substring(33,comando.length());
      String idM=filaM.substring(0,2);
      String idf="";
      String users="";
      String fila="";
      int limite=contadorFilas("config.txt");
      for(int i=1;i<limite;i++){
        fila=leerArchivo(i,"config.txt");
        idf=fila.substring(0,2);
        if(idf.equals(idM)){
          fila=filaM;
        }
        if(i==1){
          users=users+fila;
        }else{
          users=users+"\n"+fila;
        }
        if(i==(limite-1)){
          users=users;
          }
        
      }
      String nuevo="ID\tACCESO\tIMAGEN\tCEDULA\tNOMBRES\tAPELLIDOS\tSEXO\tROL\tCLAVE\n"
      +users.substring(0,users.length()-1);
      eliminarArchivo("config.txt");
      escribirUsuario(nuevo);
      indicadorLed();
      Serial.flush();  
    }
    //Comando para eliminar usuario ok
    if(comando.startsWith("4c565c62c2a30349cd330c673296f218")){
     String filaM=comando.substring(33,comando.length());
      String idf="";
      String users="";
      String fila="";
      String filas="";
      int limite=contadorFilas("config.txt");
      int i=1;
      for(i=1;i<limite;i++){
        fila=leerArchivo(i,"config.txt");
        if(!fila.startsWith(filaM)){
          if(i==1){
            users=users+fila;
            }else{
              users=users+"\n"+fila;
            }
        }else{
          users=users;
        }  

        if(i==(limite-1)){
          users=users;
          }
      }
      String nuevo="ID\tACCESO\tIMAGEN\tCEDULA\tNOMBRES\tAPELLIDOS\tSEXO\tROL\tCLAVE\n"+users;
      eliminarArchivo("config.txt");
      escribirUsuario(nuevo);
      indicadorLed();
      Serial.flush();  
    }

     ////////////////////////////////////////////////////////////////////////////
  //Comandos para obtener dispositivos ok
  if(comando.equals("1afde2ffe9aef4726eec064a0d0c9b17")){
      String disp="";
      String fila="";
      int limite=contadorFilas("mac.txt");
      for(int i=1;i<limite;i++){
        fila=leerArchivo(i,"mac.txt");
        fila.replace('\t','%');
        disp=disp+fila+"#";
      }
      Serial.println(disp);
      indicadorLed();
      Serial.flush();  
    }
     //Comando para eliminar dispositivos ok
    if(comando.startsWith("8fec071f2b1924bc03cfc31bacacb049")){
     String filaM=comando.substring(33,comando.length());
      String idf="";
      String dispo="";
      String fila="";
      String filas="";
      int limite=contadorFilas("mac.txt");
      int i=1;
      for(i=1;i<limite;i++){
        fila=leerArchivo(i,"mac.txt");
        if(!fila.startsWith(filaM)){
          if(i==1){
            dispo=dispo+fila;
            }else{
              dispo=dispo+"\n"+fila;
            }
          }else{
            dispo=dispo;
        }  
        if(i==(limite-1)){
          dispo=dispo;
          }
      }
      String nuevo="ID\tACCESO\tNOMBRE\tMAC\n"+dispo;
      eliminarArchivo("mac.txt");
      escribirMac(nuevo);
      indicadorLed();
      Serial.flush();  
    } 

    //Comando para modificar acceso ok
    if(comando.startsWith("d2ba57909dfd2c080468eaad2463980a")){
      String filaM=comando.substring(33,comando.length());
      String idM=filaM.substring(0,2);
      String idf="";
      String dispo="";
      String fila="";
      int limite=contadorFilas("mac.txt");
      for(int i=1;i<limite;i++){
        fila=leerArchivo(i,"mac.txt");
        idf=fila.substring(0,2);
        if(idf.equals(idM)){
          fila=filaM;
        }
        if(i==1){
          dispo=dispo+fila;
        }else{
          dispo=dispo+"\n"+fila;
        }
        if(i==(limite-1)){
          dispo=dispo;
          }
      }
      String nuevo="ID\tACCESO\tNOMBRE\tMAC\n"+dispo;
      eliminarArchivo("mac.txt");
      escribirMac(nuevo);
      indicadorLed();
      Serial.flush();  
    }

    //Comando para reeestablecer arduino ok
    if(comando.equals("4eb94307e81956d5dea125cdfa07c359")){
      eliminarArchivo("config.txt");
      eliminarArchivo("mac.txt");
      eliminarArchivo("conexion.txt");
      Serial.println("ok");
      indicadorLed();
      Serial.flush();  
    }

    //Comando para permitir conexiones ok
    if(comando.startsWith("7074763d8b382526a1963a62cdc4509b")){
      String orden=comando.substring(32,comando.length());
      String valor="";
      
      if(orden.equals("44a09576e4ab0f0588e7a780cbb50aca")){
        eliminarArchivo("conexion.txt");
        modificarConexion("permitido");
        Serial.println("444bcb3a3fcf8389296c49467f27e1d6");
      }else{
          eliminarArchivo("conexion.txt");
          modificarConexion("denegado");
          Serial.println("cb5e100e5a9a3e7f6d1fd97512215282");
      }
      indicadorLed();
    }

    //Comando para obtener conexion
    if(comando.equals("50975bacb0d830fffd090b08c8c11691")){
      String orden=leerArchivo(0,"conexion.txt");
      if(orden.equals("permitido")){
        Serial.println("40f3d08a8addd234ec4cd24a6bb893b9");
      }else{
          Serial.println("982b3a8b0359d633b137b30a4269a903");
      }
      indicadorLed();
    }
  }
  }

  
 //Final de llamadas

////////////////////////////////////////////////////////////////////////////////
//METODOS DE FUNCIONAMIENTO

//Metodo para leer una linea especifica de un archivo
String leerArchivo(int Linea,char* Ruta){
  int Lin=0;
  String Resultado;
  byte Bin;
  File myFile=SD.open(Ruta,FILE_READ);
  if (myFile) {
    while (myFile.available()) {
      Bin=myFile.read();
       if (Bin==10){
        Lin++;
      }else{
        if (Lin==Linea){
          Resultado=Resultado+(char(Bin));
        }
        if (Lin>Linea){
          myFile.close();
        }
      }    
    }
    Resultado=Resultado.substring(0,Resultado.length());
    myFile.close();
  }else {
    Resultado= "No se puede leer el archivo";
  }
  return Resultado;
}

//Metodo para verificar estado de conexion
int verificarConexion(){
  String conexion=leerArchivo(0,"conexion.txt");
  if(conexion.equals("permitir")){
    return 1;
  }else{
    return 0;
  }
}

//Metodo para modificar estado de conexion
void modificarConexion(String valor ){
  File dataFile=SD.open("conexion.txt",FILE_WRITE);
  if(dataFile){
    dataFile.print(valor);
    dataFile.close();
  }
}


//Metodo que verifica si hay datos de configuración guardados en el arduino
String verificarEstado(){
  String estado="";
  String id="0";
   if(tarjeta==1){
      if (SD.exists("config.txt")) {
        id=obtenerIdUltimo("mac.txt");
        estado="ae47b813280d669ecc58b4f17cbd59fc"+id;
      }else{
          estado="18f854b217d390ef7916cfeb407b8b74"+id;
      }
    }else{
      estado="8560bb2b8e0e1748779e0061bda97cc8"+id;
    }
  return estado;
}

//Metodo para escribir datos de usuario
void escribirUsuario(String user){
  String estadoEscritura="";
  File dataFile=SD.open("config.txt",FILE_WRITE);
  
  if(dataFile){
    dataFile.print(user);
    dataFile.close();
    estadoEscritura="444bcb3a3fcf8389296c49467f27e1d6";
  }else{
     estadoEscritura="925d75c0c95290a269b8db10e7add477";
  }
  Serial.println(estadoEscritura);
}

//Metodo para escribir los datos del móvil
void escribirMac(String mac){
  String estadoEscritura="";
  File dataFile=SD.open("mac.txt",FILE_WRITE);
  
  if(dataFile){
    dataFile.print(mac);
    dataFile.close();
    estadoEscritura="ok";
  }else{
     estadoEscritura="error_m";
  }
  Serial.println(estadoEscritura);
}

//Metodo para contar el numero total de filas en un archivo
int contadorFilas(char* archivo){
  int i=0;
  boolean val=true;
  String v="";
  while(val){
    v=leerArchivo(i,archivo);
    i++;
    if(v.equals("")|| v.equals("\t")){
      break;
    } 
  }
  return i;
}

//Metodo para obtener el ultimo id de fila de un archivo
String obtenerIdUltimo(char* archivo){
  String fila="";
  int inicio=0;
  int limite=contadorFilas(archivo);
  fila=leerArchivo(limite-2,archivo);
  
  for(int j=0;j<fila.length();j++){
    if(fila.charAt(j)=='\t'){
      inicio=j;
      break;
      }
  }
  return fila.substring(0,inicio);
}

//Metodo para verificar solo mac
int verificarsoloMac(String mac){
  int limit=contadorFilas("mac.txt");
  String valMac="";
  String subMac="";
  String acceso="";
  int estado=0;

  for(int i=1; i<limit; i++){
    valMac=leerArchivo(i,"mac.txt");  
    subMac=valMac.substring(valMac.length()-17,valMac.length());
    if(subMac.equals(mac)){
      estado=1;
      break;
    }
  }
  return estado;
}

//Metodo para verificar mac y estado de acceso
int verificarMac(String mac){
  int limit=contadorFilas("mac.txt");
  String valMac="";
  String subMac="";
  String acceso="";
  int estado=0;

  for(int i=1; i<limit; i++){
    valMac=leerArchivo(i,"mac.txt");
    
    subMac=valMac.substring(valMac.length()-17,valMac.length());
    if(subMac.equals(mac)){
      estado=1;
      break;
    }
  }
  
  for(int j=0;j<valMac.length();j++){ 
    if(valMac.charAt(j)=='\t'){
      acceso=valMac.substring(j+1,j+2);
      break;
    }
  }
  if(estado==1 && acceso.equals("1")){
    estado=1;
  }else{
    estado=0;
    }
  return estado; 
}

//Metodo para verificar el usuario a traves de la contraseña
String verificarUser(String clave){
  int limit=contadorFilas("config.txt");
  int limitInf=0;
  String valUser="";
  String subClave="";
  String rol="nan";
  int sizeUser=0;
  for(int i=1; i<limit; i++){
    valUser=leerArchivo(i,"config.txt");
    if(valUser.endsWith(clave)){
      rol=extraerRol(valUser);
    }
  } 
  return rol; 
}

//Metodo para verificar el estado de acceso de un usuario
String verificarDatos(String nombre){
  int limit=contadorFilas("config.txt");
  int limitInf=0;
  String fila="";
  String rol="nan";
  int sizeUser=0;
  for(int i=1; i<limit; i++){
    fila=leerArchivo(i,"config.txt");
    fila.replace('\t','%');
    if(extraerNomImagen(fila).equals(nombre)){
      
      if(extraerPermiso(fila).equals("1")){
         rol=extraerRol(fila);
          break;
      }else{
        rol="denegado";
        break;
      }    
    }else{
      rol="no_existe";
      break;
    }
  } 
  return rol;
}

//Metodo para extraer permisos de usuarios ok
String extraerPermiso(String fila){
  int cont=0;
  int inicio=0;
  int fin=0;
  for(int i=0;i<fila.length();i++){
    if(fila.charAt(i)=='%'){
      cont++;
    }
    if(cont==1){
      inicio=i+1;
      fin=i+2;
      break;
    }
  }
  return fila.substring(inicio,fin);
}

//Metodo para extraer nombre de imagen de usuarios ok
String extraerNomImagen(String fila){
  int cont=0;
  int inicio=0;
  int fin=0;
  String nombre="";
  for(int i=0;i<fila.length();i++){
    if(fila.charAt(i)=='%'){
      cont++;
    }
    if(cont==1){
      inicio=i+2;
    }
     if(cont==2){
      fin=i+1;  
    }
  }
  nombre=fila.substring(inicio,fin);  
  return nombre;
}

//Metodo para extraer rol de un usuario
String extraerRol(String linea){
  int limite=linea.length()-33;
  int inicio=limite-5;
  return linea.substring(inicio,limite);
}

//Metodo para eliminar un archivo
void eliminarArchivo(char* archivo){
  SD.remove(archivo);  
}

//Metodo para activar los indicadores Led
void indicadorLed(){
  digitalWrite(30,HIGH);
  delay(300);
  digitalWrite(30,LOW);
  delay(300);
  digitalWrite(30,HIGH);
  delay(300);
   digitalWrite(30,LOW);
  delay(300);
  digitalWrite(30,HIGH);
  delay(300);
  digitalWrite(30,LOW);
}

