#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

//atualizar dados do ip conforme sua rede
IPAddress ip(192,168,25,20);
IPAddress gateway(192,168,25,1);
IPAddress subnet(255,255,255,0);

EthernetServer server(1000); //porta

const int LM35 = A0; // Pino Analogico onde vai ser ligado ao pino 2 do LM35
const int REFRESH_RATE = 2000;  //Tempo de atualização entre as leituras em ms
const float CELSIUS_BASE = 0.4887585532746823069403714565; //Base de conversão para Graus Celsius 
float sensor1 = 0;

int LUZ1 = 2;

void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando programa..");
  Ethernet.begin(mac,ip,gateway,subnet);
  Serial.println("Server ativo no IP: ");
  Serial.print(Ethernet.localIP());
  server.begin();

  pinMode(LUZ1, OUTPUT);
  digitalWrite(LUZ1, LOW);
}

void loop()
{
  EthernetClient client = server.available();
  if(client)
  {
    boolean continua = true;
    String linha = "";

    while(client.connected())
    {
      if(client.available()){
        char c = client.read();
        linha.concat(c);
  
        if(c == '\n' && continua)
        {
          client.println("HTTP/1.1 200 OK");
          // IMPORTANTE, ISSO FAZ O ARDUINO RECEBER REQUISIÇÃO AJAX DE OUTRO SERVIDOR E NÃO APENAS LOCAL.
          client.println("Content-Type: text/javascript");
          client.println("Access-Control-Allow-Origin: *");
          client.println();          
         
          int iniciofrente = linha.indexOf("?");
                
          if(iniciofrente>-1){     //verifica se o comando veio
            iniciofrente     = iniciofrente+6; //pega o caractere seguinte
            int fimfrente    = iniciofrente+3; //esse comando espero 3 caracteres
            String acao    = linha.substring(iniciofrente,fimfrente);//recupero o valor do comando
            
 
            if      ( acao == "001"){ 
              digitalWrite(LUZ1, true); 
            }

             else if (acao == "002"){
              digitalWrite(LUZ1, false);
            }
            
            
            sensor1 = readTemperature();
            
            client.print("dados({ sensor1 : ");
            client.print(sensor1);
            client.print(",");
            client.print(" LUZ1 : ");
            client.print(digitalRead(LUZ1));
            client.print("})");
         }          
          break;
        }
        if(c == '\n') { continua = true; }
        else if (c != '\r') { continua = false; }
      }
    }
     delay(1);
     client.stop();
  }
}
  float readTemperature(){
    Serial.println(analogRead(LM35));
    return (analogRead(LM35) * CELSIUS_BASE); 
  }
  
          
          /*
          
          #####################
          ## Debug na serial ##
          #####################
          
          sensor1 = analogRead(analogPin1);
          delay(10);
          sensor2 = analogRead(analogPin2);
          
          Serial.println("dados({ \'sensor1\' : ");
          delay(10);
          Serial.println(sensor1);
          delay(10);
          Serial.println(" , \'sensor2\' :  ");
          delay(10);
          Serial.println(sensor2);
          delay(10);
          Serial.println(" }) "); 
          delay(500);
          */
