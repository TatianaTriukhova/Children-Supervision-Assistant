import controlP5.*;
import processing.sound.*;
import processing.serial.*;

import com.temboo.core.*;
import com.temboo.Library.Google.Gmail.*;

//declarations for serial communication
Serial port;
String value;

//declarations for the GUI
ControlP5 cp5;
PFont fontik;

//declarations for playing mp3 file
SoundFile file;
int counter = 0;


// Create a session using your Temboo account application details
TembooSession session = new TembooSession("tatianatr", "myFirstApp", "eHHDLdXtdSw4o2LdR44Ttv8H3AH7Vuat");

//declarations for Email sending
String googleProfile = "Tanya";
String email;
String toAddress = email;
String subject = "Alert";
String messageBody = "I woke up!";


void sendEmail() {
  SendEmail emailObj = new SendEmail(session);

  // Set Profile
  emailObj.setCredential(googleProfile);

  // Set inputs
  emailObj.setMessageBody(messageBody);
  emailObj.setSubject(subject);
  //sendEmailChoreo.setFromAddress(fromAddress);
  emailObj.setToAddress(email);

  // Run the Choreo and store the results
  SendEmailResultSet sendEmailResults = emailObj.run();

  println(sendEmailResults.getSuccess());
}


void setup() {
  size(400, 250);
  String portName = Serial.list()[0];
  port = new Serial(this, portName, 9600);
  port.bufferUntil('\n'); 

  cp5 = new ControlP5(this);
  cp5.addTextfield("email").setPosition(150, 100).setSize(100, 30);
  cp5.addButton("Submit").setPosition(180, 150).setSize(40, 20);
  fontik = createFont("light bold", 20);
}

void draw() {
  background(150, 0, 150);
  fill(255, 255, 255);               
  textFont(fontik);
  text("Please, type your email below", 80, 80);

  if (port.available()>0)
  {
    value = port.readStringUntil('\n');
    if (counter<1) {
      println("Opening Process");
      play();
      sendEmail();
    }
    counter = 1;
  }
}

void Submit() {
  print("email submitted");
  email = cp5.get(Textfield.class, "email").getText();
  println();
}


void play() {
  file = new SoundFile(this, "C:/Users/Tatiana/Desktop/studying/SENIORPROJECT/SENIORPROJECT/lullaby_goodnight.mp3");
  file.play();
}
