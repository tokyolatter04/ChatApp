
# Chat App

## About

This is a TCP chat room which supports real time instant messaging between clients using a custom messaging protocol. This can be ran on a local network or over the internet, the messaging protocol uses a secure key exchange between the server and client and fully encrypts all traffic

## Features

- Realtime instant messaging
- Encrypted communication
- Effective disconnection handling
- Client loads the conversation when it first connects
- Client loads a list of all of the other clients connected

## Messaging Protocol

The custom stream-based messaging protocol is an encrypted JSON based protocol. Each message consists of a header and a body, the header contains the size of the body and a SHA256 hash of the body to ensure the data received is correct. Each packet has its own channel, this is a string in the header which describes what the packet is, for example a ChatMessage object would have the header "message".

Message Packet Structure:
- HEADER_DATA
- PACKET_SPLITTER (Delimiter to seperate the header data from the packet body)
- PACKET_BODY

## Interface

The GUI was programmed in the DearIMGUI library using OpenGL. Users can send messages, view messages and view a list of all of the connected clients

![Capture](https://github.com/tokyolatter04/ChatApp/assets/97055625/25bb09b7-90b8-4be2-803f-b729ff3ec1bf)
