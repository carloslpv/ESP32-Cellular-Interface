#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Variáveis para armazenar dados
#define EEPROM_SIZE 1200
#define P1_ADDRS 0
#define P2_ADDRS 21
#define P3_ADDRS 42
#define P4_ADDRS 63
#define P5_ADDRS 84
#define M1_ADDRS 105
#define M2_ADDRS 146
#define M3_ADDRS 187
#define M4_ADDRS 228
#define M5_ADDRS 269
#define M6_ADDRS 310
#define M7_ADDRS 351
#define M8_ADDRS 392
#define M9_ADDRS 433
#define M10_ADDRS 474
#define M11_ADDRS 515
#define M12_ADDRS 556
#define M13_ADDRS 597
#define M14_ADDRS 638
#define M15_ADDRS 679
#define M16_ADDRS 720
#define M17_ADDRS 761
#define M18_ADDRS 802
#define M19_ADDRS 843
#define M20_ADDRS 884
#define M21_ADDRS 925
#define M22_ADDRS 966
#define M23_ADDRS 1007
#define M24_ADDRS 1048
#define M25_ADDRS 1089
#define MAX_PHONES 5
#define MAX_MESSAGES 25
#define MAX_MESSAGES_NMBR 5

// Defina as credenciais Wi-Fi
const char* ssid = "ESP32-IOT";
const char* password = "iot123";

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
              align-items: center;
              display: flex;
              justify-content: center;
              border-radius: 16px;
              border: none;
              cursor: pointer;
            }
        
            .btn-primary { 
              background-color: #007bff;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-danger { 
              background-color: #dc3545;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-call { 
              background-color: #28a745;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-sms { 
              background-color: #17a2b8;
              color:#fff; 
              text-decoration: none;
            }
            
            .container-flex {
              display: flex;
              justify-content: space-between;
              align-items: center;
              gap: .3rem;
            }

            .container-flex-column {
              display: flex;
              flex-direction: column;
              justify-content: space-between;
              align-items: center;
              gap: .3rem;
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
      </div>
      <br>
      <div id="telefone-list">
        $PHONES$
      </div>
    </div>
    <br>
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

const char* enviarSms_html = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <link rel='icon' href='data:,'>
        <style>
            :root {
              --background-color: #EEEEEE; 
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
              align-items: center;
              display: flex;
              justify-content: center;
              border-radius: 16px;
              border: none;
              cursor: pointer;
            }

            .paragrafo {
                max-width: 60%;
            }
        
            .btn-primary { 
              background-color: #007bff;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-danger { 
              background-color: #dc3545;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-call { 
              background-color: #28a745;
              color:#fff; 
              text-decoration: none;
            }
        
            .btn-sms { 
              background-color: #17a2b8;
              color:#fff; 
              text-decoration: none;
            }
            
            .container-flex {
              display: flex;
              justify-content: space-between;
              align-items: center;
              gap: .3rem;
            }

            .container-flex-column {
              display: flex;
              flex-direction: column;
              justify-content: space-between;
              align-items: center;
              gap: .3rem;
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
          <h1>Enviar Mensagem</h1>
          <div class="container-flex-column">
            <p><strong>Contato</strong></p>
            $MESSAGES$
          </div>
          <br>
        </div>
        <br>
        <a href='index.html'>Voltar</a>
    </body>
</html>
)=====";

void resetPhone(Telefone &telefone){
  telefone.id = 0;
  memset(telefone.numero, 0, sizeof(telefone.numero));
  memset(telefone.operadora, 0, sizeof(telefone.operadora));
}

void returnAllMemoryAddressMessages(int phoneId, int * listMessages){
  Mensagem mensagem;

  for (int i = 0; i < MAX_MESSAGES_NMBR; i++) {
    listMessages[i] = -1;
  }

  if (phoneId == 1){
    EEPROM.get(M1_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[0] = M1_ADDRS;
    }

    EEPROM.get(M2_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[1] = M2_ADDRS;
    }

    EEPROM.get(M3_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[2] = M3_ADDRS;
    }

    EEPROM.get(M4_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[3] = M4_ADDRS;
    }

    EEPROM.get(M5_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[4] = M5_ADDRS;
    }
  } else if (phoneId == 2){
    EEPROM.get(M6_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[0] =  M6_ADDRS;
    }

    EEPROM.get(M7_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[1] = M7_ADDRS;
    }

    EEPROM.get(M8_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[2] = M8_ADDRS;
    }

    EEPROM.get(M9_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[3] = M9_ADDRS;
    }

    EEPROM.get(M10_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[4] = M10_ADDRS;
    }
  } else if (phoneId == 3){
    EEPROM.get(M11_ADDRS, mensagem);
    if (mensagem.id == 0) {
      listMessages[0] = M11_ADDRS;
    }

    EEPROM.get(M12_ADDRS, mensagem);
    if (mensagem.id == 0) {
      listMessages[1] = M12_ADDRS;
    }

    EEPROM.get(M13_ADDRS, mensagem);
    if (mensagem.id == 0) {
      listMessages[2] = M13_ADDRS;
    }

    EEPROM.get(M14_ADDRS, mensagem);
    if (mensagem.id == 0) {
      listMessages[3] = M14_ADDRS;
    }

    EEPROM.get(M15_ADDRS, mensagem);
    if (mensagem.id == 0) {
      listMessages[4] = M15_ADDRS;
    }
  } else if (phoneId == 4) {
    EEPROM.get(M16_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[0] =  M16_ADDRS;
    }

    EEPROM.get(M17_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[1] = M17_ADDRS;
    }

    EEPROM.get(M18_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[2] = M18_ADDRS;
    }

    EEPROM.get(M19_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[3] = M19_ADDRS;
    }

    EEPROM.get(M20_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[4] = M20_ADDRS;
    }
  } else if (phoneId == 5) {
    EEPROM.get(M21_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[0] = M21_ADDRS;
    }

    EEPROM.get(M22_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[1] = M22_ADDRS;
    }

    EEPROM.get(M23_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[2] = M23_ADDRS;
    }

    EEPROM.get(M24_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[3] = M24_ADDRS;
    }

    EEPROM.get(M25_ADDRS, mensagem);
    if (mensagem.id != 0) {
      listMessages[4] = M25_ADDRS;
    }
  }
}

void returnAllMemoryAddressPhones(int* listPhones) {
  Telefone telefone;

  for (int i = 0; i < MAX_PHONES; i++) {
    listPhones[i] = -1;
  }

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

  resetPhone(telefone);
  Serial.println(telefone.id);
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
  int listPhones[MAX_PHONES];
  returnAllMemoryAddressPhones(listPhones);

  Telefone telefone;

  String divs = "";
  for (int i = 0; i < MAX_PHONES; i++) {
    if(listPhones[i] >= 0){
      readTelefone(listPhones[i], telefone);
      if (telefone.id > 0) {
        divs += "<div id = telefone:" + String(i) + "\>" + "Número: " + String(telefone.numero) + "<br>" + "Operadora: " + String(telefone.operadora) +  "</div>";
        divs += "<div class=\"container-flex\">";
        divs += "<a class=\"btn btn-primary\" href=\"cadastro.html\">Editar</a>";
        divs += "<a class=\"btn btn-danger\" href=delete.html?phoneId=" + String(telefone.id) + ">Excluir</a>";
        divs += "</div>";
        divs += "<div class=\"container-flex\">";
        divs += "<a class=\"btn btn-call\" href=sendCall.html?phoneId=" +String(telefone.id) + ">Ligar</a>";
        divs += "<a class=\"btn btn-sms\" href=enviarSms.html?phoneId=" + String(telefone.id) + ">SMS</a>";
        divs += "</div><br>";
      }
    }
  }
  String html = String(visualizar_html);
  html.replace("$PHONES$", divs);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", html);
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
      if(listPhones[i] >= 0){
        readTelefone(listPhones[i], telefone);
        if (telefone.id > 0) {
            options += "<option value='" + String(telefone.numero) + "'>" + String(telefone.numero) + "</option>";
        }
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
      server.send(200, "text/html", "<html><body><h1>Dados Salvos</h1><a href='index.html'>Voltar</a></body></html>");
    } else if(messageIndex < 0){
      server.send(400, "text/html", "<html><body><h1>Limite de mensagens excedido, favor excluir um cadastro</h1><a href='index.html'>Voltar</a></body></html>");
    }
  }
  server.send(400, "text/html", "<html><body><h1>Erro ao salvar a mensagem</h1><a href='index.html'>Voltar</a></body></html>");
}

