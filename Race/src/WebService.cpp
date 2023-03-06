#include "WebService.h"

#include "Car.h"
#include "Controller.h"
#include "OilObstacle.h"
#include "RampObstacle.h"
#include "ColorUtils.h"
#include "TimeUtils.h"

#include "CSS.h"

using namespace RaceConfig;
using namespace ColorUtils;
using namespace TimeUtils;

WebService *WebService::_instance = NULL;

const char *WebService::_ssid = "openLedRace";
const char *WebService::_password = "1234led56";

const String WebService::_pin_info_html = R"rawliteral(
<div class='w3-bar'>
<br><hr style="height:3px;color:black;background-color:black"><br>
<h2>Pins</h2>
<b>LEDs: </b>)rawliteral" + String(PIN_LED) + "<br>"
+"<b>Gate: </b>" + PIN_CIRCLE + "<br>"
+"<b>Audio: </b>" + PIN_AUDIO + "<br>"
+"<b>Player 1: </b>" + PIN_P1 + "<br>"
+"<b>Player 2: </b>" + PIN_P2 + "<br>"
+"<b>Player 3: </b>" + PIN_P3 + "<br>"
+"<b>Player 4: </b>" + PIN_P4 + "<br></div>";

String WebService::_index_html = "";
String WebService::_players_html = "";
String WebService::_obstacles_html = "";

WebService::WebService() : _server(80)
{
}

WebService &WebService::Instance()
{
    if (_instance == NULL)
    {
        _instance = new WebService();
    }
    return *_instance;
}

void WebService::Init()
{
    //buildIndexHTML();

    Serial.println("Starting web server...");
    WiFi.softAP(WebService::_ssid, WebService::_password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Server IP address: ");
    Serial.println(IP);

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().buildIndexHTML();
                   request->send_P(200, "text/html", _index_html.c_str());
               });

    _server.on("/Start", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   Save();
                   RaceStarted = true;
                   request->redirect("/");
               });

    _server.on("/Stop", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceStarted = false;
                   request->redirect("/");
               });

    _server.on("/player", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyPlayer(request);
                   request->redirect("/#editplayers");
               });

    _server.on("/obstacle", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyObstacle(request);
                   Obstacles.Sort();
                   request->redirect("/#editobstacles");
               });

    _server.on("/general", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyGeneral(request);
                   request->redirect("/");
               });
    
    _server.on("/addoil", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   Obstacles.Add(new OilObstacle(0,10, DEFAULT_OIL_COLOR));
                   Obstacles.Sort();
                   request->redirect("/#editobstacles");
               });
    
    _server.on("/addramp", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   Obstacles.Add(new RampObstacle(0,10, 5, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_HILL));
                   Obstacles.Sort();
                   request->redirect("/#editobstacles");
               });

    _server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceConfig::Save();
                   request->redirect("/");
               });

    _server.on("/load", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceConfig::Load();
                   request->redirect("/");
               });

    _server.on("/deleteconf", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceConfig::Delete();
                   request->redirect("/");
               });

    _server.on("/deleterecord", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   deleteRecord();
                   request->redirect("/");
               });

    _server.on("/boardData", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().buildBoardData();
                   request->send(200, "text/plain", _index_html.c_str() );
               });
               

    _server.begin();
}

void WebService::modifyGeneral( AsyncWebServerRequest *request)
{
    MaxLED = request->getParam("MaxLED")->value().toInt();
    MaxLoops = request->getParam("MaxLoops")->value().toInt();
    EasyMode = request->hasParam("EasyMode");
    track.updateLength(MaxLED);
}

void WebService::modifyPlayer(AsyncWebServerRequest *request)
{
    int index = request->getParam("Index")->value().toInt();
    String newName = request->getParam("Name")->value();
    uint32_t newColor = FromHTMLColor(request->getParam("Color")->value().c_str());

    if (!newName.isEmpty())
    {
        Players[index].setName(const_cast<char *>(newName.c_str()));
    }
    Players[index].setColor(newColor);
    buildIndexHTML();
}

