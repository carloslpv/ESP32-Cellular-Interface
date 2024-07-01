#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Variáveis para armazenar dados
#define EEPROM_SIZE 1000
#define P1_ADDRS 0
#define P2_ADDRS 18
#define P3_ADDRS 36
#define P4_ADDRS 61
#define P5_ADDRS 79
#define M1_ADDRS 97
#define M2_ADDRS 131
#define M3_ADDRS 165
#define M4_ADDRS 199
#define M5_ADDRS 233
#define M6_ADDRS 267
#define M7_ADDRS 301
#define M8_ADDRS 335
#define M9_ADDRS 369
#define M10_ADDRS 403
#define M11_ADDRS 437
#define M12_ADDRS 471
#define M13_ADDRS 505
#define M14_ADDRS 539
#define M15_ADDRS 573
#define M16_ADDRS 607
#define M17_ADDRS 641
#define M18_ADDRS 675
#define M19_ADDRS 709
#define M20_ADDRS 743
#define M21_ADDRS 777
#define M22_ADDRS 811
#define M23_ADDRS 845
#define M24_ADDRS 879
#define M25_ADDRS 913
#define MAX_PHONES 5
#define MAX_MESSAGES 25

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
  int id;
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
      <li class="item"><a class="link__grande" href='cadastro.html'>Cadastrar Número</a></li>
      <li class="item"><a class="link__grande" href='visualizar.html'>Visualizar Telefones Cadastrados</a></li>
      <li class="item"><a class="link__grande" href='cadastro.html'>Alterar Cadastros</a></li>
      <li class="item"><a class="link__grande" href='cadastraMessage.html'>Cadastrar Mensagem</a></li>
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
    <form  class="formulario card" action='/salvarPhone' method='post'>
      <h1>Cadastrar Telefone</h1>
      <div class="formulario__campo">
        <label for='number'>Número de Telefone:</label><br>
        <input class="formulario__input" type='text' id='number' name='number' value=''>
      </div>
      <br>
      <div class="formulario__campo">
        <select name="operator" required="required">
          <option value="" disabled selected>Selecione uma operadora</option>
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
    <script>
      document.addEventListener("DOMContentLoaded", function() {
        const dataJson = $DATA_JSON$;
        const container = document.getElementById('telefone-list');
        dataJson.forEach(telefone => {
          const p = document.createElement('p');
          p.innerHTML = `Número: ${telefone.numero} <br> Operadora: ${telefone.operadora}`;
          container.appendChild(p);
          const hr = document.createElement('hr');
          container.appendChild(hr);
        });
      });
    </script>
  </head>
  <body>
    <div class="card">
      <h1>Visualizar Cadastros</h1>
      <div class="container-flex">
        <p><strong>Contato</strong></p>
        <a class="btn btn-primary" href='acoes.html'>Ações</a>
      </div>
      <br>
      <div id="telefone-list"></div>
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

const char* cadastraMessage_html = R"=====(
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
    .text-area { 
        min-width: 25vw;
        min-height: 10vh;
        padding: .5rem;
    }
    .select {
        background-color: #fff;
        
    }
    .w-25vw {
        min-width: 25vw;
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
  </style>
</head>
<body>
  <form  class="formulario card" action='/saveMessage' method='post'>
    <h1>Vincular mensagem ao telefone:</h1>
    <div class="formulario__campo">
      <label for='phone'>Número de Telefone:</label><br>
      <select class="formulario__input select w-25vw" name="phone">
        <option value=""disabled selected>Selecione um número de telefone</option>
        $OPTIONS$
      </select>
    </div>
    <br>
    <div class="formulario__campo">
      <label for='message'>Mensagem:</label><br>
      <textarea maxlength="30" class="formulario__input text-area" id='message' name='message'></textarea>
    </div>
    <br><br>
    <input class="btn btn-primary" type='submit' value='Salvar'>
  </form>
  <br>
  <a href='index.html'>Voltar</a>
</body>
</html>
)=====";