void handleSendSms() {
  String numeroStr = server.arg("phoneId");
  int phoneId = numeroStr.toInt();
  int listMessages[MAX_MESSAGES_NMBR];

  returnAllMemoryAddressMessages(phoneId, listMessages);
  Mensagem mensagem;
  String messagesHtml;

  for (int i = 0; i < MAX_MESSAGES_NMBR; i++) {
    if(listMessages[i] >= 0){
      readMensagem(listMessages[i], mensagem);
      if (mensagem.id > 0) {
        messagesHtml += "<div class=\"container-flex\">";
        messagesHtml += "<p class=\"paragrafo\">" + String(mensagem.mensagem) + ".</p>";
        messagesHtml += "<a class='btn btn-call' href='sendMessageRequest.html?phoneId=" + String(phoneId) + "&messageId=" + String(mensagem.id) + ">Enviar</a>";
        messagesHtml += "</div>";
      }
    }
  }

  String html = String(enviarSms_html);
  html.replace("$MESSAGES$", messagesHtml);
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", html);
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
  if(address >= 0){
    EEPROM.get(address, tel);
  }
}

void readMensagem(int address, Mensagem& msg) {
  if(address >= 0){
    EEPROM.get(address, msg);
  }
}

void printMensagem(int address) {
  Mensagem mensagem;
  readMensagem(address, mensagem);
  Serial.print("Mensagem em ");
  Serial.print(address);
  Serial.print(": id = ");
  Serial.print(mensagem.id);
  Serial.print(", idTelefone = ");
  Serial.print(mensagem.idTelefone);
  Serial.print(", mensagem = ");
  Serial.println(mensagem.mensagem);
}

