/* Importing all necessary libraries */
import 'package:custom_splash/custom_splash.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'loginScreen/loginScreen.dart';

/*main function for */
void main(){
  Function duringSplash = () {
    int a = 123 + 23;
    print(a);

    if (a > 100)
      return 1;
    else
      return 2;
  };

  Map<int, Widget> op = {1: LoginPage(), 2: LoginPage()};
/* Create first screen and run the app*/
  runApp(
      MaterialApp(
        home: CustomSplash(
          imagePath: 'assets/img1.png',
          backGroundColor: Colors.white,
          animationEffect: 'zoom-in',
          logoSize: 200,
          home: LoginPage(),
          customFunction: duringSplash,
          duration: 2500,
          type: CustomSplashType.StaticDuration,
          outputAndHome: op,
        ),
      )
  );
}