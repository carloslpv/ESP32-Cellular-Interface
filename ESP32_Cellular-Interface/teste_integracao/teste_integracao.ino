#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

// Variáveis para armazenar dados
#define EEPROM_SIZE 1000

// Defina as credenciais Wi-Fi
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Defina o servidor web na porta 80
WebServer server(80);

typedef struct {
  int id;
  char numero[12];
  char operadora[3];
} Telefone;

typedef struct {
  int idTelefone;
  char mensagem[30];
} Mensagem;

/// HTML das páginas
const char* index_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='icon' href='data:,'>
    <style>
      :root {
        --background-color: #EEEEEE; 
        --button-color: #658864;
      }
      body {
        font-family: Arial, Helvetica, sans-serif;
        background-color: var(--background-color);
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: center;
        margin: 0;
      }
      .box {
        display: flex;
        flex-direction: column;
        border: 1px solid #000;
        padding: 5rem;
        list-style: none;
        justify-content: space-evenly;
      }
      .link__grande { 
        color: #000;
        font-size: 24px;
        text-decoration: none;
        position: relative;
        display: inline-block;
      }
      .link__grande::after {
        content: '';
        position: absolute;
        width: 100%;
        transform: scaleX(0);
        height: 2px;
        bottom: 0;
        left: 0;
        background-color: #000;
        transform-origin: bottom right;
        transition: transform 0.3s ease-out;
      }
      .link__grande:hover::after {
        transform: scaleX(1);
        transform-origin: bottom left;
      }
      .item { 
        padding: .5rem; 
        margin: .5rem; 
      }
      .card {
        position: relative;
        display: flex;
        flex-direction: column;
        min-width: 0;
        word-wrap: break-word;
        background-color: #fff;
        background-clip: border-box;
        border: 1px solid rgba(0, 0, 0, 0.125);
        border-radius: 1rem;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      }
    </style>
  </head>
  <body>
    <h1>Configurações do ESP32</h1>
    <ul class="card box">
      <li class="item"><a class="link__grande" href='cadastro.html'>Cadastrar Número e Mensagem</a></li>
      <li class="item"><a class="link__grande" href='visualizar.html'>Visualizar Cadastros</a></li>
      <li class="item"><a class="link__grande" href='cadastro.html'>Alterar Cadastros</a></li>
    </ul>
  </body>
</html>
)=====";

const char* cadastro_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='icon' href='data:,'>
    <style>
      :root {
        --background-color: #EEEEEE; 
        --button-color: #658864;
      }
      body{
        font-family: Arial, Helvetica, sans-serif;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: center;
        margin: 0;
        background-color: var(--background-color);
      }
      .formulario__campo { 
        display: flex;
        flex-direction: column;
        align-items: flex-start;
        justify-content: space-between;
      }
      .formulario__input { 
        min-width: 25rem;
        min-height: 2rem;
        border-radius: 15px;
      }
      .formulario {
        display: flex;
        align-items: start;
        flex-direction: column;
      }
      .btn {
        max-width: 5rem;
        max-height: 5rem;
        min-width: 6rem;
        min-height: 2rem;
        border-radius: 16px;
        border: none;
        cursor: pointer;
      }
      .btn-primary { 
        background-color: var(--button-color);
      }
      .card {
        position: relative;
        display: flex;
        flex-direction: column;
        min-width: 0;
        word-wrap: break-word;
        background-color: #fff;
        background-clip: border-box;
        border: 1px solid rgba(0, 0, 0, 0.125);
        border-radius: 1rem;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        padding: 2.5rem;
      }

      .formulario__campo select {
        min-width: 25rem;
        min-height: 2rem;
        border-radius: 15px;
        padding: 0.5rem;
        border: 1px solid #ccc;
        background-color: #fff;
        font-size: 1rem;
        color: #333;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        appearance: none;
        -webkit-appearance: none;
        -moz-appearance: none;
      }

      .formulario__campo select:focus {
        outline: none;
        border-color: var(--button-color);
        box-shadow: 0 0 0 3px rgba(0, 123, 255, 0.25);
      }

      .formulario__campo select option {
        padding: 0.5rem;
      }
    </style>
  </head>
  <body>
    <form  class="formulario card" action='/salvar' method='post'>
      <h1>Cadastrar Telefone</h1>
      <div class="formulario__campo">
        <label for='phone'>Número de Telefone:</label><br>
        <input class="formulario__input" type='text' id='phone' name='phone' value=''>
      </div>
      <br>
      <div class="formulario__campo">
        <select name="operator" required="required">
          <option value="">Selecione uma operadora</option>
          <option value="oi">Oi</option>
          <option value="vivo">Vivo</option>
          <option value="claro">Claro</option>
          <option value="tim">Tim</option>
        </select>
      </div>
      <br><br>
      <input class="btn btn-primary" type='submit' value='Salvar'>
    </form>
    <br>
    <a href='index.html'>Voltar</a>
  </body>
  <script>
  </script>
</html>
)=====";