void printTelefone(int address) {
  Telefone telefone;
  readTelefone(address, telefone);
  Serial.print("Telefone em ");
  Serial.print(address);
  Serial.print(": id = ");
  Serial.print(telefone.id);
  Serial.print(", numero = ");
  Serial.print(telefone.numero);
  Serial.print(", operadora = ");
  Serial.println(telefone.operadora);
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
  Serial.println(comando);
  updateSerial();
}

void sendSms(String number, String message) {
  Serial.println("AT+CMGF=1\n");
  updateSerial();
  String comandoSms = "AT+CMGS=\"+55" + String(number) + "\"";
  Serial.println(comandoSms);
  updateSerial();
  Serial.println(message);
  updateSerial();
  Serial.write(26);
  updateSerial();
  
}

void handleSendSmsRequest() {
  if (server.hasArg("phoneId") && server.hasArg("messageId")) {
    int phoneId = server.arg("phoneId").toInt();
    int messageId = server.arg("messageId").toInt();
    
    Telefone telefone;
    Mensagem mensagem;
    int phoneAddress;
    int messageAddress;
    
    switch (phoneId) {
      case 1: phoneAddress = P1_ADDRS; break;
      case 2: phoneAddress = P2_ADDRS; break;
      case 3: phoneAddress = P3_ADDRS; break;
      case 4: phoneAddress = P4_ADDRS; break;
      case 5: phoneAddress = P5_ADDRS; break;
      default: 
        server.send(400, "text/html", "<html><body><h1>ID do telefone inválido</h1><a href='index.html'>Voltar</a></body></html>");
        return;
    }
    
    readTelefone(phoneAddress, telefone);
    
    switch (messageId) {
      case 1: messageAddress = (phoneId == 1) ? M1_ADDRS : (phoneId == 2) ? M6_ADDRS : (phoneId == 3) ? M11_ADDRS : (phoneId == 4) ? M16_ADDRS : M21_ADDRS; break;
      case 2: messageAddress = (phoneId == 1) ? M2_ADDRS : (phoneId == 2) ? M7_ADDRS : (phoneId == 3) ? M12_ADDRS : (phoneId == 4) ? M17_ADDRS : M22_ADDRS; break;
      case 3: messageAddress = (phoneId == 1) ? M3_ADDRS : (phoneId == 2) ? M8_ADDRS : (phoneId == 3) ? M13_ADDRS : (phoneId == 4) ? M18_ADDRS : M23_ADDRS; break;
      case 4: messageAddress = (phoneId == 1) ? M4_ADDRS : (phoneId == 2) ? M9_ADDRS : (phoneId == 3) ? M14_ADDRS : (phoneId == 4) ? M19_ADDRS : M24_ADDRS; break;
      case 5: messageAddress = (phoneId == 1) ? M5_ADDRS : (phoneId == 2) ? M10_ADDRS : (phoneId == 3) ? M15_ADDRS : (phoneId == 4) ? M20_ADDRS : M25_ADDRS; break;
      default: 
        server.send(400, "text/html", "<html><body><h1>ID da mensagem inválido</h1><a href='index.html'>Voltar</a></body></html>");
        return;
    }
    
    readMensagem(messageAddress, mensagem);
    sendSms(telefone.numero, mensagem.mensagem);
    
    server.send(200, "text/html", "<html><body><h1>SMS enviado com sucesso</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Parâmetros de solicitação ausentes</h1><a href='index.html'>Voltar</a></body></html>");
  }
}