void WebService::modifyObstacle(AsyncWebServerRequest *request)
{
    int index = request->getParam("Index")->value().toInt();
    if( request->hasParam("Delete"))
    {
        Obstacles.Remove(index);
        Obstacles.Sort();
        return;
    }
    IObstacle::ObstacleType type = (IObstacle::ObstacleType)(request->getParam("Type")->value().toInt());
    word start = strtoul(request->getParam("Start")->value().c_str(), NULL, 10);
    word end = strtoul(request->getParam("End")->value().c_str(), NULL, 10);
    uint32_t color = FromHTMLColor(request->getParam("Color")->value().c_str());

    IObstacle* obstacle = Obstacles[index];
    obstacle->setStart(start);
    obstacle->setEnd(end);
    obstacle->setColor(color);

    switch (type)
    {
    case IObstacle::OBSTACLE_OIL:
    {
        OilObstacle *oil = static_cast<OilObstacle *>(obstacle);
        oil->setPressDelay(strtoul(request->getParam("Delay")->value().c_str(), NULL, 10));
    }
    break;

    case IObstacle::OBSTACLE_RAMP:
    {
        RampObstacle *ramp = static_cast<RampObstacle *>(obstacle);
        ramp->setHeight(request->getParam("Height")->value().toInt());
        ramp->setStyle((RampObstacle::RampStyle)request->getParam("Style")->value().toInt());
    }
    break;
    }
}

void WebService::notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void WebService::buildPlayersHTML()
{
    _players_html = "<div id='editplayers' class='w3-bar'><br><hr style='height:3px;color:black;background-color:black'><br><h2>Players</h2>";

    //<form action='/player'>
    //<input type='color' name='Color' value=#COLOR>1 - Name1 <input type='text' name='Name' size = MAX_NAME_LENGTH>
    //<input type='hidden' name='Index' value='i'>
    //<input type='submit'><br>
    //</form><br>
    for (word i = 0; i < Players.Count(); ++i)
    {
        char color[8];
        ToHTMLColor(Players[i].car().getColor(), color);

        _players_html += "<form action='/player'><input type='color' name='Color' value='"
        + String(color)
        + "'> Pin "
        + Players[i].controller().getPin()
        + " <input type='text' name='Name' value='"
        + Players[i].getName()
        + "' size="
        + String(MAX_NAME_LENGTH)
        + "><input type='hidden' name='Index' value='"
        + String(i)
        + "'>";
        if( !RaceStarted)
        {
            _players_html += "<input type='submit'>";
        }
        _players_html += "</form><br>";
    }

    _players_html += "</div>";
}

