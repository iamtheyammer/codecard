/*
  cli.h
  
*/

void help() {
  Serial.println(F("***************************************************************************************"));
  Serial.println("  " + String(projectName) + " v" + String(projectVersion));
  Serial.println("  " + String(projectAuthor));
  Serial.println("  " + String(projectSite)); 
  Serial.println(F("***************************************************************************************"));
  Serial.println(F("Commands:"));
  Serial.println(F("  ls                Show all stored key/values"));
  Serial.println(F("  help              Show this help"));  
  Serial.println(F("  shortpress[a|b]   Simulate the press of a button")); 
  Serial.println(F("  longpress[a|b]    Simulate the long press of a button"));  
  Serial.println(F("  connect           Connect to wifi"));  
  Serial.println(F("  disconnect        Disconnect wifi"));   
  Serial.println(F("  restart           Restart wifi"));  
  Serial.println(F("  status            Show wifi status"));
  Serial.println(F("  home              Show home screen"));    
  Serial.println(F("  reset             Reset to factory settings"));     
  Serial.println();
  Serial.println(F("Usage:"));
  Serial.println(F("  Read saved key value:"));
  Serial.println(F("    key"));
  Serial.println(F("  Save new key value:")); 
  Serial.println(F("    key=[value]"));
  Serial.println();
  Serial.println(F("Available keys:"));
  Serial.print(F("  "));
  for (int i=0; i < keysLen; i++) {
    if (i == 8) {
      Serial.println(F(""));
      Serial.print(F("  "));
    }
    if (keys[i] == "initalsetup") {
      continue;
    } 
    Serial.print(keys[i]);
    if (i != keysLen - 1) {
      Serial.print(F(", "));
    }
    
  }
  Serial.println(F(""));
  Serial.println(F(">>>"));
}

void showStatus() {
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("MAC address: "));
  Serial.println(WiFi.macAddress());
  WiFi.printDiag(Serial);
  Serial.println(F(">>>")); 
}

void listAll() {
  for (int i=0; i < keysLen; i++) {
    String key = keys[i];
    if (key.indexOf("initalsetup") > -1) {
      continue;
    }  
    Serial.print(key);
    Serial.print(F(": "));
    String val = getFromMemory(i);
    if (key.indexOf("method") > -1 && val == "") {
      val = "GET";
    }
//    if (key.indexOf("initalsetup") > -1 && val == "") {
//      val = "true";
//    }    
    Serial.println(val);
  }
  Serial.println(F(">>>")); 
}

void eraseAll(){
  for (int i = 0; i < eepromSize; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  WiFi.disconnect(true);
  Serial.println(F("All values have been erased!"));
  listAll();
}

void loadDefaults(){
  for (int i=0; i < keysLen; i++) {
    String key = keys[i];
    String val = "";
    if (key.indexOf("ssid") > -1 && val == "") {
      saveToMemory(i, defaultSSID);
    }
    if (key.indexOf("password") > -1 && val == "") {
      saveToMemory(i, defaultPassword);
    }
    if (key.indexOf("button") > -1 && val == "") {
      saveToMemory(i, defaultUrl);
    }
    if (key.indexOf("fingerprint") > -1 && val == "") {
      saveToMemory(i, defaultFingerprint);
    }        
  }
  saveToMemory(getKeyIndex("initalsetup"), "false");
  
  Serial.println(F("Default urls and sha fingerprints loaded.")); 
  Serial.println(F(">>>")); 
}

void evalInput(String input) {
  // get the location of the first equals sign
  int firstEqualsLoc = input.indexOf("=");
  // the beginning of the string until the equals sign
  String key = input.substring(0, firstEqualsLoc);
  // after the first equals sign to the end of the string
  String val = firstEqualsLoc == -1 ? "" : input.substring(firstEqualsLoc + 1);
  
  if (key == "ls" || key == "ll") { listAll(); return; }
  if (key == "help") { help(); return; }
  if (key == "eraseall") { eraseAll(); return; } // not displayed under help
  if (key == "status") { showStatus(); return; }
  if (key == "connect") { connectWifi(); return; }  
  if (key == "disconnect") { disconnectWifi(); return; }  
  if (key == "restart") { restartWifi(); return; }   
  if (key == "heap") { Serial.println("Free HEAP: " + String(ESP.getFreeHeap())); return; }
  if (key == "home") { defaultScreen(); return; }
  if (key == "reset") { loadDefaults(); return; }

  if (key.indexOf("longpress") > -1) {
    String btnLabel = key.substring(9);
    if (btnLabel == "a" || btnLabel == "b"){
      pushButton(btnLabel, 2);
    }else{
      Serial.println(F("Not a valid button!"));
      Serial.println(F(">>>"));
    }
    return;
  } 
    
  if (key.indexOf("shortpress") > -1) {
    String btnLabel = key.substring(10);
    if (btnLabel == "a" || btnLabel == "b"){
      pushButton(btnLabel, 1);
    }else{
      Serial.println(F("Not a valid button!"));
      Serial.println(F(">>>"));
    }
    return;
  }
   
  int keyIndex = getKeyIndex(key);
  if (keyIndex == -1) { 
    Serial.print(F("'"));
    Serial.print(key);
    Serial.print(F("'"));
    Serial.println(F(" is not a valid key or command! Type 'help' or 'ls' for more info."));
    Serial.println(F(">>>"));
    return; 
  }

  if (val != "") {
      if (key.indexOf("method") > -1) {
        val.toUpperCase();  
      }
      if (val == "\"\"" || val == "''") { 
        val=""; 
      }
      saveToMemory(keyIndex, val);
      Serial.print(F("Value saved for "));
      Serial.print(key);
      Serial.print(F(": "));
      Serial.println(val);
      Serial.println(F(">>>"));
      if (key == "ssid" || key == "password"){
        Serial.println(F("Use the command 'restart' for changes to take effect. "));
        Serial.println(F(">>>"));
        return;
      }
  } else {
      Serial.print(key);
      Serial.print(F(": "));
      String val = getFromMemory(keyIndex);
      if (key.indexOf("method") > -1 && val == "") {
        val = "GET";
      }
      Serial.println(val);
      Serial.println(F(">>>")); 
  }
    
}
