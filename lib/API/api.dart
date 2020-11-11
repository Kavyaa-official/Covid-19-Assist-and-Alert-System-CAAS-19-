/* Importing all necessary libraries */
import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'package:http/http.dart' as http;

/* postRequest function to communicate with python server */
Future<http.Response> postRequest (url, data) async {

  //encode Map to JSON
  var body = json.encode(data);

  var response = await http.post(url,
      headers: {"Content-Type": "application/json"},
      body: body
  );

  return response;
}