void WebService::buildObstaclesHTML()
{
    _obstacles_html = "<div id='editobstacles' class='w3-bar'><br><hr style='height:3px;color:black;background-color:black'><br><h2>Obstacles</h2>";

    String typeName;
    String specifics;
    //<form action='/obstaclei'>
    //<span style="background-color: rgba(255, 0, 0, 1);">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>1 - Type: start(%start%) <input type='text' name='Start' size=5>
    // end(%end%) <input type='text' name='End' size=5>
    //<input type='hidden' name='Index' value='i'>
    //<input type='hidden' name='Type' value=%type%>
    //specifics
    //<input type='submit'><br>
    //</form><br>
    for (word i = 0; i < Obstacles.Count(); ++i)
    {
        char color[8];
        ToHTMLColor(Obstacles[i]->getColor(), color);

        switch (Obstacles[i]->getType())
        {
        case IObstacle::OBSTACLE_OIL:
        {
            typeName = "Oil";
            OilObstacle *oil = static_cast<OilObstacle *>(Obstacles[i]);
            //Delay (%delay%): <input type='text' name='Delay' size=4>
            specifics = " Delay <input type='text' name='Delay' value='" + String(oil->getPressDelay()) + "'size=5>";
        }
        break;

        case IObstacle::OBSTACLE_RAMP:
            typeName = "Ramp";
            RampObstacle* ramp = static_cast<RampObstacle *>(Obstacles[i]);
            //Height (%height%): <input type='text' name='Height' size=2>
            //Style <select name='Style'>
            //<option value=0>Style0</option>
            //...
            //<option value=N selected>StyleN</option>
            //</select>
            specifics = " Height <input type='text' name='Height' value='"
            + String(ramp->getHeight())
            + "' size=2>"
            + " Style <select name='Style'>";

            for (int s = 0; s < RampObstacle::RAMP_STYLE_END; ++s)
            {
                specifics += "<option value='" + String(s) + "'";
                if (ramp->getStyle() == s)
                {
                    specifics += " selected";
                }
                specifics += ">" + String(RampObstacle::getStyleName((RampObstacle::RampStyle)s)) + "</option>";
            }
            specifics += "</select>";
            break;
        }

        _obstacles_html += "<form action='/obstacle'><input type='color' name='Color' value='"
        + String(color)
        + "'> "
        + String(i + 1)
        + " - <b>"
        + typeName
        + "</b>: Start <input type='text' name='Start' value='"
        + String(Obstacles[i]->getStart())
        + "' size=5> End <input type='text' name='End' value='" 
        + String(Obstacles[i]->getEnd())
        + "' size=5><input type='hidden' name='Index' value='"
        + String(i)
        + "'><input type='hidden' name='Type' value='"
        + String(Obstacles[i]->getType())
        + "'>"
        + specifics;
        if( !RaceStarted)
        {
            _obstacles_html += R"rawliteral(<input type='submit'>
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>REMOVE</b><input type='checkbox' name='Delete' value='delete'>)rawliteral";
        }
        _obstacles_html += "</form><br>";
    }
    _obstacles_html += "</div>";
    
    //Buttons to add obstacles
    if( !RaceStarted)
    {
        _obstacles_html += R"rawliteral(
        <div class='w3-center'>
        <form style='display:inline-block;' action='/addoil'><input type='submit' value='Add Oil'></form>
        &nbsp;&nbsp;&nbsp;
        <form style='display:inline-block;' action='/addramp'><input type='submit' value='Add Ramp'></form>
        </div>)rawliteral";
    }
}

