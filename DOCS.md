<p align="center"> 
	<img width="default" height="200" src="https://i.ibb.co/mcBLfy6/geo.png">
</p>

# Geo Documentation

Welcome to the Geo Documentation! The purpose of this Markdown file is to ensure you understand the project structure and functionality flows of the Geo Client, Engine, Server, Manager, and Studio projects.

## Table Of Contents

*   [About Geo](#about-geo)
*   [Key Features](#key-features)
*   [Usage](#usage)
*   [Structure](#structure)
    *   [1. Engine Project Structure](#1-engine-project-structure)
    *   [2. Client Project Structure](#2-client-project-structure)
    *   [3. Server Project Structure](#3-server-project-structure)
    *   [4. Manager Project Structure](#4-manager-project-structure)
    *   [5. Studio Project Structure](#5-studio-project-structure)
    *   [6. Assets Folder](#6-assets-folder)
*   [Flows](#flows)
    *   [1. Geo Client Flow](#1-geo-client-flow)
    *   [2. Geo Server Flow](#2-geo-server-flow)
    *   [3. Geo Manager Flow](#3-geo-manager-flow)
    *   [4. Geo Studio Flow](#4-geo-studio-flow)
*   [Conclusion](#conclusion)

## About Geo

Geo is a comprehensive 2D game engine for Windows written in C++. Geo functions similarly to [Roblox](https://roblox.com), where games are designed with XML, programmed in Lua, and can be played either offline or online using its multiplayer networking support. Geo has the capabilities of creating professional and extensive games that run smoothly both locally and online. Since its private development in August 2023, the engine has advanced with a plethora of features and abilities.

## Key Features

1.  **2D Game Creation:** Build beautiful 2D games, rendered by Direct2D, with XML using a variety of object types, including rectangles, text, images, audio, and scripts, all organized in an hierarchial object tree structure. 
2.  **Lua Scripting:** Implement game logic and object behavior using Lua, allowing for custom game rules, object editing, and dynamic content to create complex, functional games.
3.  **Local & Online Multiplayer:** Supports both offline and online multiplayer games, enabling fast, networked gameplay using the [Pascal](https://github.com/pupppy44/pascal) networking library.
4.  **Server Hosting:** Host your own game servers with the Geo Server, managing player connections, game states, authoritative game logic, remote events, and object reflections to clients.
5.  **Helper Extensions:** Use a variety of helper libraries, also known as systems, in your code, such as HTTP clients, keyboard/mouse handlers, database managers, and animation systems.
6. **Core Graphical Interfaces:** Consists of core graphical interfaces, such as a menu and in-game sidebar, allowing for a unique and polished game experience.
7. **Extensive Support:** Supports extensions of objects, code systems, and overall functionality of the game engine.

## Usage

Games can be played using the Geo Client and hosted using the Geo Server. Games can be played using the Geo Client and hosted using the Geo Server. To play a game locally, launch the `Geo Client` executable with a game's XML file path as an argument, which will call `game.start_local_game(xml_path)`. To join an online game, launch the `Geo Client` and use `game.start_online_game(ip, port)` to connect to a `Geo Server`. To host a game, launch the `Geo Server` executable with a game's XML file path and port number as arguments, which will call `server.load(xml_path)` and `server.start(port)`. The `Geo Manager` can be used to create and manage multiple `Geo Server` instances via its HTTP API, calling `create_server(game_id)`. Finally, the `Geo Studio` allows for visual game design, script editing, and local testing, and can connect to the `Geo Manager` to control servers

## Structure

This section outlines the directory structure and key components of each project within all Geo projects.

### 1. Engine Project Structure

The `Engine` project is the core of the Geo system, and its structure reflects its role as a foundational library. It's organized into several key directories:

*   **`core/`**: This directory contains the fundamental classes that drive the engine.
    *   `game.h/cpp`: The main game class, responsible for initializing and managing a game instance. It handles the game loop, loading, and clearing game states.
    *   `engine.h/cpp`: Manages the Direct2D rendering context, the game object tree, and the overall rendering process. It's the heart of the visual output.
    *   `window.h/cpp`: Handles window creation, input processing (keyboard and mouse), and basic window management.
    *   `parser.h/cpp`: Responsible for parsing game data from XML files, converting the XML structure into game objects.
*   **`code/`**: This directory focuses on the scripting and code execution aspects of the engine.
    *   `runner.h/cpp`: Manages the Lua scripting environment, including loading, running, and clearing scripts. It also handles callbacks from Lua to C++.
    *   `registry.h/cpp`: Handles the binding of C++ objects and functions to the Lua environment, making them accessible from scripts.
    *   `callbacks.h/cpp`: Manages all callback functions used by the engine, and used for communication between Lua and C++.
*   **`objects/`**: This directory defines the base game objects that can be used in games.
    *   This includes classes like `rect`, `text`, `image`, `audio`, and `script`, each representing a different type of game element.
*   **`systems/`**: This directory contains various game systems that provide core functionality.
    *   `input_system.h/cpp`: Handles keyboard and mouse input.
    *   `http_system.h/cpp`: Manages HTTP requests for fetching data from external APIs.
    *   `tween_system.h/cpp`: Provides a tweening system for smooth animations.
    *   `data_system.h/cpp`: Handles saving and loading game data.
    *   `speech_system.h/cpp`: Provides text-to-speech functionality.
*   **`tree/`**: This directory manages the game object hierarchy.
    *   `object.h/cpp`: The base class for all game objects, defining common properties and methods.
    *   `tree.h/cpp`: Manages the tree structure of game objects.
*   **`structs/`**: This directory contains data structures used throughout the engine.
    *   `game_info.h`: Defines the structure for game information.
    *   `local_player.h`: Defines the structure for local player information.
*   **`util/`**: This directory contains utility classes and functions.
    *   `helpers.h/cpp`: Provides helper functions for common tasks like string manipulation, color conversion, and file downloading.
    *   `logger.h/cpp`: Implements a logging mechanism for messages and errors.
    *   `tween.h/cpp`: Implements the tween animation class.

The `Engine` project is designed to be a reusable library, and its structure reflects this by separating core functionality into distinct directories.

### 2. Client Project Structure

The `Client` project is structured to provide a playable game experience.

*   **`main.cpp`**: The entry point for the client application.
*   **`resource.h`**: Stores UI resources (XML files) and the icon.
*   **`core/`**: Includes the core engine components from the `Engine` project.
*   **`network/`**: Contains classes for client-server communication.
    *   `client.h/cpp`: Manages connections to game servers.
    *   `client_reflector.h/cpp`: Handles object creation and property reflection between client and server.

### 3. Server Project Structure

The `Server` project is structured to host multiplayer games.

*   **`main.cpp`**: The server entry point.
*   **`resource.h`**: Stores the UI resources.
*   **`core/`**: Includes the core engine components from the `Engine` project.
*   **`network/`**: Contains classes for server-client communication.
    *   `server_reflector.h/cpp`: Manages object creation and property reflection between server and clients.
    *   `player.h/cpp`: Represents a network player.
    *   `event.h/cpp`: Networked event class for firing events from server to clients (or client to server)
*   **`server/`**: Contains classes for managing the server.
    *   `server.h/cpp`: Creates and maintains the server and the players connected to it.
    *   `player_handler.h/cpp`: Manages the player list and related events.

### 4. Manager Project Structure

The `Manager` project is structured to manage game server instances.

*   **`main.cpp`**: The manager application entry point.
*   **`manager/`**: Contains classes for managing server instances.
    *   `manager.h/cpp`: Manages all the server instances and provides a HTTP API that clients can connect to.
    *   `create_server.h/cpp`: A function that starts a new game server instance.
    *   `get_servers.h/cpp`: A function that retrieves all servers as a JSON string.
    *   `find_port.h/cpp`: A function that finds an available port.
    *   `generate_guid.h/cpp`: A function that generates a GUID.

### 5. Studio Project Structure

The `Studio` project is structured to provide a visual game design environment.

*   **`main.cpp`**: The studio application entry point.
*   **`resource.h`**: Stores UI resources (HTML).
*   **`studio/`**: Contains classes for managing the studio.
    *   `studio.h/cpp`: Main studio class for initializing the Geo environment.
    *   `view.h/cpp`: Manages the WebView2 container and window.
    *   `hook.h/cpp`: Contains the class used for communicating between C++ and Javascript.

This structure section provides a detailed overview of each project's organization, highlighting the purpose of key directories and files. The `Engine` project is described in more detail due to its central role in the Geo system. Let me know if this is what you were looking for!

### 6. Assets Folder

The `Assets` folder contains assets for the Geo Client and Geo Server.

- `ui/`: All UI components.
    - `geo_core_ui`: The Core UI for the Geo Client, containing the in-game sidebar.
    - `geo_menu`: The Geo Home Menu for the Geo Client, used for loading games.
    - `geo_loader`: The Geo Studio UI displayed when loading a game. Unused.
    - `geo_register`: The Geo Studio UI for registering a new account. Unused.

## Flows

This section outlines the typical, chronological flow of execution within the Geo Client and Geo Server, highlighting how they utilize the core engine components. The purpose is to provide a clear, detailed understanding of what the client and server does to function. You can further take a look at the `main.cpp` file for the Geo Client and Geo Server for an example on how the programs are started.

### 1. Geo Client Flow

The Geo Client's flow can be broken down into these key stages:

1.  **Initialization:**
    *   The `main` function initializes the `core::game` object, which in turn initializes the `core::window`, `core::engine`, and `code::runner`.
    *   The `core::window` creates the application window and sets up input handling.
    *   The `core::engine` initializes the Direct2D rendering context and prepares for rendering.
    *   The `code::runner` initializes the Lua scripting environment and registers all C++ objects and functions.
2.  **Game Loading (Local or Online):**
    *   **Local Game:** If a game file path is provided as a command-line argument, `game.start_local_game(xml_path)` is called. This function uses the `core::parser` to parse the XML file, creating game objects and adding them to the `core::engine.tree`.
    *   **Online Game:** If an IP address and port are provided, `game.start_online_game(ip, port)` is called. This function initializes the `network::client`, connects to the server, and joins the game. The server then sends game data to the client.
    * Both methods will eventually have the Core UI loaded, known as a in-game sidebar for reading game information and leaving the game. Local games load the Core UI through its executable resources, while online games have the Geo Server send it through.
3.  **Game Loop:**
    *   The `core::window.start()` function begins the main game loop.
    *   The loop continuously processes messages, renders the game, and handles input.
    *   **Rendering:** The `core::engine.render()` function iterates through the game objects in the `core::engine.tree` and calls their `render()` methods, drawing them to the screen.
    *   **Input Handling:** The `core::window`'s `wndproc` function captures keyboard and mouse input, which is then passed to the `code::runner.callbacks` to trigger Lua callbacks.
    *   **Script Execution:** The `code::runner` executes Lua scripts associated with game objects, updating their properties and behaviors.
    *   **Network Updates (Online):** If connected to a server, the `network::client` receives updates from the server and applies them to the game objects.
4.  **Game Termination:**
    *   When the window is closed, the game loop ends, and the `core::game.clear()` function is called to clean up resources.

### 2. Geo Server Flow

The Geo Server's flow is as follows:

1.  **Initialization:**
    *   The `main` function initializes the `server::server` object, which in turn initializes the `core::game`, `network::server_reflector`, and `server::player_handler`.
    *   The `core::game` initializes the `core::engine` and `code::runner`.
    *   The `code::runner` initializes the Lua scripting environment and registers all C++ objects and functions.
    *   The `network::server_reflector` is initialized to handle communication with clients.
    *   The `server::player_handler` is initialized to manage connected players.
2.  **Game Loading:**
    *   The `server.load(xml_path)` function is called, which uses the `core::parser` to parse the XML file, creating game objects and adding them to the `core::engine.tree`.
3.  **Server Start:**
    *   The `server.start(port)` function starts the server, listening for incoming client connections.
4.  **Client Connection Handling:**
    *   When a client connects, the `server::player_handler.join_player(peer, name, avatar)` function is called to create a new `network::player` object.
    *   The `network::server_reflector.send_game_info(peer)` function sends game information to the client.
    *   The `network::server_reflector.load_game(peer)` function sends all game objects to the client.
5.  **Game Loop (Implicit):**
    *   The server doesn't have a traditional game loop like the client. Instead, it uses a background thread to reflect object changes and process events.
    *   **Object Reflection:** The `network::server_reflector` continuously checks for updated object properties and sends them to connected clients.
    *   **Event Handling:** The `server::svr.on_packet` function listens for incoming packets from clients, including remote events, and calls the appropriate Lua callbacks.
    *   **Script Execution:** The `code::runner` executes Lua scripts associated with game objects, updating their properties and behaviors.
6.  **Client Disconnection Handling:**
    *   When a client disconnects, the `server::svr.on_disconnect` function is called, and the `server::player_handler.remove_player(player, reason)` function removes the player from the game.

### 3. Geo Manager Flow

The Geo Manager's flow is as follows:

1.  **Initialization:**
    *   The `main` function initializes the `manager::manager` object, which sets up the HTTP server using `httplib::Server`.
    *   The `manager::manager` sets up routes for handling HTTP requests, including `/`, `/servers`, and `/create`.
2.  **Server Start:**
    *   The `manager::manager.start()` function starts the HTTP server, listening for incoming requests on port 80.
3.  **HTTP Request Handling:**
    *   When a client sends a request to `/servers`, the `manager::get_servers()` function is called, which serializes the list of active game servers into a JSON string and sends it back to the client.
    *   When a client sends a request to `/create` with a `game_id` parameter, the `manager::create_server(game_id)` function is called. This function:
        *   Generates a unique server ID using `generate_guid()`.
        *   Finds an available port using `find_port()`.
        *   Starts a new `Geo Server` process using `_spawnl`, passing the game ID, port, and server ID as command-line arguments.
        *   Adds the new server information to the list of active servers.
        *   Sends the port number of the new server back to the client.
4.  **Continuous Operation:**
    *   The `httplib::Server` continuously listens for incoming HTTP requests and dispatches them to the appropriate handlers.

### 4. Geo Studio Flow

The Geo Studio's flow is as follows:

1.  **Initialization:**
    *   The `main` function initializes the `studio::studio` object, which in turn initializes the `geo::core::game` (for local previews), `studio::view`, and `studio::hook`.
    *   The `geo::core::game` initializes the `core::engine` and `code::runner` for local game previews.
    *   The `studio::view` initializes the Win32 window, creates the WebView2 environment and controller, and loads the `studio.html` file.
2.  **Studio Start:**
    *   The `studio::view.start()` function shows the window and starts the message loop, which handles window events and keeps the application running.
3.  **WebView2 Interaction:**
    *   The WebView2 control renders the HTML, CSS, and JavaScript of the Geo Studio application.
    *   The JavaScript code in the WebView2 interacts with the C++ code through the `studio::hook` class (though the provided code doesn't show specific hook implementations).
    *   The JavaScript code can use the `core::game` to preview games locally, and can use the `httplib` to connect to the `Geo Manager` to create and manage game servers.
4.  **Continuous Operation:**
    *   The `studio::view` continuously processes window messages, renders the WebView2 content, and handles user input.

## Conclusion

This documentation provides a comprehensive overview of the Geo project structure, key features, usage instructions, and detailed project flows. By understanding the organization of the projects, the purpose of key components, and the execution flow of the client, server, manager, and studio applications, developers can gain a deeper insight into how the Geo engine functions and how its various parts interact. If you have any further questions or need additional information, feel free to create an Issue or Pull Request! I hope this documentation serves as a valuable resource for understanding the Geo project.