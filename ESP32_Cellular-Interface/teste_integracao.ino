#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

// OBS: ISSO COM CERTEZA NÃO VAI FUNCIONAR :)

// Defina as credenciais Wi-Fi
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Defina o servidor web na porta 80
WebServer server(80);

// Variáveis para armazenar dados
const int eepromSize = 512;
const int maxNumbers = 5;
String phoneNumbers[maxNumbers];
String messages[maxNumbers][5];

// HTML das páginas
const char* index_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
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
    </style>
  </head>
  <body>
    <form  class="formulario card" action='/salvar' method='post'>
      <h1>Cadastrar Número e Mensagem</h1>
      <div class="formulario__campo">
        <label for='phone'>Número de Telefone:</label><br>
        <input class="formulario__input" type='text' id='phone' name='phone' value=''>
      </div>
      <br>
      <div class="formulario__campo">
        <label for='message'>Mensagem:</label><br>
        <textarea class="formulario__input" id='message' name='message'></textarea>
      </div>
      <br><br>
      <input class="btn btn-primary" type='submit' value='Salvar'>
    </form>
    <br>
    <a href='index.html'>Voltar</a>
  </body>
</html>
)=====";

const char* visualizar_html = R"=====(
<!DOCTYPE html>
<html>
  <head>
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
      .btn-primary { 
        background-color: var(--button-color);
        color:#fff; 
        text-decoration: none;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Ações</h1>
      <ul class="lista">
        <li>
          <p>487887878787</p>
          <button type="button">LIGAR</button>
        </li>
        <li>
          <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse eget dignissim lacus, nec suscipit lectus.</p>
          <button type="button">OLHA A MENSAGEM!</button>
        </li>
        <li>
          <br>
          <a href='visualizar.html'>Voltar</a>
        </li>
      </ul>  
    </div>
  </body>
</html>
)=====";

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

void handleSalvar() {
  if (server.hasArg("phone") && server.hasArg("message")) {
    String phone = server.arg("phone");
    String message = server.arg("message");

    // Salva o número e a mensagem na memória EEPROM
    for (int i = 0; i < maxNumbers; i++) {
      if (phoneNumbers[i].isEmpty()) {
        phoneNumbers[i] = phone;
        for (int j = 0; j < 5; j++) {
          if (messages[i][j].isEmpty()) {
            messages[i][j] = message;
            break;
          }
        }
        break;
      }
    }
    EEPROM.commit();
    server.send(200, "text/html", "<html><body><h1>Dados Salvos</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Erro ao salvar os dados</h1><a href='index.html'>Voltar</a></body></html>");
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(eepromSize);
  for (int i = 0; i < eepromSize; i++) {
    phoneNumbers[i] = EEPROM.readString(i * 10);
    for (int j = 0; j < 5; j++) {
      messages[i][j] = EEPROM.readString(i * 10 + j * 10);
    }
  }

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.on("/cadastro.html", handleCadastro);
  server.on("/visualizar.html", handleVisualizar);
  server.on("/acoes.html", handleAcoes);
  server.on("/salvar", handleSalvar);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
