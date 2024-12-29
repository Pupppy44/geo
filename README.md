<p align="center"> 
	<img width="default" height="200" src="https://i.ibb.co/mcBLfy6/geo.png">
</p>

## Geo - 2D Game Engine
Geo is a comprehensive 2D game engine for Windows written in C++. Geo functions similarly to [Roblox](https://roblox.com), where games are designed with XML, programmed in Lua, and can be played either offline or online using its multiplayer networking support. See the [documentation](https://github.com/Pupppy44/geo/blob/master/DOCS.md) for everything you need to know.

## Update Log

### 12/28/2024 (Version 0.14)
- Happy early New Year! Geo is now public.
	- Documentation for Geo added in DOCS.md
- New `textbox` object for text input
- New `scrollbox` object for objects in a scrollable box
	- Additionally, all objects support child objects in their XML
- Rich text tags for text objects (bold, italic, underline, strikethrough, color)
- Added `geo.create_object(type, name)` and `object.destroy()` Lua functions
- Improvements with default properties for objects and error handling

### 10/27/2024 (Version 0.13)
- Data System for storing local save data
- Speech system for text-to-speech
- New `hidden` property for objects
- Studio testing

### 5/29/2024 (Version 0.12)
- New CoreUI for games
- Click and hover callbacks for objects
- New structures for local players (`geo.player`) and game information (`geo.info`)
- Functions for loading games (`geo.load()`) and exiting games (`geo.exit()`)
- New properties for rectangles (outlines), texts (alignment), and images (rotation)
- New API class for accessing Geo APIs
- Support for custom players using Windows profiles
- Improvements for networking and scripts
- Geo Menu testing

### 5/18/2024 (Version 0.11)
- Development for Geo is back up!
    - Images and audios with support for online files
    - Texts and improved rectangles
    - Tween system
    - Server improvements for reflections and player callbacks

### 12/29/2023 (Version 0.10)
- The entire engine has been recoded from scratch with many improvements!
	- Reorganized project structure
	- Improved bindings for objects
	- UI debugger for Client and Server
	- Many more fixes and optimizations
- Server is now functional with new features
	- Better player management (geo.players) and support for kicking players
	- Improved connecting/disconnecting and timeouts for players
	- Separate remote events
- Server Manager testing

### 9/24/2023 (Version 0.09) 
- New sprite class for sprites
- Geo home menu for loading games (+ support for loading games from files)

### 9/23/2023 (Version 0.08)

- Structure reorganization for Geo & GGS (server is currently broken)
- Improved logging
- Vertical Sync
- Smoother tweens (+ memory leaks from those fixed)
- Improved memory management for objects
- CoreUI menu added
- Support for parsing separate XML objects
- Click events for rectangles (all objects soon)
- Visible property for objects
- Exit game functionality
- Studio testing

### 8/31/2023 (Version 0.07)
- Better player objects for server

### 8/30/2023 (Version 0.06)
- Remote event callbacks
- Wait function

### 8/26/2023 (Version 0.05)
- Players metatable (geo.game)
- Precompiled headers
- Remote events
- Server fixes/improvements

### 8/25/2023 (Version 0.04)
- New tweening library + tests
- Network fixes
- Input system (key down)

### 8/24/2023 (Version 0.03)
- Organization improvements

### 8/21/2023 (Version 0.02)
- Audio support
- Various updates and game testing

### 8/18/2023 (Version 0.01)
- Server for hosting games
- Property reflections prototype
- Parser improvements
- Other fixes/enhancements

### 8/17/2023 (Version 0.00)
- First upload!
