#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Variables to store the phone number and message
String phoneNumber = "";
String message = "";


void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            if (header.indexOf("GET / ") >= 0) {
              handleRoot(client);
            } else if (header.indexOf("GET /cadastro") >= 0) {
              handleCadastro(client);
            } else if (header.indexOf("GET /visualizar") >= 0) {
              handleVisualizar(client);
            } else if (header.indexOf("GET /alterar") >= 0) {
              handleAlterar(client);
            } else if (header.indexOf("POST /salvar") >= 0) {
              handleSalvar(client, header);
            }
            
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
           currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void handleRoot(WiFiClient& client) {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='icon' href='data:,'>";
  html += "<style>body { font-family: Arial, sans-serif; } ul { list-style-type: none; padding: 0; } li { margin: 5px 0; }</style></head><body>";
  html += "<h1>Configurações do ESP32</h1>";
  html += "<ul><li><a href='/cadastro'>Cadastrar Número e Mensagem</a></li>";
  html += "<li><a href='/visualizar'>Visualizar Cadastros</a></li>";
  html += "<li><a href='/alterar'>Alterar Cadastros</a></li></ul>";
  html += "</body></html>";
  client.println(html);
}

void handleCadastro(WiFiClient& client) {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='icon' href='data:,'></head><body>";
  html += "<h1>Cadastrar Número e Mensagem</h1>";
  html += "<form action='/salvar' method='post'>";
  html += "<label for='phone'>Número de Telefone:</label><br>";
  html += "<input type='text' id='phone' name='phone' value='" + phoneNumber + "'><br>";
  html += "<label for='message'>Mensagem:</label><br>";
  html += "<textarea id='message' name='message'>" + message + "</textarea><br><br>";
  html += "<input type='submit' value='Salvar'>";
  html += "</form><br><a href='/'>Voltar</a></body></html>";
  client.println(html);
}

void handleSalvar(WiFiClient& client, String header) {
  if (header.indexOf("phone=") >= 0 && header.indexOf("message=") >= 0) {
    int phoneStart = header.indexOf("phone=") + 6;
    int phoneEnd = header.indexOf("&", phoneStart);
    int messageStart = header.indexOf("message=") + 8;
    int messageEnd = header.indexOf(" ", messageStart);

    phoneNumber = header.substring(phoneStart, phoneEnd);
    message = header.substring(messageStart, messageEnd);
    
    client.println("<html><body><h1>Dados Salvos</h1><a href='/'>Voltar</a></body></html>");
  } else {
    client.println("<html><body><h1>Erro ao salvar os dados</h1><a href='/'>Voltar</a></body></html>");
  }
}

void handleVisualizar(WiFiClient& client) {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='icon' href='data:,'></head><body>";
  html += "<h1>Visualizar Cadastros</h1>";
  html += "<p><strong>Número de Telefone:</strong> " + phoneNumber + "</p>";
  html += "<p><strong>Mensagem:</strong> " + message + "</p>";
  html += "<br><a href='/'>Voltar</a></body></html>";
  client.println(html);
}

void handleAlterar(WiFiClient& client) {
  handleCadastro(client);
}
