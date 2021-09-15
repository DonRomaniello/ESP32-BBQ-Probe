#include <SPIFFS.h>
 
void setup() {
 
  Serial.begin(115200);
  Serial.println();
 
  bool success = SPIFFS.begin();
 
  if (!success) {
    Serial.println("Error mounting the file system");
    return;
  }
 
  File file = SPIFFS.open("/file.txt", "w");
 
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
 
  int bytesWritten = file.print("TEST SPIFFS");
 
  if (bytesWritten == 0) {
    Serial.println("File write failed");
    return;
  }
 
  file.close();
 
// git commit comment 
  File file2 = SPIFFS.open("/file.txt", "r");
 
  if (!file2) {
    Serial.println("Failed to open file for reading");
    return;
  }
 
  Serial.println("File Content:");
 
  while (file2.available()) {
 
    Serial.write(file2.read());
  }
 
  file2.close();

File file3 = SPIFFS.open("/text.txt", "r");
 
  if (!file3) {
    Serial.println("Failed to open file for reading");
    return;
  }
 
  Serial.println("File Content:");
 
  while (file3.available()) {
 
    Serial.write(file3.read());
  }
 
  file3.close();
  
}
 

void loop() {}