void returnAllMemoryAddressPhones(int* listPhones) {
  Telefone telefone;

  EEPROM.get(P1_ADDRS, telefone);
  if (telefone.id != 0) {
    listPhones[0] = P1_ADDRS;
  }

  EEPROM.get(P2_ADDRS, telefone);
  if (telefone.id != 0) {
    listPhones[1] = P2_ADDRS;
  }

  EEPROM.get(P3_ADDRS, telefone);
  if (telefone.id != 0) {
    listPhones[2] = P3_ADDRS;
  }

  EEPROM.get(P4_ADDRS, telefone);
  if (telefone.id != 0) {
    listPhones[3] = P4_ADDRS;
  }

  EEPROM.get(P5_ADDRS, telefone);
  if (telefone.id != 0) {
    listPhones[4] = P5_ADDRS;
  }
}

int checkPhoneIndexWithAddress(int memoryAddress) {
  if (memoryAddress == P1_ADDRS) {
    return 1;
  } else if (memoryAddress == P2_ADDRS) {
    return 2;
  } else if (memoryAddress == P3_ADDRS) {
    return 3;
  } else if (memoryAddress == P4_ADDRS) {
    return 4;
  } else if (memoryAddress == P5_ADDRS) {
    return 5;
  }
  return -1;
}

int checkMessageIndexWithAddress(int memoryAddress){

  if(memoryAddress == M1_ADDRS || memoryAddress == M6_ADDRS || memoryAddress == M11_ADDRS || memoryAddress == M16_ADDRS || memoryAddress == M21_ADDRS){
    return 1;
  } else if (memoryAddress == M2_ADDRS || memoryAddress == M7_ADDRS || memoryAddress == M12_ADDRS || memoryAddress == M17_ADDRS || memoryAddress == M22_ADDRS){
    return 2;
  } else if (memoryAddress == M3_ADDRS || memoryAddress == M8_ADDRS || memoryAddress == M13_ADDRS || memoryAddress == M18_ADDRS || memoryAddress == M23_ADDRS){
    return 3;
  } else if (memoryAddress == M4_ADDRS || memoryAddress == M9_ADDRS || memoryAddress == M14_ADDRS || memoryAddress ==M19_ADDRS || memoryAddress ==M24_ADDRS){
    return 4;
  } else if (memoryAddress == M5_ADDRS || memoryAddress == M10_ADDRS || memoryAddress == M15_ADDRS || memoryAddress == M20_ADDRS || memoryAddress == M25_ADDRS){
    return 5;
  }
  return -1;
}

int checkFreePhoneMemoryAddress() {
  Telefone telefone;

  EEPROM.get(P1_ADDRS, telefone);
  if (telefone.id == 0) {
    return P1_ADDRS;
  }

  EEPROM.get(P2_ADDRS, telefone);
  if (telefone.id == 0) {
    return P2_ADDRS;
  }

  EEPROM.get(P3_ADDRS, telefone);
  if (telefone.id == 0) {
    return P3_ADDRS;
  }

  EEPROM.get(P4_ADDRS, telefone);
  if (telefone.id == 0) {
    return P4_ADDRS;
  }

  EEPROM.get(P5_ADDRS, telefone);
  if (telefone.id == 0) {
    return P5_ADDRS;
  }
  
  return -1;
}

int checkFreeMessageMemoryAddress(int phoneId) {
  Mensagem mensagem;

  if (phoneId == 1){
    EEPROM.get(M1_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M1_ADDRS;
    }

    EEPROM.get(M2_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M2_ADDRS;
    }

    EEPROM.get(M3_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M3_ADDRS;
    }

    EEPROM.get(M4_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M4_ADDRS;
    }

    EEPROM.get(M5_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M5_ADDRS;
    }
  } else if (phoneId == 2){
    EEPROM.get(M6_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M6_ADDRS;
    }

    EEPROM.get(M7_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M7_ADDRS;
    }

    EEPROM.get(M8_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M8_ADDRS;
    }

    EEPROM.get(M9_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M9_ADDRS;
    }

    EEPROM.get(M10_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M10_ADDRS;
    }
  } else if (phoneId == 3){
    EEPROM.get(M11_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M11_ADDRS;
    }

    EEPROM.get(M12_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M12_ADDRS;
    }

    EEPROM.get(M13_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M13_ADDRS;
    }

    EEPROM.get(M14_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M14_ADDRS;
    }

    EEPROM.get(M15_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M15_ADDRS;
    }
  } else if (phoneId == 4) {
    EEPROM.get(M16_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M16_ADDRS;
    }

    EEPROM.get(M17_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M17_ADDRS;
    }

    EEPROM.get(M18_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M18_ADDRS;
    }

    EEPROM.get(M19_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M19_ADDRS;
    }

    EEPROM.get(M20_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M20_ADDRS;
    }
  } else if (phoneId == 5) {
    EEPROM.get(M21_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M21_ADDRS;
    }

    EEPROM.get(M22_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M22_ADDRS;
    }

    EEPROM.get(M23_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M23_ADDRS;
    }

    EEPROM.get(M24_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M24_ADDRS;
    }

    EEPROM.get(M25_ADDRS, mensagem);
    if (mensagem.id == 0) {
      return M25_ADDRS;
    }
  }

  return -1;
}