const char* visualizar_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='icon' href='data:,'>
    <style>
      :root {
        --background-color: #EEEEEE; 
        --button-color: #658864;
      }
      body{
        font-family: Arial, Helvetica, sans-serif;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: center;
        margin: 0;
        background-color: var(--background-color);
      }
      .card { 
        background-origin: white;
        border-radius: 16px;
      }
      .container-flex {
        display: flex;
        justify-content: space-between;
        align-items: center;
      }
      .card {
        position: relative;
        display: flex;
        padding: 2.5rem;
        flex-direction: column;
        min-width: 0;
        word-wrap: break-word;
        background-color: #fff;
        background-clip: border-box;
        border: 1px solid rgba(0, 0, 0, 0.125);
        border-radius: 1rem;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Visualizar Cadastros</h1>
      <div class="container-flex">
        <p><strong>Contato</strong></p>
        <a class="btn btn-primary" href='acoes.html'>Ações</a>
      </div>
      <br>
    </div>
    <br>
    <a href='index.html'>Voltar</a>
  </body>
</html>
)=====";

const char* acoes_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
      :root {
        --background-color: #EEEEEE; 
        --button-color: #658864;
      }
      body{
        font-family: Arial, Helvetica, sans-serif;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: center;
        margin: 0;
        background-color: var(--background-color);
      }
      .card { 
        background-origin: white;
        border-radius: 16px;
      }
      .container-flex {
        display: flex;
        justify-content: space-between;
        align-items: center;
      }
      .card {
        position: relative;
        display: flex;
        padding: 2.5rem;
        flex-direction: column;
        min-width: 0;
        word-wrap: break-word;
        background-color: #fff;
        background-clip: border-box;
        border: 1px solid rgba(0, 0, 0, 0.125);
        border-radius: 1rem;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      }
      .btn {
        max-width: 5rem;
        max-height: 5rem;
        min-width: 6rem;
        min-height: 2rem;
        border-radius: 16px;
        border: none;
        cursor: pointer;
      }
      .btn-primary { 
        background-color: var(--button-color);
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Ações</h1>
      <div class="container-flex">
        <a class="btn btn-primary" href='alterar.html'>Alterar</a>
        <br>
        <a class="btn btn-primary" href='deletar.html'>Deletar</a>
      </div>
      <br>
    </div>
    <a href='index.html'>Voltar</a>
  </body>
</html>
)=====";

Telefone buildTelephone(String phoneString, String operatorNameString){
  char* operatorCode = verifyOperator(operatorNameString);
  const char* phone = phoneString.c_str();
  Telefone telefone;
  strcpy(telefone.numero, phone);
  strcpy(telefone.operadora, operatorCode);
  return telefone;
}

char* verifyOperator(String operatorName) {
  if (operatorName == "oi") {
    return "14";
  } else if (operatorName == "vivo") {
    return "15";
  } else if (operatorName == "claro") {
    return "21";
  } else if (operatorName == "tim") {
    return "41";
  }
  return "";
}
// Funções para lidar com as páginas
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleCadastro() {
  server.send(200, "text/html", cadastro_html);
}

void handleVisualizar() {
  server.send(200, "text/html", visualizar_html);
}

void handleAcoes() {
  server.send(200, "text/html", acoes_html);
}

void handleSalvarTelefone() {
  if (server.hasArg("phone") && server.hasArg("operator")) {
    String phone = server.arg("phone");
    String operatorName = server.arg("operator");

    Telefone telefone = buildTelephone(phone, operatorName);

    clearEEPROM();    
    // Salva os dados na EEPROM
    writeTelefone(0, telefone);
    Telefone telefono;
    Serial.println("TelefoneInformado");
    readTelefone(0, telefono);
    Serial.println(telefono.id);
    Serial.println(telefono.numero);
    Serial.println(telefono.operadora);
    
    server.send(200, "text/html", "<html><body><h1>Dados Salvos</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Erro ao salvar os dados</h1><a href='index.html'>Voltar</a></body></html>");
  }
}

void writeTelefone(int address, Telefone tel) {
  EEPROM.put(address, tel);
  EEPROM.commit();
}

void writeMensagem(int address, Mensagem msg) {
  EEPROM.put(address, msg);
  EEPROM.commit();
}

void readTelefone(int address, Telefone &tel) {
  EEPROM.get(address, tel);
}

void readMensagem(int address, Mensagem &msg) {
  EEPROM.get(address, msg);
}

void printTelefone(Telefone tel) {
  Serial.print("ID: ");
  Serial.println(tel.id);
  Serial.print("Número: ");
  Serial.println(tel.numero);
  Serial.print("Operadora: ");
  Serial.println(tel.operadora);
}

void printMensagem(Mensagem msg) {
  Serial.print("ID Telefone: ");
  Serial.println(msg.idTelefone);
  Serial.print("Mensagem: ");
  Serial.println(msg.mensagem);
}

void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  Serial.println("Subindo acess point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Endereço IP: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.on("/cadastro.html", handleCadastro);
  server.on("/visualizar.html", handleVisualizar);
  server.on("/acoes.html", handleAcoes);
  server.on("/salvar", handleSalvarTelefone);

  server.begin();
  Serial.println("HTTP server started");
  EEPROM.end();
}

void loop() {
  server.handleClient();
}