void handleSendCallRequest() {
  if (server.hasArg("phoneId")) {
    int phoneId = server.arg("phoneId").toInt();
    
    Telefone telefone;
    int phoneAddress;
    
    switch (phoneId) {
      case 1: phoneAddress = P1_ADDRS; break;
      case 2: phoneAddress = P2_ADDRS; break;
      case 3: phoneAddress = P3_ADDRS; break;
      case 4: phoneAddress = P4_ADDRS; break;
      case 5: phoneAddress = P5_ADDRS; break;
      default: 
        server.send(400, "text/html", "<html><body><h1>ID do telefone inválido</h1><a href='index.html'>Voltar</a></body></html>");
        return;
    }
    
    readTelefone(phoneAddress, telefone);
    sendCall(telefone.numero);
    
    server.send(200, "text/html", "<html><body><h1>Chamada realizada com sucesso</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Parâmetros de solicitação ausentes</h1><a href='index.html'>Voltar</a></body></html>");
  }
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

void handleIndex(){
  server.send(200, "text/html", index_html);
}

void readMensagem(int address, Mensagem* mensagem) {
  EEPROM.get(address, *mensagem);
}

void handleDelete() {
  if (server.hasArg("phoneId")) {
    int phoneId = server.arg("phoneId").toInt();
    int listMessages[MAX_MESSAGES_NMBR];
    returnAllMemoryAddressMessages(phoneId, listMessages);

    for (int i = 0; i < MAX_MESSAGES_NMBR; i++) {
      if (listMessages[i] >= 0) {
        for (int j = 0; j < sizeof(Mensagem); j++) {
          EEPROM.write(listMessages[i] + j, 0);
        }
        Serial.println("Mensagem deletada...");
      }
    }

    int phoneAddress;
    switch (phoneId) {
      case 1: phoneAddress = P1_ADDRS; break;
      case 2: phoneAddress = P2_ADDRS; break;
      case 3: phoneAddress = P3_ADDRS; break;
      case 4: phoneAddress = P4_ADDRS; break;
      case 5: phoneAddress = P5_ADDRS; break;
      default: 
        server.send(400, "text/html", "<html><body><h1>ID do telefone inválido</h1><a href='index.html'>Voltar</a></body></html>");
        return;
    }

    for (int i = 0; i < sizeof(Telefone); i++) {
      EEPROM.write(phoneAddress + i, 0);
    }
    Serial.println("Telefone deletado...");

    EEPROM.commit();
    server.send(200, "text/html", "<html><body><h1>Dados deletados com sucesso</h1><a href='index.html'>Voltar</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Argumento phoneId ausente</h1><a href='index.html'>Voltar</a></body></html>");
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
  server.on("/salvarPhone", handleSavePhone);
  server.on("/cadastraMessage.html", handleCadastraMessageGetPhones);
  server.on("/saveMessage", handleSaveMessage);
  server.on("/enviarSms.html", handleSendSms);
  server.on("/index.html", handleIndex);
  server.on("/sendMessageRequest.html", handleSendSmsRequest);
  server.on("/sendCall.html", handleSendCallRequest);
  server.on("/delete.html", handleDelete);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}