int checkPhoneMemoryAddressWithNumber(const char* phoneNumber) {
  Telefone telefone;

  EEPROM.get(P1_ADDRS, telefone);
  if (strcmp(telefone.numero,phoneNumber) == 0) {
    return P1_ADDRS;
  }

  EEPROM.get(P2_ADDRS, telefone);
  if (strcmp(telefone.numero,phoneNumber) == 0) {
    return P2_ADDRS;
  }

  EEPROM.get(P3_ADDRS, telefone);
  if (strcmp(telefone.numero,phoneNumber) == 0) {
    return P3_ADDRS;
  }

  EEPROM.get(P4_ADDRS, telefone);
  if (strcmp(telefone.numero,phoneNumber) == 0) {
    return P4_ADDRS;
  }

  EEPROM.get(P5_ADDRS, telefone);
  if (strcmp(telefone.numero,phoneNumber) == 0) {
    return P5_ADDRS;
  }

  return -1;
}

Telefone buildTelephone(int id, String numberString, String operatorNameString) {
  char* operatorCode = verifyOperator(operatorNameString);
  const char* number = numberString.c_str();
  Telefone telefone;
  telefone.id = id;
  strcpy(telefone.numero, number);
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
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  int listPhones[5];
  returnAllMemoryAddressPhones(listPhones);
  Telefone telefone;

  for (int i = 0; i < MAX_PHONES; i++) {
    readTelefone(listPhones[i], telefone);
    if (telefone.id > 0) {
      JsonObject obj = array.createNestedObject();
      obj["numero"] = telefone.numero;
      obj["operadora"] = telefone.operadora;
    }
  }

  String json;
  serializeJson(doc, json);

  String html = String(visualizar_html);
  html.replace("$DATA_JSON$", json);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", html);
}

void handleAcoes() {
  server.send(200, "text/html", acoes_html);
}

void handleSavePhone() {
  if (server.hasArg("number") && server.hasArg("operator")) {
    String number = server.arg("number");
    String operatorName = server.arg("operator");

    int memoryAddress = checkFreePhoneMemoryAddress();
    int id = checkPhoneIndexWithAddress(memoryAddress);
    if (id < 0) {
      server.send(400, "text/html", "<html><body><h1>Limite de números atingidos, faça a exclusão de um cadastro de Telefone para seguir</h1><a href='index.html'>Voltar</a></body></html>");
    }
    Telefone telefone = buildTelephone(id, number, operatorName);

    writeTelefone(memoryAddress, telefone);
    Serial.println("Telefone cadastrado com sucesso!");
    server.send(200, "text/html", "<html><body><h1>Dados Salvos</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Erro ao salvar os dados</h1><a href='index.html'>Voltar</a></body></html>");
  }
}

void handleCadastraMessageGetPhones(){
   int listPhones[MAX_PHONES];
    returnAllMemoryAddressPhones(listPhones);
    Telefone telefone;

    String options = "";
    for (int i = 0; i < MAX_PHONES; i++) {
        readTelefone(listPhones[i], telefone);
        if (telefone.id > 0) {
            options += "<option value='" + String(telefone.numero) + "'>" + String(telefone.numero) + "</option>";
        }
    }

    String html = String(cadastraMessage_html);
    html.replace("$OPTIONS$", options);

    server.send(200, "text/html", html);
}

