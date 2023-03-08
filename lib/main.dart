import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'page2.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Linux texture',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  static const platform = MethodChannel('surban.net/texture');

  int? _texture;

  Future<void> _initTexture() async {
    await Future.delayed(const Duration(seconds: 1));

    final int texture = await platform.invokeMethod('newTexture');
    setState(() {
      _texture = texture;
    });
  }

  @override
  void initState() {
    super.initState();
    _initTexture();
  }

  @override
  Widget build(BuildContext context) {
    final texture = _texture;
    //final texture = null;

    return Scaffold(
      appBar: AppBar(
        title: const Text("Click + to trash OpenGL context"),
      ),
      body: texture != null ? Texture(textureId: texture) : const Text("empty"),
      floatingActionButton: FloatingActionButton(
        heroTag: 'data',
        onPressed: () {
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => const Page2()),
          );
        },
        tooltip: 'Trash OpenGL context',
        child: const Icon(Icons.add),
      ),
    );
  }
}
