import 'package:flutter/material.dart';
import 'dart:async';
class loginDetailsShow extends StatelessWidget{
  loginDetailsShow({
    @required this.sysid,
    @required this.passwd,
  });
  final sysid;
  final passwd;
  @override

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: <Widget>[
            Text('System id: ' + sysid),
            Text('Password: ' + passwd),
          ],
        ),
      ),
    );
  }
}