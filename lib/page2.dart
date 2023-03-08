import 'package:flutter/material.dart';

class Page2 extends StatefulWidget {
  const Page2({super.key});

  @override
  State<Page2> createState() => _Page2State();
}

class _Page2State extends State<Page2> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Page 2"),
      ),
      body: const Text("Page 2"),
      floatingActionButton: FloatingActionButton(
        heroTag: 'data',
        onPressed: () {
          Navigator.pop(context);
        },
        tooltip: 'Back',
        child: const Icon(Icons.add),
      ),
    );
  }
}