Mensagem buildMensagem(int messageIndex, int phoneId, String messageString){
  const char* message  = messageString.c_str();
  Mensagem mensagem;
  mensagem.id = messageIndex;
  mensagem.idTelefone = phoneId;
  strcpy(mensagem.mensagem, message);
  return mensagem;
}

void handleSaveMessage(){
  if (server.hasArg("phone") && server.hasArg("message")) {
    String phone = server.arg("phone");
    String message = server.arg("message");
    const char* number = phone.c_str();
    int phoneMemoryAddress = checkPhoneMemoryAddressWithNumber(number);
    int phoneId = checkPhoneIndexWithAddress(phoneMemoryAddress);
    int messageMemoryAddress = checkFreeMessageMemoryAddress(phoneId);
    int messageIndex = checkMessageIndexWithAddress(messageMemoryAddress);
    if(messageIndex >= 0){
      Mensagem mensagem = buildMensagem(messageIndex, phoneId, message);
      writeMensagem(messageMemoryAddress, mensagem);
      Serial.println("Mensagem cadastrada com sucesso...");
      Serial.print("Endereco: ");Serial.println(messageMemoryAddress);
      Serial.println(mensagem.id);
      Serial.println(mensagem.idTelefone);
      Serial.println(mensagem.mensagem);
      server.send(200, "text/html", "<html><body><h1>Dados Salvos</h1><a href='index.html'>Voltar</a></body></html>");
    } else if(messageIndex < 0){
      server.send(400, "text/html", "<html><body><h1>Limite de mensagens excedido, favor excluir um cadastro</h1><a href='index.html'>Voltar</a></body></html>");
    }
  }
  server.send(400, "text/html", "<html><body><h1>Erro ao salvar a mensagem</h1><a href='index.html'>Voltar</a></body></html>");
}

void writeTelefone(int address, Telefone tel) {
  EEPROM.put(address, tel);
  EEPROM.commit();
}

void writeMensagem(int address, Mensagem msg) {
  EEPROM.put(address, msg);
  EEPROM.commit();
}

void readTelefone(int address, Telefone& tel) {
  EEPROM.get(address, tel);
}

void readMensagem(int address, Mensagem& msg) {
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

int returnPhoneFromNumber(String number) {
  Telefone telefone;
  EEPROM.get(P1_ADDRS, telefone);
  if (strcmp(telefone.numero, number.c_str()) == 0) {
    return P1_ADDRS;
  }

  EEPROM.get(P2_ADDRS, telefone);
  if (strcmp(telefone.numero, number.c_str()) == 0) {
    return P2_ADDRS;
  }

  EEPROM.get(P3_ADDRS, telefone);
  if (strcmp(telefone.numero, number.c_str()) == 0) {
    return P3_ADDRS;
  }

  EEPROM.get(P4_ADDRS, telefone);
  if (strcmp(telefone.numero, number.c_str()) == 0) {
    return P4_ADDRS;
  }

  EEPROM.get(P5_ADDRS, telefone);
  if (strcmp(telefone.numero, number.c_str()) == 0) {
    return P5_ADDRS;
  }

  return -1;
}

void sendCall(String number) {

  Telefone telefone;
  int phoneAddress = returnPhoneFromNumber(number);
  EEPROM.get(phoneAddress, telefone);


  String comando = "ATD0" + String(telefone.operadora) + String(telefone.numero) + ";\n";
  Serial.println("testechamada");
  Serial.println(comando);
  updateSerial();
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    Serial2.write(Serial.read());  //Forward what Serial received to Software Serial Port
  }
  while (Serial2.available()) {
    Serial.write(Serial2.read());  //Forward what Software Serial received to Serial Port
  }
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
  server.on("/salvarPhone", handleSavePhone);
  server.on("/cadastraMessage.html", handleCadastraMessageGetPhones);
  server.on("/saveMessage", handleSaveMessage);

  server.begin();
  Serial.println("HTTP server started");

  sendCall("49988888888");
}

void loop() {
  server.handleClient();
}