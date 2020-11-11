/* Importing all necessary libraries */
import 'dart:convert';
import 'dart:ui';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:iotapp/API/api.dart';
import 'package:iotapp/homePage/HomePage.dart';
import 'loginDetailsShow.dart';
import 'package:http/http.dart' as http;

/* Creating a State less widget class */
class LoginPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return new MaterialApp(
      home: new LoginScreen(),
      theme: new ThemeData(primarySwatch: Colors.blue),
    );
  }
}

class LoginScreen extends StatefulWidget {
  @override
  _LoginPageState createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginScreen>
    with SingleTickerProviderStateMixin {
  final _loginFormKey = GlobalKey<FormState>(); //########################
  final _tempLoginFormKey = GlobalKey<FormState>();
  TextEditingController sysid = TextEditingController();
  TextEditingController passwd = TextEditingController();

  void varList() async {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => loginDetailsShow(
          sysid: sysid.text,
          passwd: passwd.text,
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final Size screenSize = MediaQuery.of(context).size;
    return new Scaffold(
      resizeToAvoidBottomInset: false,
      backgroundColor: Colors.black87,
      appBar: new AppBar(
        title: new Text('CAAS-19'),
        centerTitle: true,
      ),
      body: new Stack(
        //mainAxisAlignment: MainAxisAlignment.center,
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
          // new SingleChildScrollView(
          new Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              new Form(
                key: _loginFormKey,
                child: new Theme(
                  data: new ThemeData(
                      brightness: Brightness.dark,
                      primarySwatch: Colors.teal,
                      inputDecorationTheme: new InputDecorationTheme(
                          labelStyle: new TextStyle(
                        color: Colors.white70,
                        fontSize: 20,
                      ))),
                  child: new Container(
                    padding: const EdgeInsets.all(10.0),
                    child: new Column(
                      crossAxisAlignment: CrossAxisAlignment.center,
                      children: <Widget>[
                        new TextFormField(
                          validator: (value) {
                            if (value.isEmpty) {
                              return 'System ID cannot be empty';
                            }
                            return null;
                          },
                          decoration: new InputDecoration(
                            labelText: "System ID",
                          ),
                          controller: sysid,
                          keyboardType: TextInputType.text,
                        ),
                        new TextFormField(
                          validator: (value) {
                            if (value.isEmpty) {
                              return 'Password cannot be empty';
                            }
                            return null;
                          },
                          decoration: new InputDecoration(labelText: "Password"),
                          controller: passwd,
                          keyboardType: TextInputType.text,
                          obscureText: true,
                        ),
                        new Padding(
                          padding: const EdgeInsets.only(top: 10.0),
                        ),
                        new RaisedButton(
                          shape: RoundedRectangleBorder(borderRadius: new BorderRadius.circular(30.0)),
                          color: Colors.teal.withOpacity(0.4),
                          textColor: Colors.white,
                          child: new Text("Sign in"),
                          onPressed: () async {
                            if (_loginFormKey.currentState.validate()) {
                              Map data = {
                                "SYS_ID":sysid.text,
                                "PASSWORD":passwd.text
                              };
                              // Creating the URL
                              var url = "PLACE YORU URL HERE"##############"http://caas19.loca.lt/auth";
                              http.Response decision = await postRequest(url, data);

                              if(decision.statusCode == 200){
                                var decode = jsonDecode(decision.body);

                                if (decode['check'] == "OK"){
                                  Navigator.push(
                                    context,
                                    new MaterialPageRoute(
                                      builder: (context) => new HomePage(sysid: sysid.text,
                                        passwd: passwd.text,),
                                    ),
                                  );
                                }else{
                                  showDialog(
                                      context: context,
                                      builder: (BuildContext context) {
                                        return AlertDialog(
                                          title: Text("Error"),
                                          content: Text(decode['check']),

                                        );
                                  });
                              };
                              }
                            };
                          },
                          splashColor: Colors.redAccent,
                        ),
                        new Padding(
                          padding: const EdgeInsets.only(top: 10.0),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
            ],
          ),
          //),
        ],
      ),
    );
  }
}
