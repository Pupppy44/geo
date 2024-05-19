#include "../engine/core/game.h"

int main()
{
	geo::core::game game;
	game.init();
	game.start_local_game("d:\\x\\0.xml");
	//game.start_local_game("<geo name=\"Sandbox\" background=\"#0000ff\"><object class=\"audio\"><property name=\"name\" type=\"2\" value=\"audio\"/><property name=\"path\" type=\"2\" value=\"d:/x/game.wav\"/></object><object class=\"script\"><property name=\"name\" type=\"2\" value=\"script\"/><property name=\"context\" type=\"2\" value=\"default\"/><property name=\"code\" type=\"2\" value=\"geo.game.audio.path='d:/x/bitter.wav'geo.game.audio.play('hello!')local i = 0;while true do print('hi x' .. i) i=i+1 wait(1) end\"/></object></geo>");
	//game.start_online_game("127.0.0.1", 1337);
}