void WebService::buildIndexHTML()
{
    buildPlayersHTML();
    buildObstaclesHTML();

    _index_html = R"rawliteral(
    <!DOCTYPE html>
    <html lang='fr'>
    <head>
        <title>OpenLedRace Configuration</title>
        <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>
        <style>
        )rawliteral"
    + String(css)
    + R"rawliteral(
        </style>
    </head>
    <body>
    <div class='w3-center w3-padding-16'>
        <H1>Liffab</H1>
        <H2>Open LED Race Configuration</H2>
      </div>
      <div class='w3-card )rawliteral"
    + (RaceStarted ? "w3-green" : "w3-red")
    + R"rawliteral( w3-padding-small w3-jumbo w3-center'>
        <p>Race Status: )rawliteral"
    + (RaceStarted ? "Started" : "Stopped")
    + R"rawliteral(</p>
      </div>
      <div class='w3-center'>
        <a href='/Start' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:40%; height:50%;'>Start</a>
        <a href='/Stop' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:40%; height:50%;'>Stop</a>
      </div>
      <div class='w3-bar'>
      <br><hr style="height:3px;color:black;background-color:black"><br>
      <h2>Records</h2>)rawliteral";

    if( strlen(AllTimeRecord._name) > 0)
    {
        _index_html += + "<h3>Best Time Record</h3><b>"
        + String(AllTimeRecord._name)
        + "</b>: "
        + getTimeString(AllTimeRecord._time);
        + " for "
        + getTimeString( millis() - AllTimeRecord._date);
    }

    if( strlen(CurrentRecord._name) > 0)
    {
        _index_html += "<h3>Last race record</h3><b>"
        + String(CurrentRecord._name)
        + "</b>: "
        + getTimeString(CurrentRecord._time);
    }

    if( strlen(EZAllTimeRecord._name) > 0 )
    {
        _index_html += "<h3>Easy Mode: All time track record</h3><b>"
        + String(EZAllTimeRecord._name)
        + "</b>: "
        + getTimeString(EZAllTimeRecord._time)
        + " for "
        + getTimeString( millis() - EZAllTimeRecord._date);
    }

    if( strlen(EZCurrentRecord._name) > 0 )
    {
        _index_html += "<h3>Easy Mode: Last race record</h3><b>"
        + String(EZCurrentRecord._name)
        + "</b>: "
        + getTimeString(EZCurrentRecord._time);
    }

      _index_html +=   + R"rawliteral(
        <div class='w3-center'>
        <form action='/deleterecord'>
        <input type='submit' value='Delete Record'>
        </form>
        </div>
        </div>
        <div class='w3-bar'>
        <br><hr style="height:3px;color:black;background-color:black"><br>
        <h2>General</h2>
        <form action='/general'>
        Circuit LED count: <input type='text' name='MaxLED' value=')rawliteral"
    + String(MaxLED)
    + R"rawliteral(' size=5>
        <br>
        Number of loops: <input type='text' name='MaxLoops' value=')rawliteral"
    + String(MaxLoops)
    +"' size=2><br>"
    + "<input type='checkbox' name='EasyMode' value='EasyMode'>Easy Mode<br>";

    if(!RaceStarted)
    {
        _index_html += "<input type='submit'>";
    }
    
    _index_html += "</form><br></div>"
    + _players_html
    + _obstacles_html;
    
    //Buttons to add obstacles
    if( !RaceStarted)
    {
        _index_html += R"rawliteral(
        "<div class='w3-center'><br><hr style='height:3px;color:black;background-color:black'><br>
        <h2>Save/Load</h2>
        <form style='display:inline-block;' action='/save'><input type='submit' value='Save Configuration'></form>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <form style='display:inline-block;' action='/load'><input type='submit' value='Load Configuration'></form>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <form style='display:inline-block;' action='/deleteconf'><input type='submit' value='Delete Configuration'></form>
        </div>)rawliteral";
    }

    _index_html += _pin_info_html
    +"</body></html>";
}

void WebService::buildBoardData()
{
    _index_html = "BestRecordName:" + String(AllTimeRecord._name) + "\n" 
    + "BestRecordTime:" + getTimeString(AllTimeRecord._time) + "\n"
    + "BEstRecordColor:" + toString(AllTimeRecord._color) + "\n"
    + "BestRecordDuration:" + getTimeString(millis() - AllTimeRecord._date) + "\n"

    + "CurrentRecordName:" + String(CurrentRecord._name) + "\n" 
    + "CurrentRecordTime:" + getTimeString(CurrentRecord._time) + "\n"
    + "CurrentRecordColor:" + toString(CurrentRecord._color) + "\n"

    + "EasyRecordName:" + String(EZAllTimeRecord._name) + "\n" 
    + "EasyRecordTime:" + getTimeString(EZAllTimeRecord._time) + "\n"
    + "EasyRecordColor:" + toString(EZAllTimeRecord._color) + "\n"
    + "EasyRecordDuration:" + getTimeString(millis() - EZAllTimeRecord._date) + "\n"

    + "EasyCurrentRecordName:" + String(EZCurrentRecord._name) + "\n" 
    + "EasyCurrentRecordTime:" + getTimeString(EZCurrentRecord._time) + "\n"
    + "EasyCurrentRecordColor:" + toString(EZCurrentRecord._color) + "\n"

    + "RaceStarted:" + (RaceStarted ? "1":"0") + "\n";
}