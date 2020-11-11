/* Importing all necessary libraries */
import 'dart:ui';
import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:iotapp/API/api.dart';
import 'package:vibration/vibration.dart';

/* Creating a State full widget class */
class HomePage extends StatefulWidget {

  HomePage({
    @required this.sysid,
    @required this.passwd,
  });
  final sysid;
  final passwd;
  @override
  _HomePageState createState() => _HomePageState(sysid: sysid, passwd: passwd);
}

class _HomePageState extends State<HomePage> {
  List data;
  final sysid;
  final passwd;
  _HomePageState({
    @required this.sysid,
    @required this.passwd,
  });

  @override
  void initState() {
    super.initState();
    setState(() {
      //temp = buildMsgWidget() as String;
      const oneSecond = const Duration(seconds: 10);
      new Timer.periodic(oneSecond, (Timer t) => buildMsgWidget());
    });
  }
  @override
  Widget build(BuildContext context) {
    return new Scaffold(
      //backgroundColor: Colors.black87,
      appBar: new AppBar(
        title: new Text("Home"),
        centerTitle: true,
      ),
      body: new Stack(
        fit: StackFit.expand,
        children: <Widget>[
          new Container(
            constraints: BoxConstraints.expand(),
            decoration: BoxDecoration(
                image: DecorationImage(
                    image: new AssetImage("assets/covid.jpg"),
                    fit: BoxFit.cover
                )
            ),
            child: ClipRRect(
              child: BackdropFilter(
                filter: ImageFilter.blur(sigmaX: 5, sigmaY: 5),
                child: Container(
                  alignment: Alignment.center,
                  color: Colors.grey.withOpacity(0.1),
                ),
              ),
            ),
          ),
          new Container(
            alignment: Alignment.center,

              child: new Text(temp,
              style: TextStyle(fontWeight: FontWeight.w400, color: Colors.white, fontSize: 20),
              ),
              //color: Colors.white,
          ),
        ],
      )
    );
  }
  String temp="refreshing";

  Future<void> buildMsgWidget() async {
    print("IN build msg");
    var url = "PLACE YOUR SERVER URL HERE"; ####################"http://caas19.loca.lt/checkAlert";

    Map data = {
      "SYS_ID":sysid,
      "PASSWORD":passwd
    };
    http.Response decision = await postRequest(url, data);
    if(decision.statusCode == 200){
      var decode = jsonDecode(decision.body);
      if (decode['check'] == "ERROR"){
        temp = 'DB ERROR';
        //print(temp);
        Vibration.vibrate(duration: 2000);
        setState(() {
        });
      }
      else if (decode['check'] == "OK"){
        temp = "OK";
        setState(() {

        });
      }else if (decode["check"] == "SANITIZER"){
        temp = "Entry without Sanitization";
        Vibration.vibrate(duration: 2000);
        setState(() {
        });
      }else if(decode['check'].toString().substring(0, 8) =="DISTANCE")
        //print(decode['check'].toString().substring(9).split(":").join(","))
        temp = "Distance violation at module number: " + decode['check'].toString().substring(9).split(":").join(",");
        Vibration.vibrate(duration: 2000);
        setState(() {
        });
      }
  }
}
