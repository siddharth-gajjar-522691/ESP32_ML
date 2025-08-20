void Prepare_HTML_page(void);

void Prepare_HTML_page(void){
//    Load_Data();
    Serial.println("preparing web page now");
    DateTime now= rtc.now(); 
   String HTML="";
    HTML.concat("<!DOCTYPE html>");
    HTML.concat("<html lang=\"en\">");
    HTML.concat("<head>");
    HTML.concat("<title>i Technology </title>");
    HTML.concat("<meta charset=\"UTF-8\">");
    HTML.concat("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    HTML.concat("<style>");
    HTML.concat("table,th,td{border: 1px solid black;border-collapse:collapse;padding:15px;margin-left:auto;margin-right:auto;text-align: center;}");
    HTML.concat("body {font-family: Arial;  margin: 0;}");
    HTML.concat(".header {padding: 60px;text-align:center;background: #000000;color:white;font-size: 20px;}");
    HTML.concat(".content {padding:20px;}</style></head><body onload=\"myFunction();\">");
    HTML.concat("<div class=\"header\">  <h1>i Technology</h1><h3>Machine Operation monitor. </h3>");
    HTML.concat("<h6>" + String(now.day())+"/"+String(now.month())+"/"+String(now.year())+','+String(now.hour())+":"+String(now.minute())+":"+String(now.second())+ "<br> core Temp: "+String((temprature_sens_read() - 32) / 1.8)+" C</h6>");
    HTML.concat("<p> Total Machines : ");
    HTML.concat(Machines);
    HTML.concat("</p></div>");
//    HTML.concat("</p></div>");
    HTML.concat("<br><br>");
    

    HTML.concat("<table style=\"width:50%\"><tr><th>Sr No.</th><th>MACHINE ID</th><th>Machine UP TIME</th><th>DOWN TIME</th></tr>"); 
    
    String person_table = "";
    Serial.println("preparing data in person table");
    for(int j=0;j<Machines;j++){
          person_table.concat("<tr><td>");
          person_table.concat(String(j+1));
          person_table.concat("</td><td>");
          person_table.concat(String("Machine ") + String(j+1));      
          person_table.concat("</td><td style=\"background-color:Green;\">");
          
          int m_hr = 0;
          int m_mn = 0;
          int m_sec =0;
         
          int l = machine_data[j][On];
          if(l>59)
          {
            if(l>3600)
            {
              m_hr = l/3600;              
            }
            else
            {
              m_hr = 0;
            }        
            m_mn  = (l%3600)/60;
            m_sec = (l%3600)%60;            
            
            person_table.concat(String(m_hr) + ":" + String(m_mn)+ ":" + String(m_sec));
          }
          else
          {
            person_table.concat("0:" + String(l));
          }
          
          person_table.concat("</td><td style=\"background-color:Red;\">");  
          person_table.concat(String());
          int k = machine_data[j][Off];
          if(k>59)
          {
            if(k>3600)
            {
              m_hr = k/3600;
            }
            else
            {
              m_hr = 0;
            }
            
            m_mn = (k%3600)/60;
            m_sec = (k%3600)%60; 
            person_table.concat(String(m_hr) + ":" + String(m_mn)+ ":" + String(m_sec));
          }
          else
          {
            person_table.concat("0:" + String(k));
          }
          person_table.concat("</td></tr>");
    }
    
    HTML.concat(person_table + "</table>");
    HTML.concat("<br>");
    HTML.concat("Click <a href=\"/S\">SEND SMS</a> to send the sms to Number : ");
    HTML.concat(Phone_Number);
    HTML.concat("<br><br>");
    HTML.concat("<a href=\"/O\">Restore</a>  ");
//    HTML.concat("<script>function myFunction() {setTimeout(function(){ window.location.reload();}, 10000);}</script>");
    HTML.concat("<br><br>");
    HTML.concat("<form action=\"/\"><fieldset>");
    HTML.concat("<label for=\"fname\">Phone Number(10 digit):</label><br>");
    HTML.concat("<input type=\"text\" id=\"fname\" name=\"fname\" value=\""+String(Phone_Number)+"\"><br><br>");
    
    HTML.concat("<label for=\"1name\">Shift time 1:</label><br>");
    HTML.concat("<input type=\"text\" id=\"1name\" name=\"1name\" value=\"");
    HTML.concat(String(Shift1));
    HTML.concat("\"><br><br>");
    
    HTML.concat("<label for=\"2name\">Shift time 2:</label><br>");
    HTML.concat("<input type=\"text\" id=\"2name\" name=\"2name\" value=\"");
    HTML.concat(String(Shift2));
    HTML.concat("\"><br><br>");
    HTML.concat("<input type=\"submit\" value=\"Submit\"></fieldset></form>");

    HTML.concat("<br><br>"); 
    
    HTML.concat("<form action=\"/\"><fieldset>");
    HTML.concat("<label for=\"file_ssid\">SSID : </label><br>");
    HTML.concat("<input type=\"text\" id=\"file_ssid\" name=\"file_ssid\" value=\"");
    HTML.concat(String(file_ssid));
    HTML.concat("\"><br><br>");
    
    
    HTML.concat("<label for=\"file_pwd\">Password : </label><br>");
    HTML.concat("<input type=\"text\" id=\"file_pwd\" name=\"file_pwd\" value=\"");
    HTML.concat(String(file_pwd));
    HTML.concat("\"><br><br>");    
    HTML.concat("<input type=\"submit\" value=\"Connect\"></fieldset></form>");
    
    HTML.concat("<br><br>"); 
    HTML.concat("<footer>");
    HTML.concat("  Mac address  : " + WiFi.macAddress());
    HTML.concat("<br>  local address: ");
    String LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
    HTML.concat(LocalIP);
    
    HTML.concat("<br>Developed By: i Technology ");
    HTML.concat("<p><br>itechnology1242@gmail.com");
    HTML.concat("<br>Surat, India</p>");
    HTML.concat("</footer>");
    HTML.concat("</body></html>");
    Serial.println("returning to main loop");
}
